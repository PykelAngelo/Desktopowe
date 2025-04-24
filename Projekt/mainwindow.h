#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTimer>
#include <QLabel>
#include <QComboBox>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void DodajZadanie();
    void UsunZadanie();
    void SprawdzZakonczenie();
    void OdnowStatystyki();
    void UzyjFiltru();
    void SortujPoZakonczeniu();

private:
    QTableWidget *OknoGlowne;
    QLineEdit *Tytul;
    QLineEdit *Opis;
    QDateTimeEdit *DataDodania;
    QPushButton *PrzyciskDodaj;
    QPushButton *PrzyciskUsun;
    QLabel *statystyki;
    QComboBox *filtr;
    QTimer *Termin;

    void Interfejs();
    void DodajZadanieDoTabeli(const QString &title, const QString &desc, const QDateTime &datetime);
    bool IstniejaceZadania(const QString &title);
};

#endif
