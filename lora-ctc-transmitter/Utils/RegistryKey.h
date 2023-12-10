//------------------------------------------------------------------------------
//
//	File:		RegistryKey.h
//
//	Abstract:	Windows Registry Helper Class Declaration
//
//	Version:	0.1
//
//	Date:		09.02.2015
//
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//------------------------------------------------------------------------------

#ifndef REGISTRYKEY_H
#define REGISTRYKEY_H

#include <Qt>

// only supported for windows platforms !
#ifdef  Q_OS_WIN

#include <qstringlist.h>
#include <windows.h>

#define MAX_KEY_LENGTH 256
#define MAX_VALUE_NAME 4096

//------------------------------------------------------------------------------
//
//  TRegistryKey - Class Declaration
//
//------------------------------------------------------------------------------

class TRegistryKey
{
public:
                        TRegistryKey();
                        TRegistryKey(HKEY key);

    virtual             ~TRegistryKey();

    void                LocalMachine();
    TRegistryKey*       OpenSubKey(const QString& keyName);
    const QStringList&  GetSubKeyNames();
    int                 GetKeyValues(QStringList& list);
    const QString&      GetValue(const QString keyName);

private:

private:
    HKEY                HKey;

    QStringList         SubKeys;
    QString             Value;
};

#endif // WINDOWS
#endif // REGISTRYKEY_H

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
