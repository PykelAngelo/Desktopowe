#include <QApplication>
#include <QStyleFactory>
#include <QInputDialog>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);


    QString styleChoice = QInputDialog::getItem(nullptr, "Wybierz motyw", "Motyw:", {"Jasny", "Ciemny"}, 0, false);
    if (styleChoice == "Ciemny") {
        qApp->setStyle(QStyleFactory::create("Fusion"));
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53,53,53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(25,25,25));
        darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(53,53,53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        qApp->setPalette(darkPalette);
    }

    MainWindow w;
    w.setWindowTitle("Menedżer Zadań");
    w.resize(900, 600);
    w.show();
    return a.exec();
}
