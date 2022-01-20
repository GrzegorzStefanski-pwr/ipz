#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QDateTime>
#include <QDebug>
#include <Qlist>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

// Metody publiczne
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

// Metody prywatne
private slots:
    // Utils
    void addToLogs(QString message);
    void readFromPort();
    void sendMessageToDevice(QString message);
    void sendMessageToDeviceWithoutLogs(QString message);

    // Handler'y przycisków
    void on_pushButtonSearch_clicked();
    void on_pushButtonClearLogs_clicked();
    void on_pushButtonConnect_clicked();
    void on_pushButtonDisconnect_clicked();
    void on_pushButtonSendData_clicked();
    void on_pushButtonRunRobot_clicked();
    void on_pushButtonAcceptPoint_clicked();
    void on_pushButtonGripper_clicked();

// Zmienne prywatne
private:
    Ui::MainWindow *ui;
    QSerialPort *device;
    bool gripperStatus;
};
#endif // MAINWINDOW_H
