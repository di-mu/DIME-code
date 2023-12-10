//------------------------------------------------------------------------------
//
//	File:		MainWindow.cpp
//
//	Abstract:	Main Window Class Implementation
//
//	Version:	0.1
//
//	Date:		02.02.2015
//
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  Section Include Files
//
//------------------------------------------------------------------------------

#include "MainWindow2.h"
#include <QDateTime>
#include "WiMODLRHCI.h"
#include "SerialDevice.h"
#include <cstdio>
#include <stdlib.h>
//------------------------------------------------------------------------------
//
//  TMainWindow - Class Constructor
//
//------------------------------------------------------------------------------

TMainWindow::TMainWindow()
{
    //CreateGUI();
    //cmdConnection_Query();
    //TimerPeriod = 50; // 50ms polling period
    //TimerID     = startTimer(TimerPeriod);
    // register for rx-messages and debug output

    RadioIF.RegisterClient(this);
}

//------------------------------------------------------------------------------
//
//  TMainWindow - Class Desctructor
//
//------------------------------------------------------------------------------

TMainWindow::~TMainWindow()
{
    // close comport
    RadioIF.Close();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  Connection Handling
//
//  @brief: the following section contains connection/comport related functions
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  cmdConnection_Open
//
//  @brief: try to open the selected comport
//
//------------------------------------------------------------------------------

void
TMainWindow::cmdConnection_Open()
{
    char cmd[] = "sudo fuser -k /dev/ttyUSBx";
    printf("Using ttyUSB1...\n");
    QString Port;
    int Port_Number = 1;

    //scanf("%d",&Port_Number);
    cmd[sizeof(cmd)-2] = '0' + Port_Number;
    system(cmd);

    Port.append("ttyUSB");
    Port.append(QString::number(Port_Number));


    if (RadioIF.Open(Port))
    {
        printf("Connection OK\n");
    }
    else
    {
        printf("Connection Error\n");
    }
    printf("-------------------------\n");
}

//------------------------------------------------------------------------------
//
//  cmdConnection_Query
//
//  @brief: query available comports
//
//------------------------------------------------------------------------------

void
TMainWindow::cmdConnection_Query()
{
    ShowCommand("query available comports");

    // clear current comport list
    //Connection.ComPorts->clear();

    // get available comports from device monitor
    QStringList comPorts;

    // get available comports
    TSerialDevice::GetComPorts(comPorts);

    // show result
    ShowMessage("Comports", comPorts);

    // show available comports
    //Connection.ComPorts->addItems(comPorts);
}

//------------------------------------------------------------------------------
//
//  cmdConnection_Close
//
//  @brief: close connection to radio module
//
//------------------------------------------------------------------------------

void
TMainWindow::cmdConnection_Close()
{
    ShowCommand("close connection");

    //Connection.Status->setText("closed");

    RadioIF.Close();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  timerEvent
//
//  @brief: handle comport receiver path in timer event
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void
TMainWindow::timerEvent(QTimerEvent* /* ev */)
{
    // poll radio interface for message reception
    RadioIF.Process();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  Device Management Events
//
//  @brief: the follwoing section contains handlers for device management events
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  cmdDevice_Ping
//
//  @brief: test connection to radio module
//
//------------------------------------------------------------------------------

void
TMainWindow::cmdDevice_Ping()
{
    ShowCommand("ping request");

    if (RadioIF.PingRequest() == WiMODLR_RESULT_OK)
    {
        printf("ok - connection to radio enabled\n");
    }
    else
    {
        printf("error\n");
    }
    printf("-------------------------\n");
}

//------------------------------------------------------------------------------
//
//  cmdDevice_FactoryReset
//
//  @brief: restore factory settings
//
//------------------------------------------------------------------------------

void
TMainWindow::cmdDevice_FactoryReset()
{
    ShowCommand("factory reset request");

    if (RadioIF.FactoryReset() == WiMODLR_RESULT_OK)
    {
        ShowResult("ok - factory settings restored");
    }
    else
    {
        ShowResult("error");
    }
}

//------------------------------------------------------------------------------
//
//  cmdDevice_GetRadioConfiguration
//
//  @brief: get radio configuration
//
//------------------------------------------------------------------------------

void
TMainWindow::cmdDevice_GetRadioConfiguration()
{
    ShowCommand("get radio configuration");

    UINT8 status;

    TWiMODLR_RadioConfig config;

    if (RadioIF.GetRadioConfiguration(config, status) == WiMODLR_RESULT_OK)
    {
        if (status == DEVMGMT_STATUS_OK)
        {
            ShowResult("ok - radio configuration read");

            // convert config to human readable key-value-list
            TKeyValueList list;
            RadioIF.ConvertRadioConfiguration(list, config);
            ShowMessage("radio config", list);
        }
        else
        {
            ShowResult("error - " + QString(RadioIF.GetDeviceMgmtStatusString(status)));
        }
    }
    else
    {
        ShowResult("error - command not sent");
    }
    printf("-------------------------\n");
}

//------------------------------------------------------------------------------
//
//  cmdDevice_SetRadioConfiguration
//
//  @brief: set radio configuration
//
//------------------------------------------------------------------------------

void
TMainWindow::cmdDevice_SetRadioConfiguration(int power)
{
    ShowCommand("set radio configuration");

    UINT8 status;

    static TWiMODLR_RadioConfig config;

    // read current configuration
    if (RadioIF.GetRadioConfiguration(config, status) == WiMODLR_RESULT_OK)
    {
        // data valid ?
        if (status == DEVMGMT_STATUS_OK)
        {
            // change parameters
            // ...
            //config.Frequency = 12451840;            //2.470GHz Channel 24
            config.PowerLevel = power;
            //printf("%d\n", (2480000000/((double)52000000/double(1 << 18))));
            config.Frequency = (2480000000/((double)52000000/double(1 << 18)));
            //config.Frequency = 12477046;            //2.475GHZ Channel 25
            //config.Frequency = 12492170;              //2478
            //config.Frequency = 12494690;              //2478.5
            //config.Frequency = 12497211; //2479 MHz
            //config.Frequency = 12502252;                //2480 Channels 26
            //config.Frequency = 12512334; //2482 MHz
            //config.Frequency = 12517376;               //2483
            //config.Frequency = 12527458;              //2485
            //config.Frequency = 12532499;              //2486
            //config.Frequency = 12557705;              //2491
            //config.Frequency = 12577870;              //2495


            // change spreading factor
            config.SpreadingFactor  = WiMODLR_RADIO_CONFIG_SF5;
            // change error coding
            config.ErrorCoding = WiMODLR_RADIO_CONFIG_EC_4_5;
            // change bandwidth
            config.Bandwidth = WiMODLR_RADIO_CONFIG_BW_1600kHz;

            // write back new configuration to RAM
            if (RadioIF.SetRadioConfiguration(config, WiMODLR_STORE_INTO_RAM, status) == WiMODLR_RESULT_OK)
            {
                // data valid ?
                if (status == DEVMGMT_STATUS_OK)
                {
                    ShowResult("ok - radio configuration changed");
                }
                else
                {
                    ShowResult("error - " + QString(RadioIF.GetDeviceMgmtStatusString(status)));
                }
            }
            else
            {
                ShowResult("error - command not sent");
            }
        }
        else
        {
            ShowResult("error - couldn't read current configuration");
        }
    }
    else
    {
        ShowResult("error - couldn't read current configuration");
    }
    printf("-------------------------\n");
}

void
TMainWindow::cmdDevice_SetRadioConfiguration2(int power)
{
    ShowCommand("set radio configuration");

    UINT8 status;

    TWiMODLR_RadioConfig config;


    config.RadioMode        = RadioIF.old_RadioMode;
    config.GroupAddress     = RadioIF.old_GroupAddress;
    config.TxGroupAddress   = RadioIF.old_TxGroupAddress;
    config.DeviceAddress    = RadioIF.old_DeviceAddress;
    config.TxDeviceAddress  = RadioIF.old_TxDeviceAddress;
    config.Modulation       = RadioIF.old_Modulation;
    config.Frequency        = (2475000000/((double)52000000/double(1 << 18)));
    config.Bandwidth        = WiMODLR_RADIO_CONFIG_BW_1600kHz;
    config.SpreadingFactor  = WiMODLR_RADIO_CONFIG_SF3;
    config.ErrorCoding      = WiMODLR_RADIO_CONFIG_EC_4_5;
    config.PowerLevel       = power;
    config.TxControl        = RadioIF.old_TxControl;
    config.RxControl        = RadioIF.old_RxControl;
    config.RxWindowTime     = RadioIF.old_RxWindowTime;
    config.LEDControl       = RadioIF.old_LEDControl;
    config.RadioOptions     = RadioIF.old_RadioOptions;
    // NEW
    config.FSKDataRate      = RadioIF.old_FSKDataRate;
    config.PowerSavingMode  = RadioIF.old_PowerSavingMode;
    config.LBTThreshold     = RadioIF.old_LBTThreshold;



            // write back new configuration to RAM
    RadioIF.SetRadioConfiguration2(config, WiMODLR_STORE_INTO_RAM, status);

    ShowResult("ok - radio configuration changed");


}

void
TMainWindow::cmdDevice_SetRadioConfiguration3()
{
    ShowCommand("set radio configuration");

    UINT8 status;

    TWiMODLR_RadioConfig config;

    // read current configuration
    if (RadioIF.GetRadioConfiguration(config, status) == WiMODLR_RESULT_OK)
    {
        // data valid ?
        if (status == DEVMGMT_STATUS_OK)
        {
            // change parameters
            // ...
            //config.Frequency = 12451840;            //2.470GHz Channel 24

            //printf("%f\n", (2475000000/((double)52000000/double(1 << 18))));
            config.Frequency = (2475000000/((double)52000000/double(1 << 18)));
            //config.Frequency = 12477046;            //2.475GHZ Channel 25
            //config.Frequency =   12492170;              //2478
            //config.Frequency =   12502252;                //2480 Channels 26
            // config.Frequency = 12517376;               //2483
            //config.Frequency = 12527458;              //2485
            config.PowerLevel = 9;
            // change spreading factor
            config.SpreadingFactor  = WiMODLR_RADIO_CONFIG_SF3;
            // change error coding
            config.ErrorCoding = WiMODLR_RADIO_CONFIG_EC_4_5;
            // change bandwidth
            config.Bandwidth = WiMODLR_RADIO_CONFIG_BW_1600kHz;

            // write back new configuration to RAM
            if (RadioIF.SetRadioConfiguration(config, WiMODLR_STORE_INTO_RAM, status) == WiMODLR_RESULT_OK)
            {
                // data valid ?
                if (status == DEVMGMT_STATUS_OK)
                {
                    ShowResult("ok - radio configuration changed");
                }
                else
                {
                    ShowResult("error - " + QString(RadioIF.GetDeviceMgmtStatusString(status)));
                }
            }
            else
            {
                ShowResult("error - command not sent");
            }
        }
        else
        {
            ShowResult("error - couldn't read current configuration");
        }
    }
    else
    {
        ShowResult("error - couldn't read current configuration");
    }
    printf("-------------------------\n");
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  Radio Events
//
//  @brief: the following section contains handlers for radio events
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  cmdRadio_SendUMessage
//
//  @brief: send unreliable message
//
//------------------------------------------------------------------------------

void
TMainWindow::cmdRadioLink_SendUMessage()
{
    UINT8  txMessage[3 + 8];
    UINT16 numBytes = (UINT16)sizeof(txMessage);

    // take default groupaddress
    UINT8 destGroupAddress = 0x10;

    // take default device address
    UINT16 destDeviceAddress = 0x1234;

    // pre-init TxPayload fields

    // add 8-Bit destination radio group address
    txMessage[0] = destGroupAddress;

    // add 16-Bit destination radio address
    HTON16(&txMessage[1], destDeviceAddress);

    // prepare example payload
    for(int i = 0; i < (numBytes - 3); i++)
    {
        txMessage[3 + i] = i;
    }

    // send unreliable message
    ShowCommand("send unreliable radio message");
    UINT8 status;
    if (RadioIF.SendURadioMessage(txMessage, numBytes, status) == WiMODLR_RESULT_OK)
    {
        if (status == DATALINK_STATUS_OK)
        {
            ShowResult("ok - message sent");
        }
        else
        {
            ShowResult("error - " + QString(RadioIF.GetRadioLinkStatusString(status)));
        }
    }
    else
    {
        ShowResult("error - message not sent");
    }
}
extern UINT8 txMessage[];
//extern UINT16 numBytes;
void
TMainWindow::cmdRadioLink_SendUMessage2(UINT16 pl)
{

    //UINT16 numBytes;
    //numBytes = (UINT16)sizeof(txMessage);
    // send unreliable message
    //ShowCommand("send unreliable radio message");
    //for(int i=0; i<103; i++)
    //    printf("%d",txMessage[i]);
    //printf("%d\n",numBytes);
    UINT8 status;
    //RadioIF.SendURadioMessage2(txMessage, numBytes, status);
    if(pl <= 0){
        return;
    }else{
        RadioIF.SendURadioMessage2(txMessage, pl, status);
    }
}


//------------------------------------------------------------------------------
//
//  evRadio_RxUMessage
//
//  @brief: handle received message
//
//------------------------------------------------------------------------------

void
TMainWindow::evRadioLink_RxUMessage(const TWiMODLR_HCIMessage& rxMsg)
{
    TKeyValueList list;

    // convert radio message into human readable key-value-list
    RadioIF.ConvertRadioRxMessage(list, rxMsg);

    ShowEvent("RadioLink Rx U-Msg", list);
    ShowLine();
}

//------------------------------------------------------------------------------
//
//  evRadio_ShowMessage
//
//  @brief: show radio debug output
//
//------------------------------------------------------------------------------

void
TMainWindow::evRadio_ShowMessage(const QString& prefix, const QString& msg)
{
    ShowMessage(prefix + ":" + msg);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  GUI Functions
//
//  @brief: the following section contains GUI related functions
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  Create GUI
//
//  @brief: create GUI elements, boxes, buttons, ....
//
//------------------------------------------------------------------------------
/*
void
TMainWindow::CreateGUI()
{
    // create list of available comports
    QStringList comPorts;
    comPorts << "COM1" << "COM2" << "COM3";

    // create comport widget
    Connection.ComPorts = new QComboBox();
    Connection.ComPorts->addItems(comPorts);
    Connection.ComPorts->setCurrentIndex(0);

    // connection handling
    QPushButton* button_open    = CreatePushButton("Open", "open connection", this, SLOT(cmdConnection_Open()));
    QPushButton* button_close   = CreatePushButton("Close", "close connection", this, SLOT(cmdConnection_Close()));
    QPushButton* button_query   = CreatePushButton("Query", "query available comports", this, SLOT(cmdConnection_Query()));
    QPushButton* button_ping    = CreatePushButton("Test / Ping", "test connection to radio", this, SLOT(cmdDevice_Ping()));

    // configuration
    QPushButton* button_factoryReset = CreatePushButton("Factory Reset", "restore factory settings", this, SLOT(cmdDevice_FactoryReset()));
    QPushButton* button_getConfig    = CreatePushButton("Get Configuration", "get radio configuration", this, SLOT(cmdDevice_GetRadioConfiguration()));
    QPushButton* button_setConfig    = CreatePushButton("Set Configuration", "set radio configuration", this, SLOT(cmdDevice_SetRadioConfiguration()));

    // radio link services
    QPushButton* button_sendUMsg = CreatePushButton("Send unreliable Message",  "send unreliable message", this, SLOT(cmdRadioLink_SendUMessage()));

    Connection.Status = new QLineEdit("closed");
    Connection.Status->setReadOnly(true);

    // create log window
    LogWindow.Box = CreateLogWindow();
    LogWindow.SingleLineOption  = new QCheckBox("show lists as single line");

    // create layouts and add widgets

    // connection handling
    QHBoxLayout* hl1 = new QHBoxLayout();
    hl1->addWidget(new QLabel("Select Comport"));
    hl1->addWidget(Connection.ComPorts);
    hl1->addWidget(button_query);
    hl1->addWidget(button_open);
    hl1->addWidget(button_ping);
    hl1->addWidget(button_close);
    hl1->addStretch(1);

    QHBoxLayout* hl2 = new QHBoxLayout();
    hl2->addWidget(new QLabel("Status"));
    hl2->addWidget(Connection.Status);

    // configuration handling
    QHBoxLayout* hl3 = new QHBoxLayout();
    hl3->addWidget(button_factoryReset);
    hl3->addWidget(button_getConfig);
    hl3->addWidget(button_setConfig);
    hl3->addStretch(1);

    // radio link services
    QHBoxLayout* hl4 = new QHBoxLayout();
    hl4->addWidget(button_sendUMsg);
    hl4->addStretch(1);


    // vertical layout
    QVBoxLayout* vl = new QVBoxLayout();
    vl->addWidget(new QLabel("Connection"));
    vl->addLayout(hl1);
    vl->addLayout(hl2);
    vl->addWidget(CreateHLine());

    vl->addWidget(new QLabel("Configuration"));
    vl->addLayout(hl3);
    vl->addWidget(CreateHLine());

    vl->addWidget(new QLabel("Radio Link Service"));
    vl->addLayout(hl4);
    vl->addWidget(CreateHLine());
    vl->addWidget(new QLabel("Status Log"));
    vl->addWidget(LogWindow.SingleLineOption);
    vl->addWidget(LogWindow.Box);



    // create main widget
    QWidget* mainWidget = new QWidget();
    mainWidget->setLayout(vl);

    // set the central widget
    setCentralWidget(mainWidget);
}
*/
//------------------------------------------------------------------------------
//
//  CreatePushButton
//
//  @brief: create a pushbutton
//
//------------------------------------------------------------------------------
/*
QPushButton*
TMainWindow::CreatePushButton(const QString& title, const QString& toolTip, const QObject* receiver, const char* slot)
{
    QPushButton* button = new QPushButton(title);

    QObject::connect(button, SIGNAL(clicked()), receiver, slot);

    button->setToolTip(toolTip);

    return button;
}
*/
//------------------------------------------------------------------------------
//
//  CreateHLine
//
//  @brief: create a horizontal seperation line
//
//------------------------------------------------------------------------------
/*
QFrame*
TMainWindow::CreateHLine()
{
    QFrame* frame = new QFrame();

    frame->setFrameStyle(QFrame::HLine);
    QPalette p = frame->palette();
    p.setColor(QPalette::WindowText, QColor(Qt::black));
    frame->setPalette(p);


    return frame;
}
*/
//------------------------------------------------------------------------------
//
//  CreateLogWindow
//
//  @brief: create a log window for debug output
//
//------------------------------------------------------------------------------
/*
QTextEdit*
TMainWindow::CreateLogWindow()
{
    QTextEdit* edit = new QTextEdit();
    edit->setFrameStyle(QFrame::NoFrame);
    edit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    edit->ensureCursorVisible();
    edit->setReadOnly(true);

    return edit;
}
*/
//------------------------------------------------------------------------------
//
//  ShowMessage
//
//  @brief: show debug output
//
//------------------------------------------------------------------------------

void
TMainWindow::ShowMessage(const QString& msg)
{
    //QDate   date = QDate::currentDate();
    //QString dateString = date.toString(tr("yyyy-MM-dd"));

    //QTime   time = QTime::currentTime();
    //QString timeString = time.toString(tr("hh:mm:ss.zzz"));

    //LogWindow.Box->append(dateString + ";" + timeString + ";" + msg);
    printf(msg.toStdString().c_str());
    printf("\n");
}

//------------------------------------------------------------------------------
//
//  ShowMessage
//
//  @brief: show debug output
//
//------------------------------------------------------------------------------

void
TMainWindow::ShowMessage(const QString& prefix, const QStringList& list)
{
    if(1)
    {
        QString msg = prefix + ":" + list.join(";");
        ShowMessage(msg);
    }
    else
    {
        for(int i = 0; i < list.count(); i++)
        {
            QString msg = prefix + ":" + list[i];
            ShowMessage(msg);
        }
    }
}

//------------------------------------------------------------------------------
//
//  ShowCommand
//
//  @brief: show debug output
//
//------------------------------------------------------------------------------

void
TMainWindow::ShowCommand(const QString& cmd)
{
    ShowMessage("Command:" + cmd);
}

//------------------------------------------------------------------------------
//
//  ShowResult
//
//  @brief: show debug output
//
//------------------------------------------------------------------------------

void
TMainWindow::ShowResult(const QString& result)
{
    ShowMessage("Result:" + result);
}

//------------------------------------------------------------------------------
//
//  ShowEvent
//
//  @brief: show debug output
//
//------------------------------------------------------------------------------

void
TMainWindow::ShowEvent(const QString& event, const QStringList& list)
{
    ShowMessage("Event:" + event, list);
}

//------------------------------------------------------------------------------
//
//  ShowLine
//
//  @brief: show line
//
//------------------------------------------------------------------------------

void
TMainWindow::ShowLine()
{

}

//------------------------------------------------------------------------------
//  end of file
//------------------------------------------------------------------------------
