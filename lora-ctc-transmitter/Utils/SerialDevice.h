//------------------------------------------------------------------------------
//
//	File:		SerialDevice.h
//
//	Abstract:	Serial Device Wrapper Class Declaration
//
//	Version:	0.1
//
//	Date:		09.02.2015
//
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//------------------------------------------------------------------------------

#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H

#include <QString>
#include <QStringList>

#ifdef  Q_OS_WIN

#include <windows.h>

#define Baudrate_9600       9600
#define Baudrate_115200     115200
#define DataBits_7          7
#define DataBits_8          8
#define Parity_Even         EVENPARITY
#define Parity_None         NOPARITY

#elif defined( Q_OS_MAC )

#include <errno.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <IOKit/serial/ioss.h>

#define INVALID_HANDLE_VALUE    -1

#define Baudrate_9600       9600
#define Baudrate_115200     115200
#define DataBits_7          CS7
#define DataBits_8          CS8
#define Parity_Even         PARENB
#define Parity_None         0

#elif defined( Q_OS_LINUX )

#include <errno.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define INVALID_HANDLE_VALUE    -1

#define Baudrate_9600       9600
#define Baudrate_115200     115200
#define DataBits_7          CS7
#define DataBits_8          CS8
#define Parity_Even         PARENB
#define Parity_None         0

#endif

#include "WMDefs.h"

//------------------------------------------------------------------------------
//
// TSerialDevice Class Declaration
//
//------------------------------------------------------------------------------

class TSerialDevice
{
public:
                TSerialDevice();
                ~TSerialDevice();

    static int  GetComPorts(QStringList& portList);

    bool        Open(const QString& comPort, UINT32 baudRate, int bits = DataBits_8, UINT8 parity = Parity_None);
    bool        Close();

    bool        SendData(UINT8* data, int txLength);
    int         ReadData(UINT8* rxBuffer, int bufferSize);

private:

#ifdef  Q_OS_WIN

    HANDLE  ComHandle;

#elif defined( Q_OS_MAC )

    int     ComHandle;

#elif defined( Q_OS_LINUX )

    int     ComHandle;

#endif

};

#endif // SERIALDEVICE_H

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------

