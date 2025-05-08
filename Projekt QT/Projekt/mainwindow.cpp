#include "mainwindow.h"
#include <QHeaderView>
#include <QHBoxLayout>
#include <QWidget>
#include <QTableWidgetItem>
#include <QStyle>
#include <QStyleOptionButton>
#include <QPainter>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    Interfejs();

    Termin = new QTimer(this);
    connect(Termin, &QTimer::timeout, this, &MainWindow::SprawdzZakonczenie);
    Termin->start(60000);
}

void MainWindow::Interfejs() {
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout();

    OknoGlowne = new QTableWidget(0, 6, this);
    OknoGlowne->setHorizontalHeaderLabels({"Tytuł", "Opis", "Termin", "Dodano", "Zrobione", "Alert"});
    OknoGlowne->horizontalHeader()->setStretchLastSection(true);
    OknoGlowne->setColumnWidth(1, 400);
    OknoGlowne->setColumnWidth(3, 140);
    OknoGlowne->setColumnWidth(4, 30);
    OknoGlowne->setColumnHidden(5, true);
    OknoGlowne->verticalHeader()->setDefaultSectionSize(30);

    Tytul = new QLineEdit();
    Tytul->setPlaceholderText("Tytuł");
    Opis = new QLineEdit();
    Opis->setMinimumWidth(300);
    Opis->setPlaceholderText("Opis");
    DataDodania = new QDateTimeEdit(QDateTime::currentDateTime());
    DataDodania->setCalendarPopup(true);
    DataDodania->setDisplayFormat("dd.MM.yyyy HH:mm");

    PrzyciskDodaj = new QPushButton("Dodaj zadanie");
    PrzyciskUsun = new QPushButton("Usuń zaznaczone");

    filtr = new QComboBox();
    filtr->addItems({"Wszystkie", "Wykonane", "Niewykonane"});
    connect(filtr, &QComboBox::currentTextChanged, this, &MainWindow::UzyjFiltru);

    statystyki = new QLabel("Statystyki: 0 wykonanych / 0 wszystkich (0%)");

    connect(PrzyciskDodaj, &QPushButton::clicked, this, &MainWindow::DodajZadanie);
    connect(PrzyciskUsun, &QPushButton::clicked, this, &MainWindow::UsunZadanie);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(Tytul);
    inputLayout->addWidget(Opis);
    inputLayout->addWidget(DataDodania);
    inputLayout->addWidget(PrzyciskDodaj);

    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(filtr);
    mainLayout->addWidget(OknoGlowne);
    mainLayout->addWidget(statystyki);
    mainLayout->addWidget(PrzyciskUsun);

    central->setLayout(mainLayout);
    setCentralWidget(central);
}

void MainWindow::DodajZadanie() {
    QString title = Tytul->text();
    QString desc = Opis->text();
    QDateTime datetime = DataDodania->dateTime();

    if (title.isEmpty()) return;

    if (IstniejaceZadania(title)) {
        QMessageBox::warning(this, "Błąd", "Zadanie o takim tytule już istnieje!");
        return;
    }

    DodajZadanieDoTabeli(title, desc, datetime);
    Tytul->clear();
    Opis->clear();
    OdnowStatystyki();
    SortujPoZakonczeniu();
}

bool MainWindow::IstniejaceZadania(const QString &title) {
    for (int i = 0; i < OknoGlowne->rowCount(); ++i) {
        if (OknoGlowne->item(i, 0)->text() == title) {
            return true;
        }
    }
    return false;
}

void MainWindow::DodajZadanieDoTabeli(const QString &title, const QString &desc, const QDateTime &datetime) {
    int row = OknoGlowne->rowCount();
    OknoGlowne->insertRow(row);
    OknoGlowne->setItem(row, 0, new QTableWidgetItem(title));
    OknoGlowne->setItem(row, 1, new QTableWidgetItem(desc));
    OknoGlowne->setItem(row, 2, new QTableWidgetItem(datetime.toString("dd.MM.yyyy HH:mm")));
    OknoGlowne->setItem(row, 3, new QTableWidgetItem(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm")));

    QWidget *checkContainer = new QWidget();
    QCheckBox *check = new QCheckBox();
    connect(check, &QCheckBox::stateChanged, this, &MainWindow::OdnowStatystyki);

    QHBoxLayout *checkLayout = new QHBoxLayout(checkContainer);
    checkLayout->addStretch();
    checkLayout->addWidget(check);
    checkLayout->addStretch();
    checkLayout->setContentsMargins(0, 0, 0, 0);

    checkContainer->setLayout(checkLayout);
    OknoGlowne->setCellWidget(row, 4, checkContainer);
    OknoGlowne->setItem(row, 5, new QTableWidgetItem("0"));
    UzyjFiltru();
}

void MainWindow::UsunZadanie() {
    for (int i = OknoGlowne->rowCount() - 1; i >= 0; --i) {
        if (OknoGlowne->item(i, 0)->isSelected()) {
            OknoGlowne->removeRow(i);
        }
    }
    OdnowStatystyki();
}

void MainWindow::SprawdzZakonczenie() {
    QDateTime now = QDateTime::currentDateTime();
    for (int i = 0; i < OknoGlowne->rowCount(); ++i) {
        QDateTime taskTime = QDateTime::fromString(OknoGlowne->item(i, 2)->text(), "dd.MM.yyyy HH:mm");
        QWidget *checkContainer = OknoGlowne->cellWidget(i, 4);
        QCheckBox *check = checkContainer->findChild<QCheckBox*>();
        QTableWidgetItem *alertItem = OknoGlowne->item(i, 5);

        if (taskTime.isValid() && taskTime < now && !check->isChecked()) {
            if (alertItem && alertItem->text() == "0") {
                QMessageBox::information(this, "Czas minął", "Czas na wykonanie zadania: '" + OknoGlowne->item(i, 0)->text() + "' minął!");
                alertItem->setText("1");
            }
        }
    }
}

void MainWindow::OdnowStatystyki() {
    int total = OknoGlowne->rowCount();
    int done = 0;
    int expired = 0;
    QDateTime now = QDateTime::currentDateTime();

    for (int i = 0; i < total; ++i) {
        QWidget *checkContainer = OknoGlowne->cellWidget(i, 4);
        QCheckBox *check = checkContainer->findChild<QCheckBox*>();
        QDateTime deadline = QDateTime::fromString(OknoGlowne->item(i, 2)->text(), "dd.MM.yyyy HH:mm");

        if (check->isChecked()) {
            done++;
        } else if (deadline.isValid() && deadline < now) {
            expired++;
        }
    }

    int percent = (total > 0) ? (done * 100 / total) : 0;
    statystyki->setText(QString("Statystyki: %1 wykonanych / %2 wszystkich (%3%) | Niewykonane po terminie: %4")
                            .arg(done)
                            .arg(total)
                            .arg(percent)
                            .arg(expired));
}

void MainWindow::UzyjFiltru() {
    QString filter = filtr->currentText();
    for (int i = 0; i < OknoGlowne->rowCount(); ++i) {
        QWidget *checkContainer = OknoGlowne->cellWidget(i, 4);
        QCheckBox *check = checkContainer->findChild<QCheckBox*>();
        bool isDone = check->isChecked();

        bool show = (filter == "Wszystkie") ||
                    (filter == "Wykonane" && isDone) ||
                    (filter == "Niewykonane" && !isDone);

        OknoGlowne->setRowHidden(i, !show);
    }
}

void MainWindow::SortujPoZakonczeniu() {
    OknoGlowne->sortItems(2, Qt::AscendingOrder);
}
