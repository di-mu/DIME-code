//------------------------------------------------------------------------------
//
//	File:		RegistryKey.cpp
//
//	Abstract:	Windows Registry Helper Class Implementation
//
//	Version:	0.1
//
//	Date:		09.02.2015
//
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//------------------------------------------------------------------------------


#include "RegistryKey.h"

// only supported for windows platforms !
#ifdef Q_OS_WIN

TRegistryKey::TRegistryKey()
{
    HKey  = 0;
}

TRegistryKey::TRegistryKey(HKEY key)
{
    HKey  = key;
}

TRegistryKey::~TRegistryKey()
{
    // close registry
    if(HKey)
        RegCloseKey(HKey);
}

void
TRegistryKey::LocalMachine()
{
    HKey = HKEY_LOCAL_MACHINE;
}

TRegistryKey*
TRegistryKey::OpenSubKey(const QString& keyName)
{
    if(!HKey)
        return 0;

    wchar_t subKeyName[1024];

    int length = keyName.toWCharArray(subKeyName);
    subKeyName[length] = 0;

    HKEY hNewKey = 0;


    DWORD   retcode  = RegOpenKeyEx(HKey,subKeyName,0,KEY_READ,&hNewKey);
    if(retcode == ERROR_SUCCESS)
    {
        TRegistryKey* subKey = new TRegistryKey(hNewKey);

        return subKey;
    }
    return 0;
}

const QStringList&
TRegistryKey::GetSubKeyNames()
{
    TCHAR    achKey[MAX_KEY_LENGTH];    // buffer for subkey name
    DWORD    cbName;                    // size of name string
    TCHAR    achClass[MAX_PATH];
    DWORD    cchClassName = MAX_PATH;   // size of class string
    DWORD    cSubKeys=0;                // number of subkeys
    DWORD    cbMaxSubKey;               // longest subkey size
    DWORD    cchMaxClass;               // longest class string
    DWORD    cValues;                   // number of values for key
    DWORD    cchMaxValue;               // longest value name
    DWORD    cbMaxValueData;            // longest value data
    DWORD    cbSecurityDescriptor;      // size of security descriptor
    FILETIME ftLastWriteTime;           // last write time

    DWORD i, retCode;

    //TCHAR  achValue[MAX_VALUE_NAME];
    //DWORD  cchValue = MAX_VALUE_NAME;

    // Get the class name and the value count.
    retCode = RegQueryInfoKey(HKey,                 // key handle
                              achClass,                // buffer for class name
                              &cchClassName,           // size of class string
                              NULL,                    // reserved
                              &cSubKeys,               // number of subkeys
                              &cbMaxSubKey,            // longest subkey size
                              &cchMaxClass,            // longest class string
                              &cValues,                // number of values for this key
                              &cchMaxValue,            // longest value name
                              &cbMaxValueData,         // longest value data
                              &cbSecurityDescriptor,   // security descriptor
                              &ftLastWriteTime);       // last write time

    // Enumerate the subkeys, until RegEnumKeyEx fails.

    SubKeys.clear();

    if (cSubKeys)
    {

        for (i = 0; i < cSubKeys; i++)
        {
            cbName = MAX_KEY_LENGTH;
            retCode = RegEnumKeyEx(HKey, i,
                                   achKey,
                                   &cbName,
                                   NULL,
                                   NULL,
                                   NULL,
                                   &ftLastWriteTime);

            if (retCode == ERROR_SUCCESS)
            {
                QString keyString = QString::fromWCharArray(achKey);

                SubKeys.append(keyString);
            }
            else
               break;
        }
    }
    return SubKeys;
}

int
TRegistryKey::GetKeyValues(QStringList& list)
{
    //TCHAR    achKey[MAX_KEY_LENGTH];    // buffer for subkey name
    //DWORD    cbName;                    // size of name string
    TCHAR    achClass[MAX_PATH];
    DWORD    cchClassName = MAX_PATH;   // size of class string
    DWORD    cSubKeys=0;                // number of subkeys
    DWORD    cbMaxSubKey;               // longest subkey size
    DWORD    cchMaxClass;               // longest class string
    DWORD    cValues;                   // number of values for key
    DWORD    cchMaxValue;               // longest value name
    DWORD    cbMaxValueData;            // longest value data
    DWORD    cbSecurityDescriptor;      // size of security descriptor
    FILETIME ftLastWriteTime;           // last write time

    DWORD i, retCode;

    TCHAR  achValue[MAX_VALUE_NAME];
    DWORD  cchValue = MAX_VALUE_NAME;

    // Get the class name and the value count.
    retCode = RegQueryInfoKey(HKey,                 // key handle
                              achClass,                // buffer for class name
                              &cchClassName,           // size of class string
                              NULL,                    // reserved
                              &cSubKeys,               // number of subkeys
                              &cbMaxSubKey,            // longest subkey size
                              &cchMaxClass,            // longest class string
                              &cValues,                // number of values for this key
                              &cchMaxValue,            // longest value name
                              &cbMaxValueData,         // longest value data
                              &cbSecurityDescriptor,   // security descriptor
                              &ftLastWriteTime);       // last write time

    // Enumerate the subkeys, until RegEnumKeyEx fails.

    list.clear();

    if (cValues)
    {
        retCode = ERROR_SUCCESS;

        for (i = 0, retCode = ERROR_SUCCESS; i < cValues; i++)
        {
            cchValue    = MAX_VALUE_NAME;
            achValue[0] = 0;
            DWORD  valueType = 0;

            retCode = RegEnumValue(HKey, i,
                                   achValue,
                                   &cchValue,
                                   NULL, &valueType, NULL, NULL);

            if (retCode == ERROR_SUCCESS )
            {
                QString keyString = QString::fromWCharArray(achValue);

     //           MainWindow->showMessage(keyString);

                list.append(GetValue(keyString));
            }
        }
    }
    return list.count();
}

const QString&
TRegistryKey::GetValue(const QString keyName)
{
    Value = QString("");

    WCHAR   value_data[256];
    DWORD   value_size = 256;
    DWORD   value_type;

    wchar_t key[1024];

    int length = keyName.toWCharArray(key);
    key[length] = 0;

    DWORD retcode = RegQueryValueEx(HKey,                   // HKEY (hkey) handle of key to query
                                    key,                    // LPSTR (lpValueName) address of name of value to query
                                    NULL,                   // LPDWORD (lpReserved) reserved
                                    &value_type,            // LPDWORD (lpType) address of buffer for value type
                                    (BYTE *) &value_data,   // LPBYTE (lpData) address of data buffer
                                    &value_size);           // LPDWORD (lpcbData) address of data buffer size

    if (retcode == ERROR_SUCCESS)
    {

        Value = QString::fromWCharArray(value_data);
    }
    return Value;
}
#endif

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
