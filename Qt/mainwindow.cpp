#include "mainwindow.h"
#include "ui_mainwindow.h"

// definicje zmiennych stanowych
bool punkt_zdefiniowany = false;
bool dane_przeslane = false;

// przypisanie stanu chwytaka do zmiennych dwustanowych
bool gripper_closed = true;
bool gripper_open = false;

// zmienne globalne
QString tempInterpolation;
QString tempResolution;
QString tempCoRdX;
QString tempCoRdY;
QString tempCoRdZ;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // tworzenie okna, handler'a portu usb i przypisanie początkowego stanu chwytaka
    ui->setupUi(this);
    this->device = new QSerialPort(this);
    this->gripperStatus = gripper_open;
}

MainWindow::~MainWindow()
{
    // usuwanie okna
    delete ui;
}

// Handler przycisku wyszukiwania urządzeń
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

// Dodawanie wiadomości do logów programowych
void MainWindow::addToLogs(QString message)
{
   QString currentDateTime = QDateTime::currentDateTime()
           .toString("yyyy.MM.dd hh:mm:ss");
   ui->textEditLogs->append(currentDateTime + "\t" + message);
}

// Czyszczenie logów programowych
void MainWindow::on_pushButtonClearLogs_clicked()
{
    ui->textEditLogs->clear();
}

// Handler przycisku otwarcia połączenia
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
            // Ustawienie parametrów połączenia
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


// Handler przycisku zakończenia połączenia
void MainWindow::on_pushButtonDisconnect_clicked()
{
    // Sprawdzenie czy połączenie jest otwarte
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

// Czytanie bufforu portu usb
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


// Wysyłanie wiadomości
void MainWindow::sendMessageToDevice(QString message)
{
    // Sprawdzenie czy możliwa jest komunikacja z robotem
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

// Wysyłanie wiadomości bez zachowania logu
void MainWindow::sendMessageToDeviceWithoutLogs(QString message)
{
    // Sprawdzenie czy możliwa jest komunikacja z robotem
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


// Handler przycisku wprowadzania punktu
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


// Handler przycisku wysyłania danych
void MainWindow::on_pushButtonSendData_clicked()
{
    if(this->device->isOpen() && this->device->isWritable())
    {
        if (punkt_zdefiniowany == true)
        {
            this->addToLogs("Wysyłam informacje do urządzenia: "
                            + tempInterpolation + ":" + tempResolution
                            + ":" + tempCoRdX + ":" + tempCoRdY + ":"
                            + tempCoRdZ + "\\n ");
            tempInterpolation.remove(0,1);

            // Wysyłanie komendy do robota
            this->sendMessageToDeviceWithoutLogs(tempInterpolation + ":" + tempResolution + ":"
                                                 + tempCoRdX + ":" + tempCoRdY + ":" + tempCoRdZ + "\n");
            // Aktualizacja zmiennych stanowych
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


// Handeler uruchomienia pracy robota
void MainWindow::on_pushButtonRunRobot_clicked()
{
    // Sprawdzenie czy możliwa jest komunikacja z robotem
    if(this->device->isOpen() && this->device->isWritable())
    {
        if (dane_przeslane == true)
        {
            // Wysyłanie komendy do robota
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

// Hander przycisku obsługi chwytaka
void MainWindow::on_pushButtonGripper_clicked()
{
    // Sprawdzenie czy możliwa jest komunikacja z robotem
    if(!(this->device->isOpen() && this->device->isWritable())){
        this->addToLogs("Port nie jest otwarty!");
        return;
    }

    // Otwieranie lub zamykanie chwytaka w zależności od jego aktualnego stanu
    if (this->gripperStatus == gripper_open){
        this->sendMessageToDeviceWithoutLogs("close_gripper\n");
        this->addToLogs("Zamykam chwytak");
        this->gripperStatus = gripper_closed;
    }
    else {
        this->sendMessageToDeviceWithoutLogs("open_gripper\n");
        this->addToLogs("Otwieram chwytak");
        this->gripperStatus = gripper_open;
    }
}
