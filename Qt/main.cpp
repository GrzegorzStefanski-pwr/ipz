#include "mainwindow.h"

#include <QApplication>

// Funkcja startowa
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
