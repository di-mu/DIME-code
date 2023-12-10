//------------------------------------------------------------------------------
//
//	File:		KeyValueList.cpp
//
//	Abstract:	Key-Value-List Helper Class Implementation
//
//	Version:	0.1
//
//	Date:		09.02.2015
//
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  Include Files
//
//------------------------------------------------------------------------------

#include "KeyValueList.h"

//------------------------------------------------------------------------------
//
//  Class Constructor
//
//------------------------------------------------------------------------------

TKeyValueList::TKeyValueList()
{
}

//------------------------------------------------------------------------------
//
//  AddHeyKeyValue
//
//  @brief: add key value pair
//
//------------------------------------------------------------------------------

void
TKeyValueList::AddHexKeyValue(
                    const QString&      key,
                    UINT8               value)
{
    QString valueString = QString("%1").arg(value, 2, 16, QLatin1Char('0')).toUpper();

    append(key + ":" + valueString);
}

void
TKeyValueList::AddHexKeyValue(
                    const QString&      key,
                    UINT16              value)
{
    QString valueString = QString("%1").arg(value, 4, 16, QLatin1Char('0')).toUpper();

    append(key + ":" + valueString);
}

void
TKeyValueList::AddKeyValue(
                    const QString&      key,
                    int                 value)
{
    QString valueString = QString("%1").arg(value);

    append(key + ":" + valueString);
}

void
TKeyValueList::AddKeyValue(
                    const QString&      key,
                    const QString&      value)
{
    append(key + ":" + value);
}


void
TKeyValueList::AddHexKeyValue(
                    const QString&      key,
                    const UINT8*        payload,
                    int                 length)
{
    QString payloadStr;

    for(int i = 0; i < length; i++)
        payloadStr.append(QString("%1 ").arg(payload[i], 2, 16, QLatin1Char('0')).toUpper());
    payloadStr.chop(1);

    append(key + ":" + payloadStr);
}

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
