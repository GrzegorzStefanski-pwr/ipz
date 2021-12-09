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

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonSearch_clicked();
    void addToLogs(QString message);
    void on_pushButtonClearLogs_clicked();
    void on_pushButtonConnect_clicked();
    void on_pushButtonDisconnect_clicked();
    void readFromPort();
    void sendMessageToDevice(QString message);
    void sendMessageToDeviceWithoutLogs(QString message);

    void on_pushButtonSendData_clicked();
    void on_pushButtonRunRobot_clicked();
    void on_pushButtonAcceptPoint_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *device;
};
#endif // MAINWINDOW_H
