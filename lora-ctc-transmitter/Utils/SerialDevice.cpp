//------------------------------------------------------------------------------
//
//	File:		SerialDevice.cpp
//
//	Abstract:	Serial Device Wrapper Class Implementation
//
//	Version:	0.1
//
//	Date:		09.02.2015
//
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//------------------------------------------------------------------------------

#include "SerialDevice.h"

#ifdef Q_OS_WIN
    // used to query COM ports
    #include "RegistryKey.h"
#elif defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    #include <QDir>
#endif

//------------------------------------------------------------------------------
//
//  TSerialDevice - Class Constructor
//
//------------------------------------------------------------------------------

TSerialDevice::TSerialDevice()
{
    // init handle
    ComHandle = INVALID_HANDLE_VALUE;
}

//------------------------------------------------------------------------------
//
//  ~TSerialDevice - Class Destructor
//
//------------------------------------------------------------------------------

TSerialDevice::~TSerialDevice()
{
    // perform close
    Close();
}

//------------------------------------------------------------------------------
//
//  Open
//
//  @brief: open comport
//
//------------------------------------------------------------------------------

bool
TSerialDevice::Open(const QString& comPort, UINT32 baudRate, int bits, UINT8 parity)
{
    if (ComHandle != INVALID_HANDLE_VALUE)
        Close();

#ifdef Q_OS_WIN
    char devName[80];

    // check if COM Port is higher than COM9
    strcpy(devName, "\\\\.\\");
    strcat(devName, comPort.toUtf8());

    ComHandle = CreateFileA(devName,
                            GENERIC_WRITE | GENERIC_READ,
                            0,
                            NULL,
                            OPEN_EXISTING,
                            0, //FILE_FLAG_WRITE_THROUGH, //0
                            NULL);

    if(ComHandle != INVALID_HANDLE_VALUE)
    {
        DCB dcb;
        if (GetCommState(ComHandle, &dcb))
        {
            dcb.DCBlength           = sizeof(DCB);
            dcb.BaudRate            = baudRate;
            dcb.ByteSize            = bits;
            dcb.Parity              = parity; //EVENPARITY;// NOPARITY;
            dcb.StopBits            = ONESTOPBIT;
            dcb.fOutxCtsFlow        = FALSE;
            dcb.fOutxDsrFlow        = FALSE;
            dcb.fDtrControl         = DTR_CONTROL_DISABLE;
            dcb.fDsrSensitivity     = FALSE;
            dcb.fTXContinueOnXoff   = FALSE;
            dcb.fOutX               = FALSE; // no XON/XOFF
            dcb.fInX                = FALSE; // no XON/XOFF
            dcb.fErrorChar          = FALSE;
            dcb.fNull               = FALSE;
            dcb.fRtsControl         = RTS_CONTROL_DISABLE;
            dcb.fAbortOnError       = FALSE;

            if (SetCommState(ComHandle, &dcb))
            {
                COMMTIMEOUTS commTimeouts;
                commTimeouts.ReadIntervalTimeout 		= MAXDWORD;
                commTimeouts.ReadTotalTimeoutMultiplier = 0;
                commTimeouts.ReadTotalTimeoutConstant 	= 0;

                commTimeouts.WriteTotalTimeoutMultiplier = 10;
                commTimeouts.WriteTotalTimeoutConstant = 1;

                SetCommTimeouts(ComHandle, &commTimeouts);

                return true;
            }
        }
        Close();
    }

#elif defined( Q_OS_MAC )

    // Open the serial port read/write, with no controlling terminal,
    // and don't wait for a connection.
    // The O_NONBLOCK flag also causes subsequent I/O on the device to
    // be non-blocking.
    // See open(2) ("man 2 open") for details.

    QString portName = QString("/dev/") + comPort;
    QByteArray name  = portName.toLocal8Bit();
    //const char* ptr = "/dev/tty.usbserial-IMST2"; //name.constData();
    const char* ptr =name.constData();

    ComHandle = ::open(ptr, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (ComHandle != INVALID_HANDLE_VALUE)
    {
        if (::ioctl(ComHandle, TIOCEXCL) != INVALID_HANDLE_VALUE)
        {
            // Get the current options and save them so we can restore the
            // default settings later.
            struct termios _originalTTYAttrs;
            struct termios options;

            if (::tcgetattr(ComHandle, &_originalTTYAttrs) != INVALID_HANDLE_VALUE)
            {
                // The serial port attributes such as timeouts and baud rate are set by
                // modifying the termios structure and then calling tcsetattr to
                // cause the changes to take effect. Note that the
                // changes will not take effect without the tcsetattr() call.
                // See tcsetattr(4) ("man 4 tcsetattr") for details.
                options = _originalTTYAttrs;
                // Print the current input and output baud rates.
                // See tcsetattr(4) ("man 4 tcsetattr") for details.
                cfmakeraw(&options);
                options.c_cc[VMIN] = 1;
                options.c_cc[VTIME] = 10;
                // The baud rate, word length, and handshake options can be set as follows:
                cfsetspeed(&options, baudRate);           // Set 57600 baud
                // cfsetspeed(&options, B115200);           // Set 115200 baud
                // Disable parity (even parity if PARODD
                options.c_cflag &= ~(CSTOPB | CRTSCTS | PARENB );
                // add flags for parity and word size
                options.c_cflag |= (CSIZE | parity);
                options.c_cflag |= (bits|CLOCAL|CREAD);               // Use n bit words
                options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
                options.c_iflag &= ~(IXOFF | IXON | INPCK | OPOST);
                // Cause the new options to take effect immediately.
                if (tcsetattr(ComHandle, TCSANOW, &options) != INVALID_HANDLE_VALUE)
                {
                    return true;
                }
            }
        }
        Close();
    }
#elif defined( Q_OS_LINUX )

    // Open the serial port read/write, with no controlling terminal,
    // and don't wait for a connection.
    // The O_NONBLOCK flag also causes subsequent I/O on the device to
    // be non-blocking.
    // See open(2) ("man 2 open") for details.

    QString portName = QString("/dev/") + comPort;
    QByteArray name  = portName.toLocal8Bit();
    //const char* ptr = "/dev/tty.usbserial-IMST2"; //name.constData();
    const char* ptr =name.constData();

    ComHandle = ::open(ptr, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (ComHandle != INVALID_HANDLE_VALUE)
    {
        if (::ioctl(ComHandle, TIOCEXCL) != INVALID_HANDLE_VALUE)
        {
            // Get the current options and save them so we can restore the
            // default settings later.
            struct termios _originalTTYAttrs;
            struct termios options;

            if (::tcgetattr(ComHandle, &_originalTTYAttrs) != INVALID_HANDLE_VALUE)
            {
                // The serial port attributes such as timeouts and baud rate are set by
                // modifying the termios structure and then calling tcsetattr to
                // cause the changes to take effect. Note that the
                // changes will not take effect without the tcsetattr() call.
                // See tcsetattr(4) ("man 4 tcsetattr") for details.
                options = _originalTTYAttrs;
                // Print the current input and output baud rates.
                // See tcsetattr(4) ("man 4 tcsetattr") for details.
                cfmakeraw(&options);
                options.c_cc[VMIN] = 1;
                options.c_cc[VTIME] = 10;
                // The baud rate, word length, and handshake options can be set as follows:
                cfsetspeed(&options, baudRate);           // Set 57600 baud
                // cfsetspeed(&options, B115200);           // Set 115200 baud
                // Disable parity (even parity if PARODD
                options.c_cflag &= ~(CSTOPB | CRTSCTS | PARENB );
                // add flags for parity and word size
                options.c_cflag |= (CSIZE | parity);
                options.c_cflag |= (bits|CLOCAL|CREAD);               // Use n bit words
                options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
                options.c_iflag &= ~(IXOFF | IXON | INPCK | OPOST);
                // Cause the new options to take effect immediately.
                if (tcsetattr(ComHandle, TCSANOW, &options) != INVALID_HANDLE_VALUE)
                {
                    return true;
                }
            }
        }
        Close();
    }
#endif
    return false;
}
//------------------------------------------------------------------------------
//
//  Close
//
//  @brief: close comport
//
//------------------------------------------------------------------------------
bool
TSerialDevice::Close()
{
#ifdef Q_OS_WIN
    if(ComHandle != INVALID_HANDLE_VALUE)
    {
        CancelIo(ComHandle);
        Sleep(100);
        CloseHandle(ComHandle);

        Sleep(100);

        ComHandle = INVALID_HANDLE_VALUE;
        return true;
    }
#elif defined( Q_OS_MAC )

    if(ComHandle != INVALID_HANDLE_VALUE)
    {
        ::close(ComHandle);

        ComHandle = INVALID_HANDLE_VALUE;
        return true;
    }
#elif defined( Q_OS_LINUX )

    if(ComHandle != INVALID_HANDLE_VALUE)
    {
        ::close(ComHandle);

        ComHandle = INVALID_HANDLE_VALUE;
        return true;
    }
#endif
    return false;
}

//------------------------------------------------------------------------------
//
//  SendData
//
//  @brief  send a block of characters
//
//------------------------------------------------------------------------------

bool
TSerialDevice::SendData(UINT8* data, int txLength)
{

    if(ComHandle == INVALID_HANDLE_VALUE)
        return false;

#ifdef Q_OS_WIN
    UINT32  numTxBytes;
    if(!WriteFile(ComHandle, data, txLength, (DWORD*)&numTxBytes, 0))
    {
        //DWORD lastError = GetLastError();
        return false;
    }
    if (numTxBytes == (UINT32)txLength)
    {
        return true;
    }
#elif defined( Q_OS_MAC )

    size_t  numTxBytes = ::write(ComHandle, data, txLength);

    if (numTxBytes == txLength)
    {
        return true;
    }

#elif defined( Q_OS_LINUX )

    size_t  numTxBytes = ::write(ComHandle, data, txLength);

    if (numTxBytes == (size_t)txLength)
    {
        return true;
    }

#endif
    return false;

}

//------------------------------------------------------------------------------
//
//  ReadData
//
//  @brief  read a block of characters
//
//------------------------------------------------------------------------------

int
TSerialDevice::ReadData(UINT8* rxBuffer, int bufferSize)
{
    // handle ok ?
    if(ComHandle == INVALID_HANDLE_VALUE)
        return 0;

#ifdef  Q_OS_WIN
    DWORD numRxBytes = 0;
    if (ReadFile(ComHandle, rxBuffer, bufferSize, &numRxBytes, 0))
    {
        return (int)numRxBytes;
    }
#elif defined ( Q_OS_MAC )
    ssize_t  numRxBytes = ::read(ComHandle, rxBuffer, bufferSize);
    if(numRxBytes > 0)
        return (int)numRxBytes;
#elif defined ( Q_OS_LINUX )
    ssize_t  numRxBytes = ::read(ComHandle, rxBuffer, bufferSize);
    if(numRxBytes > 0)
        return (int)numRxBytes;
#endif

    return 0;
}


//------------------------------------------------------------------------------
//
//  GetComPorts
//
//  @brief  return list of available comports
//
//------------------------------------------------------------------------------

#ifdef  Q_OS_WIN
int
TSerialDevice::GetComPorts(QStringList& portList)
{
    TRegistryKey key1;
    key1.LocalMachine();

    TRegistryKey* key2 = key1.OpenSubKey("HARDWARE\\DEVICEMAP\\SERIALCOMM");
    if(key2)
    {
        key2->GetKeyValues(portList);
    }
    return portList.count();
}
#elif defined(Q_OS_MAC) || defined(Q_OS_LINUX)
int
TSerialDevice::GetComPorts(QStringList& portList)
{
    QDir myDir("/dev");

    QStringList filters;

    filters.append("tty*");

    portList = myDir.entryList (filters, QDir::System);	// filter tty

    return portList.count();
}

#endif

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------

