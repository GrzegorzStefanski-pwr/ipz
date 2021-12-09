#include "mainwindow.h"
#include "ui_mainwindow.h"

bool punkt_zdefiniowany = false;
bool dane_przeslane = false;
QString tempInterpolation;
QString tempResolution;
QString tempCoRdX;
QString tempCoRdY;
QString tempCoRdZ;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->device = new QSerialPort(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButtonSearch_clicked()
{
   this->addToLogs("Szukam urządzeń...");
   ui->comboBoxDevices->clear();
   QList<QSerialPortInfo> devices;
   devices = QSerialPortInfo::availablePorts();
   for(int i=0; i < devices.count(); i++)
   {
       this->addToLogs("Znalazłem urządzenie: " + devices.at(i).portName()
                       + " " + devices.at(i).description());
       ui->comboBoxDevices->addItem(devices.at(i).portName()
                                    + "\t" + devices.at(i).description());
   }
}


void MainWindow::addToLogs(QString message)
{
   QString currentDateTime = QDateTime::currentDateTime()
           .toString("yyyy.MM.dd hh:mm:ss");
   ui->textEditLogs->append(currentDateTime + "\t" + message);
}


void MainWindow::on_pushButtonClearLogs_clicked()
{
    ui->textEditLogs->clear();
}


void MainWindow::on_pushButtonConnect_clicked()
{
    if(ui->comboBoxDevices->count() == 0)
    {
        this->addToLogs("Nie wykryto żadnych urządzeń!");
        return;
    }

    QString portName = ui->comboBoxDevices->currentText().split("\t").first();
    this->device->setPortName(portName);

    //konfiguracja portu i jego otwarcie
    if(!device->isOpen())
    {
        if(device->open(QSerialPort::ReadWrite))
        {
            this->device->setBaudRate(QSerialPort::Baud115200);
            this->device->setDataBits(QSerialPort::Data8);
            this->device->setParity(QSerialPort::NoParity);
            this->device->setStopBits(QSerialPort::OneStop);
            this->device->setFlowControl(QSerialPort::NoFlowControl);

            this->addToLogs("Otwarto port szeregowy.");

            //laczenie sygnału zawierajacego dane do odczytu z funkcja czytania portu
            connect(this->device, SIGNAL(readyRead()), this, SLOT(readFromPort()));

        } else
        {
            this->addToLogs("Otwarcie portu szeregowego nie powiodło się!");
        }
    } else
    {
        this->addToLogs("Port już jest otwarty!");
        return;
    }
}


void MainWindow::on_pushButtonDisconnect_clicked()
{
    if(this->device->isOpen())
    {
        this->device->close();
        this->addToLogs("Zamknięto połączenie.");
    } else
    {
        this->addToLogs("Port nie jest otwarty!");
        return;
    }
}


void MainWindow::readFromPort()
{
    while(this->device->canReadLine())
    {
        QString line = this->device->readLine();
        QString terminator = "\r";
        int pos = line.lastIndexOf(terminator);
        this->addToLogs(line.left(pos));
    }
}


void MainWindow::sendMessageToDevice(QString message)
{
    if(this->device->isOpen() && this->device->isWritable())
    {
        QString msg = message;
        msg.chop(1);
        this->addToLogs("Wysyłam informacje do urządzenia: " + msg);
        this->device->write(message.toStdString().c_str());
    } else
    {
        this->addToLogs("Port nie jest otwarty!");
    }
}


void MainWindow::sendMessageToDeviceWithoutLogs(QString message)
{
    if(this->device->isOpen() && this->device->isWritable())
    {
        QString msg = message;
        msg.chop(1);
        this->device->write(message.toStdString().c_str());
    } else
    {
        this->addToLogs("Port nie jest otwarty!");
    }
}


void MainWindow::on_pushButtonAcceptPoint_clicked()
{
    if(this->device->isOpen() && this->device->isWritable())
    {
        // przesyłanie rodzaju interpolacji
        if (ui->lineEditInterpolation->text() == "G1" ||
                ui->lineEditInterpolation->text() == "G2" ||
                    ui->lineEditInterpolation->text() == "G3")
        {
            tempInterpolation = ui->lineEditInterpolation->text();

            // przesylanie rozdzielczosci interpolacji
            tempResolution = ui->lineEditResolution->text();
            bool convert_done_properly = false;
            int tempResolution_int;
            tempResolution_int = tempResolution.
                    toInt(&convert_done_properly);
            if (convert_done_properly == true &&
                    (tempResolution_int >= 0 && tempResolution_int <= 10))
            {

                // przesyłanie wspolrzednej X
                tempCoRdX = ui->lineEditCoRdX->text();
                bool convert_done_properly = false;
                float tempCoRdX_float;
                tempCoRdX_float = tempCoRdX.
                        toFloat(&convert_done_properly);
                if (convert_done_properly == true &&
                        (tempCoRdX_float >= -50.0F && tempCoRdX_float <= 50.0F))
                {

                    //przesylanie wspolrzednej Y
                    tempCoRdY = ui->lineEditCoRdY->text();
                    bool convert_done_properly = false;
                    float tempCoRdY_float;
                    tempCoRdY_float = tempCoRdY.
                            toFloat(&convert_done_properly);
                    if (convert_done_properly == true &&
                         (tempCoRdY_float >= -50.0F && tempCoRdY_float <= 50.0F))
                    {

                        //przesylanie wspolrzednej Z
                        tempCoRdZ = ui->lineEditCoRdZ->text();
                        bool convert_done_properly = false;
                        float tempCoRdZ_float;
                        tempCoRdZ_float = tempCoRdZ.
                                toFloat(&convert_done_properly);
                        if (convert_done_properly == true &&
                             (tempCoRdZ_float >= -50.0F && tempCoRdZ_float <= 50.0F))
                        {

                            this->addToLogs("Punkt zdefiniowano poprawnie.");
                            punkt_zdefiniowany = true;

                        } else
                        {
                            this->addToLogs
                                    ("Podałes niepoprawną wartość współrzędnej Z!");
                        }

                    } else
                    {
                        this->addToLogs
                                ("Podałes niepoprawną wartość współrzędnej Y!");
                    }

                } else
                {
                    this->addToLogs
                            ("Podałes niepoprawną wartość współrzędnej X!");
                }

            } else
            {
                this->addToLogs
                        ("Podałes niepoprawną wartość rozdzielczosci interpolacji!");
            }

        } else
        {
            this->addToLogs("Wybrałeś niepoprawny tryb interpolacji!");
        }

    } else
    {
        this->addToLogs("Port nie jest otwarty!");
    }
}


void MainWindow::on_pushButtonSendData_clicked()
{
    if(this->device->isOpen() && this->device->isWritable())
    {
        if (punkt_zdefiniowany == true)
        {
            this->addToLogs("Wysyłam informacje do urządzenia: " + tempInterpolation + ":" + tempResolution + ":" + tempCoRdX + ":" + tempCoRdY + ":" + tempCoRdZ + "\\n ");
            tempInterpolation.remove(0,1);
            this->sendMessageToDeviceWithoutLogs(tempInterpolation + ":" + tempResolution + ":" + tempCoRdX + ":" + tempCoRdY + ":" + tempCoRdZ + "\n");

            punkt_zdefiniowany = false;
            dane_przeslane = true;
        } else
        {
            this->addToLogs("Nie zdefiniowano punktu!");
        }
    } else
    {
        this->addToLogs("Port nie jest otwarty!");
    }
}


void MainWindow::on_pushButtonRunRobot_clicked()
{
    if(this->device->isOpen() && this->device->isWritable())
    {
        if (dane_przeslane == true)
        {
            this->sendMessageToDevice("ruch_efektorem\n");
            dane_przeslane = false;
        } else
        {
            this->addToLogs("Nie przesłano danych!");
        }
    } else
    {
        this->addToLogs("Port nie jest otwarty!");
    }
}

