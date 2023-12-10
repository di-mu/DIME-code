//------------------------------------------------------------------------------
//
//	File:		KeyValueList.h
//
//	Abstract:	Key-Value-List Helper Class Declaration
//
//	Version:	0.1
//
//	Date:		09.02.2015
//
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//------------------------------------------------------------------------------

#ifndef KEYVALUELIST_H
#define KEYVALUELIST_H

//------------------------------------------------------------------------------
//
//  Include Files
//
//------------------------------------------------------------------------------

#include <QStringList>
#include "WMDefs.h"

//------------------------------------------------------------------------------
//
//  TKeyValueList   Class Declaration
//
//------------------------------------------------------------------------------

class TKeyValueList : public QStringList
{
public:
    TKeyValueList();

    void         AddHexKeyValue(const QString&      key,
                                UINT8               value);

    void         AddHexKeyValue(const QString&      key,
                                UINT16              value);

    void         AddHexKeyValue(const QString&      key,
                                const UINT8*        payload,
                                int                 length);

    void         AddKeyValue(const QString&      key,
                             UINT32              value);

    void         AddKeyValue(const QString&      key,
                             int                 value);

    void         AddKeyValue(const QString&      key,
                             const QString&      value);

};

#endif // KEYVALUELIST_H

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
