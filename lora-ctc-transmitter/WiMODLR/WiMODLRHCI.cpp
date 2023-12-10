//------------------------------------------------------------------------------
//
//	File:		WiMODLRHCI.cpp
//
//	Abstract:	WiMODLR HCI Wrapper Class Implementation
//
//	Version:	0.2
//
//	Date:		07.06.2016
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

#include "WiMODLRHCI.h"
#include "CRC16.h"
#include <QTime>
#include <iostream>


//------------------------------------------------------------------------------
//
//  Defines
//
//------------------------------------------------------------------------------

#define SX1272_FOSC             52000000.0


//------------------------------------------------------------------------------
//
// Data/Radio Link Status Code Strings
//
//------------------------------------------------------------------------------

const TWiMODLRHCI_IDString   WiMODLRHCI_DataLinkStatusStrings[] =
{
    { DATALINK_STATUS_OK,                   "OK" },
    { DATALINK_STATUS_ERROR,                "error" } ,
    { DATALINK_STATUS_CMD_NOT_SUPPORTED,    "command no supported" },
    { DATALINK_STATUS_WRONG_PARAMETER,      "wrong parameter" },
    { DATALINK_STATUS_WRONG_DEVICE_MODE,    "wrong device mode" },
    { DATALINK_STATUS_MEDIA_BUSY,   		"media busy" },
    { DATALINK_STATUS_DEVICE_BUSY,  		"device busy" },
    { DATALINK_STATUS_QUEUE_FULL,           "queue full" },
    { 0, 0 }
};



//------------------------------------------------------------------------------
//
// Device Management Status Code Strings
//
//------------------------------------------------------------------------------

const TWiMODLRHCI_IDString WiMODLRHCI_DeviceMgmtStatusStrings[] =
{
    { DEVMGMT_STATUS_OK,                   "OK" },
    { DEVMGMT_STATUS_ERROR,                "error" } ,
    { DEVMGMT_STATUS_CMD_NOT_SUPPORTED,    "command no supported" },
    { DEVMGMT_STATUS_WRONG_PARAMETER,      "wrong parameter" },
    { DEVMGMT_STATUS_WRONG_DEVICE_MODE,    "wrong device mode" },
    { 0, 0 }
};


//------------------------------------------------------------------------------
//
// Radio Configuration Strings
//
//------------------------------------------------------------------------------

// RadioModes
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_RadioModes[] =
{
    { WiMODLR_RADIO_CONFIG_RM_STANDARD, "Standard" },
    { WiMODLR_RADIO_CONFIG_RM_ECHO,     "Echo" },
    { WiMODLR_RADIO_CONFIG_RM_SNIFFER,  "Sniffer" },
    { 0, 0 }
};

// Modulations
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_Modulations[] =
{
    { WiMODLR_RADIO_CONFIG_MOD_LORA, "LoRa" },
    { WiMODLR_RADIO_CONFIG_MOD_FSK, "Fsk" },
    { 0, 0 }
};

// Bandwidths
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_Bandwidths[] =
{
    { WiMODLR_RADIO_CONFIG_BW_200kHz, "200 kHz" },
    { WiMODLR_RADIO_CONFIG_BW_400kHz, "400 kHz" },
    { WiMODLR_RADIO_CONFIG_BW_800kHz, "800 kHz" },
    { WiMODLR_RADIO_CONFIG_BW_1600kHz, "1600 kHz" },
    { 0, 0 }
};



// Spreading Factors
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_SFs[] =
{
    { 0, "SF7" },
    { 1, "SF7" },
    { 2, "SF7" },
    { 3, "SF7" },
    { 4, "SF7" },
    { WiMODLR_RADIO_CONFIG_SF5, "SF5" },
    { WiMODLR_RADIO_CONFIG_SF6, "SF6" },
    { WiMODLR_RADIO_CONFIG_SF7, "SF7" },
    { WiMODLR_RADIO_CONFIG_SF8, "SF8" },
    { WiMODLR_RADIO_CONFIG_SF9, "SF9" },
    { WiMODLR_RADIO_CONFIG_SF10, "SF10" },
    { WiMODLR_RADIO_CONFIG_SF11, "SF11" },
    { WiMODLR_RADIO_CONFIG_SF12, "SF12" },
    { 0, 0 }
};

// Error Codings
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_ECs[] =
{
    { 0, "4/5" },
    { WiMODLR_RADIO_CONFIG_EC_4_5, "4/5" },
    { WiMODLR_RADIO_CONFIG_EC_4_6, "4/6" },
    { WiMODLR_RADIO_CONFIG_EC_4_7, "4/7" },
    { WiMODLR_RADIO_CONFIG_EC_4_8, "4/8" },
    { 0, 0 }
};

// Power Levels
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_PowerLevels[] =
{
    { 0, "5 dBm" },
    { 1, "5 dBm" },
    { 2, "5 dBm" },
    { 3, "5 dBm" },
    { 4, "5 dBm" },
    { 5, "5 dBm" },
    { 6, "6 dBm" },
    { 7, "7 dBm" },
    { 8, "8 dBm" },
    { 9, "9 dBm" },
    { 10, "10 dBm" },
    { 11, "11 dBm" },
    { 12, "12 dBm" },
    { 13, "13 dBm" },
    { 14, "14 dBm" },
    { 15, "15 dBm" },
    { 16, "16 dBm" },
    { 17, "17 dBm" },
    { 18, "18 dBm" },
    { 19, "19 dBm" },
    { 20, "20 dBm" },
    { 21, "21 dBm" },
    { 0, 0 }
};

// Tx Control
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_TxControl[] =
{
    { 0, "Tx Filter off" },
    { 1, "Tx Filter on" },
    { 0, 0 }
};

// Rx Control
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_RxControl[] =
{
    { 0, "Rx off" },
    { 1, "Rx aways on" },
    { 2, "Rx window on" },
    { 0, 0 }
};

// LED Control
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_LEDControl[] =
{
    { 0, "Rx(D3)" },
    { 1, "Tx(D2)" },
    { 2, "Alive(D4)" },
    { 3, "Button(D1)" },
    { 0, 0 }
};

// Radio Options
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_RadioOptions[] =
{
    { 0, "Ext. Output" },
    { 1, "RTC ON" },
    { 2, "HCI TxInd" },
    { 3, "HCI PowerUp-Ind" },
    { 4, "HCI Button-Ind" },
    { 0, 0 }
};

//------------------------------------------------------------------------------
//
//  TWiMODLRHCI - Class Constructor
//
//------------------------------------------------------------------------------

TWiMODLRHCI::TWiMODLRHCI()
{
    // register for rx-messages
    ComSlip.RegisterClient(this);

    // pass first RxBuffer and enable receiver/decoder
    ComSlip.SetRxBuffer(&Rx.Message.SapID, (UINT16)WIMODLR_HCI_RX_MESSAGE_SIZE);

    // init counter
    Rx.CRCError = 0;

    // 1000ms timeout for response
    Rx.Timeout = 1000;
}

//------------------------------------------------------------------------------
//
//  ~TWiMODLRHCI - Class Destructor
//
//------------------------------------------------------------------------------

TWiMODLRHCI::~TWiMODLRHCI()
{
    // close comport, if opened
    Close();
}

//------------------------------------------------------------------------------
//
//  Open
//
//  @brief  open comport for communication purpose
//
//------------------------------------------------------------------------------

bool
TWiMODLRHCI::Open(const QString& comPort)
{
    // open serial device
    if (SerialDevice.Open(comPort, Baudrate_115200))
    {
         // pass first RxBuffer and enable receiver/decoder
        ComSlip.SetRxBuffer(&Rx.Message.SapID, (UINT16)WIMODLR_HCI_RX_MESSAGE_SIZE);

        // ok
        return true;
    }
    // error
    return false;
}

//------------------------------------------------------------------------------
//
//  Close
//
//  @brief  close comport
//
//------------------------------------------------------------------------------

bool
TWiMODLRHCI::Close()
{
    // close serial device
    if (SerialDevice.Close())
    {
        // ok
        return true;
    }
    // error
    return false;
}

//------------------------------------------------------------------------------
//
//  Process
//
//  @brief: handle comport receiver path
//
//------------------------------------------------------------------------------

void
TWiMODLRHCI::Process()
{
    // read data from comport
    int numRxBytes = SerialDevice.ReadData(Rx.Buffer, sizeof(Rx.Buffer));

    // bytes received ?
    if (numRxBytes > 0)
    {
        // yes, pass to SLIP Decoder
        // Complete SLIP messages will be forwared via callback to
        // callback function "ProcessRxMessage" (see Receiver section)
        ComSlip.DecodeData(Rx.Buffer, numRxBytes);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  Device Management Services
//
//  @brief: this section includes Device Management services
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  PingRequest
//
//  @brief: send ping to check communication link
//
//------------------------------------------------------------------------------

TWiMODLRResult
TWiMODLRHCI::PingRequest()
{
    // send HCI message and wait for response
    return SendHCIMessage(DEVMGMT_SAP_ID, DEVMGMT_MSG_PING_REQ, DEVMGMT_MSG_PING_RSP);
}

//------------------------------------------------------------------------------
//
//  FactoryReset
//
//  @brief: restore factory settings
//
//------------------------------------------------------------------------------

TWiMODLRResult
TWiMODLRHCI::FactoryReset()
{
    // send HCI message and wait for response
    return SendHCIMessage2(DEVMGMT_SAP_ID, DEVMGMT_MSG_RESET_RADIO_CONFIG_REQ, DEVMGMT_MSG_RESET_RADIO_CONFIG_RSP);
}

//------------------------------------------------------------------------------
//
//  GetRadioConfiguration
//
//  @brief: get radio configuration
//
//------------------------------------------------------------------------------

TWiMODLRResult
TWiMODLRHCI::GetRadioConfiguration(TWiMODLR_RadioConfig& config, UINT8& status)
{
    // send message and wait for response
    TWiMODLRResult result = SendHCIMessage(DEVMGMT_SAP_ID, DEVMGMT_MSG_GET_RADIO_CONFIG_REQ, DEVMGMT_MSG_GET_RADIO_CONFIG_RSP);

    // message sent && response received ?
    if (result == WiMODLR_RESULT_OK)
    {
        // yes, return response status
        status = Rx.Message.Payload[0];

        // status ok ? -> config valid
        if(status == DEVMGMT_STATUS_OK)
        {
            // set pointer to start of radio configuration field
            UINT8* ptr = &Rx.Message.Payload[1];

            // deserialize data
            config.RadioMode        = *ptr++;
            std::cout << config.RadioMode << std::endl;//01
            config.GroupAddress     = *ptr++;
            std::cout << config.GroupAddress << std::endl;//02
            config.TxGroupAddress   = *ptr++;
            std::cout << config.TxGroupAddress << std::endl;//03
            config.DeviceAddress    = NTOH16(ptr); ptr += 2;
            std::cout << config.DeviceAddress << std::endl;//04
            config.TxDeviceAddress  = NTOH16(ptr); ptr += 2;
            std::cout << config.TxDeviceAddress << std::endl;//05
            config.Modulation       = *ptr++;
            std::cout << config.Modulation << std::endl;//06
            config.Frequency        = NTOH24(ptr); std::cout << config.Frequency << std::endl;ptr += 3;//0707

            config.Bandwidth        = *ptr++;
            std::cout << config.Bandwidth << std::endl;//0808
            config.SpreadingFactor  = *ptr++;
            std::cout << config.SpreadingFactor << std::endl;//0909
            config.ErrorCoding      = *ptr++;
            std::cout << config.ErrorCoding << std::endl;//1010
            config.PowerLevel       = *ptr++;
            std::cout << config.PowerLevel << std::endl;//1111
            config.TxControl        = *ptr++;
            std::cout << config.TxControl << std::endl;//12
            config.RxControl        = *ptr++;
            std::cout << config.RxControl << std::endl;//13
            config.RxWindowTime     = NTOH16(ptr); ptr += 2;
            std::cout << config.RxWindowTime << std::endl;//14
            config.LEDControl       = *ptr++;
            std::cout << config.LEDControl << std::endl;//15
            config.RadioOptions     = *ptr++;
            std::cout << config.RadioOptions << std::endl;//16
            // NEW
            config.FSKDataRate      = *ptr++;
            std::cout << config.FSKDataRate << std::endl;//17
            config.PowerSavingMode  = *ptr++;
            std::cout << config.PowerSavingMode << std::endl;//18
            config.LBTThreshold     = (INT16)NTOH16(ptr); ptr += 2;
            std::cout << config.LBTThreshold << std::endl;//19

            old_RadioMode = config.RadioMode;
            old_GroupAddress = config.GroupAddress;
            old_TxGroupAddress = config.TxGroupAddress;
            old_DeviceAddress = config.DeviceAddress;
            old_TxDeviceAddress = config.TxDeviceAddress;
            old_Modulation = config.Modulation;
            old_Frequency = config.Frequency;
            old_Bandwidth = config.Bandwidth;
            old_SpreadingFactor = config.SpreadingFactor;
            old_ErrorCoding = config.ErrorCoding;
            old_PowerLevel = config.PowerLevel;
            old_TxControl = config.TxControl;
            old_RxControl = config.RxControl;
            old_RxWindowTime = config.RxWindowTime;
            old_LEDControl = config.LEDControl;
            old_RadioOptions = config.RadioOptions;
            // NEW
            old_FSKDataRate = config.FSKDataRate ;
            old_PowerSavingMode = config.PowerSavingMode;
            old_LBTThreshold = config.LBTThreshold;

        }

        //ok, HCI response message received
        return WiMODLR_RESULT_OK;
    }

    // return result
    return result;
}

//------------------------------------------------------------------------------
//
//  SetRadioConfiguration
//
//  @brief: set radio configuration
//
//------------------------------------------------------------------------------

TWiMODLRResult
TWiMODLRHCI::SetRadioConfiguration(TWiMODLR_RadioConfig& config, UINT8 destMemory, UINT8& status)
{
    UINT8 payload[1+25];

    // set destination memory (RAM / NVM(EEPROM))
    payload[0] = destMemory;

    // set pointer to start of radio configuration field
    UINT8* ptr = &payload[1];

    // serialize data
    *ptr++ = config.RadioMode;
    *ptr++ = config.GroupAddress;
    *ptr++ = config.TxGroupAddress;
    HTON16(ptr, config.DeviceAddress); ptr += 2;
    HTON16(ptr, config.TxDeviceAddress); ptr += 2;
    *ptr++ = config.Modulation;
    HTON24(ptr, config.Frequency); ptr+= 3;
    *ptr++ = config.Bandwidth;
    *ptr++ = config.SpreadingFactor;
    *ptr++ = config.ErrorCoding;
    *ptr++ = config.PowerLevel;
    *ptr++ = config.TxControl;
    *ptr++ = config.RxControl;
    HTON16(ptr, config.RxWindowTime); ptr += 2;
    *ptr++ = config.LEDControl;
    *ptr++ = config.RadioOptions;
    // new
    *ptr++ = config.FSKDataRate;
    *ptr++ = config.PowerSavingMode;
    HTON16(ptr, config.LBTThreshold); ptr += 2;

    // send message and wait for response
    TWiMODLRResult result = SendHCIMessage(DEVMGMT_SAP_ID, DEVMGMT_MSG_SET_RADIO_CONFIG_REQ, DEVMGMT_MSG_SET_RADIO_CONFIG_RSP, payload, sizeof(payload));
    if(result == WiMODLR_RESULT_OK)
    {
        // return status
        status = Rx.Message.Payload[0];

        return WiMODLR_RESULT_OK;
    }
    return result;
}

TWiMODLRResult
TWiMODLRHCI::SetRadioConfiguration2(TWiMODLR_RadioConfig& config, UINT8 destMemory, UINT8& status)
{
    UINT8 payload[1+25];

    // set destination memory (RAM / NVM(EEPROM))
    payload[0] = destMemory;

    // set pointer to start of radio configuration field
    UINT8* ptr = &payload[1];

    // serialize data
    *ptr++ = config.RadioMode;
    *ptr++ = config.GroupAddress;
    *ptr++ = config.TxGroupAddress;
    HTON16(ptr, config.DeviceAddress); ptr += 2;
    HTON16(ptr, config.TxDeviceAddress); ptr += 2;
    *ptr++ = config.Modulation;
    HTON24(ptr, config.Frequency); ptr+= 3;
    *ptr++ = config.Bandwidth;
    *ptr++ = config.SpreadingFactor;
    *ptr++ = config.ErrorCoding;
    *ptr++ = config.PowerLevel;
    *ptr++ = config.TxControl;
    *ptr++ = config.RxControl;
    HTON16(ptr, config.RxWindowTime); ptr += 2;
    *ptr++ = config.LEDControl;
    *ptr++ = config.RadioOptions;
    // new
    *ptr++ = config.FSKDataRate;
    *ptr++ = config.PowerSavingMode;
    HTON16(ptr, config.LBTThreshold); ptr += 2;

    // send message and wait for response
    SendHCIMessage2(DEVMGMT_SAP_ID, DEVMGMT_MSG_SET_RADIO_CONFIG_REQ, DEVMGMT_MSG_SET_RADIO_CONFIG_RSP, payload, sizeof(payload));


    return WiMODLR_RESULT_OK;

}

//------------------------------------------------------------------------------
//
//  ConvertRadioConfiguration
//
//  @brief: convert radio configuration into human readble key-value-list
//
//------------------------------------------------------------------------------

void
TWiMODLRHCI::ConvertRadioConfiguration(TKeyValueList& list, const TWiMODLR_RadioConfig& config)
{
    // Radio Mode
    list.AddKeyValue("RadioMode", GetStringFromTable(WiMODLRHCI_RadioConfig_RadioModes, config.RadioMode));

    // Addresses on RF Link
    list.AddHexKeyValue("GroupAddress",     config.GroupAddress);
    list.AddHexKeyValue("TxGroupAddress",   config.TxGroupAddress);
    list.AddHexKeyValue("DeviceAddress",    config.DeviceAddress);
    list.AddHexKeyValue("TxDeviceAddress",  config.TxDeviceAddress);

    // Modulation
    list.AddKeyValue("Modulation", GetStringFromTable(WiMODLRHCI_RadioConfig_Modulations, config.Modulation));

    // Carrier Frequency
    UINT32 freq = GetFrequencyFromConfig(config.Frequency);
    list.AddKeyValue("Frequency", QString("%1 Hz").arg(freq));

    // Bandwidth
    list.AddKeyValue("Bandwidth", GetStringFromTable(WiMODLRHCI_RadioConfig_Bandwidths, config.Bandwidth));

    // SF
    list.AddKeyValue("SpreadingFactor", GetStringFromTable(WiMODLRHCI_RadioConfig_SFs, config.SpreadingFactor));

    // Error Coding
    list.AddKeyValue("ErrorcCoding", GetStringFromTable(WiMODLRHCI_RadioConfig_ECs, config.ErrorCoding));

    // Power Level
    list.AddKeyValue("PowerLevel", GetStringFromTable(WiMODLRHCI_RadioConfig_PowerLevels, config.PowerLevel));

    // Tx Control
    list.AddKeyValue("TxControl", GetStringFromTable(WiMODLRHCI_RadioConfig_TxControl, config.TxControl));

    // Rx Control
    list.AddKeyValue("RxControl", GetStringFromTable(WiMODLRHCI_RadioConfig_RxControl, config.RxControl));

    // Rx Window
    list.AddKeyValue("RxWindowTime", QString("%1 ms").arg((UINT16)config.RxWindowTime));

    // LED Control
    list.AddKeyValue("LEDControl", GetCombinedStringFromTable(WiMODLRHCI_RadioConfig_LEDControl, config.LEDControl, 4));

    // Radio Options
    list.AddKeyValue("RadioOptions", GetCombinedStringFromTable(WiMODLRHCI_RadioConfig_RadioOptions, config.RadioOptions, 5));

    // NEW
    // FSK Datarate
    list.AddKeyValue("FSKDataRate", config.FSKDataRate);

    // Power Saving Mode
    list.AddKeyValue("PowerSavingMode", config.PowerSavingMode);

    // LBT Threshold
    list.AddKeyValue("LBTThreshold", (INT16)config.LBTThreshold);
}

//------------------------------------------------------------------------------
//
//  GetDeviceMgmtStatusString
//
//  @brief  convert device manamgement status code to string
//
//------------------------------------------------------------------------------

const char*
TWiMODLRHCI::GetDeviceMgmtStatusString(UINT8 status)
{
    return GetStringFromTable(WiMODLRHCI_DeviceMgmtStatusStrings, status);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  Radio Link Services
//
//  @brief: this section includes Radio Link services
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  SendURadioMessage
//
//  @brief: send unrelaible radio message
//
//------------------------------------------------------------------------------

TWiMODLRResult
TWiMODLRHCI::SendURadioMessage(UINT8* txMessage, UINT16 length, UINT8& status)
{
    // send message and wait for response
    TWiMODLRResult result = SendHCIMessage(DATALINK_SAP_ID, DATALINK_MSG_SEND_URADIO_MSG_REQ, DATALINK_MSG_SEND_URADIO_MSG_RSP, txMessage, length);

    // message sent && response received ?
    if (result == WiMODLR_RESULT_OK)
    {
        // yes, return response status
        status = Rx.Message.Payload[0];

        //ok, HCI response message received
        return WiMODLR_RESULT_OK;
    }

    // return result
    return result;
}


//my own send without response
TWiMODLRResult
TWiMODLRHCI::SendURadioMessage2(UINT8* txMessage, UINT16 length, UINT8& status)
{
    // send message and wait for response
    SendHCIMessage2(DATALINK_SAP_ID, DATALINK_MSG_SEND_URADIO_MSG_REQ, DATALINK_MSG_SEND_URADIO_MSG_RSP, txMessage, length);
    status = 0;
    // message sent && response received ?
    //if (result == WiMODLR_RESULT_OK)
    //{
        // yes, return response status
        //status = Rx.Message.Payload[0];

        //ok, HCI response message received
        return WiMODLR_RESULT_OK;
    //}

    // return result
    //return result;
}

//------------------------------------------------------------------------------
//
//  ConvertRadioMessage
//
//  @brief: convert radio message into human readable key-value-list
//
//------------------------------------------------------------------------------

void
TWiMODLRHCI::ConvertRadioRxMessage(TKeyValueList& list, const TWiMODLR_HCIMessage& rxMsg)
{
    // get radio packet format
    UINT8 format = rxMsg.Payload[0];
    list.AddHexKeyValue("Format", format);

    // get destination group address
    list.AddHexKeyValue("DstGroupAddress:",rxMsg.Payload[1]);

    // get destination device address
    UINT16 val = NTOH16(&rxMsg.Payload[2]);
    list.AddHexKeyValue("DstDeviceAddress:",val);

    // get source group address
    list.AddHexKeyValue("SrcGroupAddress:",rxMsg.Payload[4]);

    // get source device address
    val = NTOH16(&rxMsg.Payload[5]);
    list.AddHexKeyValue("SrcDeviceAddress:",val);

    // get radio message payload (msg length - x bytes header)
    int numPayloadBytes = rxMsg.Length - DATALINK_MSG_HEADER_SIZE;

    // extended HCI output fomat attched ?
    if (format & 0x01)
    {
        // correct  by additional footer bytes
        numPayloadBytes -= DATALINK_MSG_FOOTER_SIZE;
    }

    // add payload
    if(numPayloadBytes > 0)
    {
        list.AddHexKeyValue("Payload",&rxMsg.Payload[DATALINK_MSG_HEADER_SIZE], numPayloadBytes);
    }

    // extended HCI output ?
    if(format & 0x01)
    {
        int extOffset = DATALINK_MSG_HEADER_SIZE + numPayloadBytes;

        INT16  rssi = NTOH16(&rxMsg.Payload[extOffset]);
        INT8   snr  = rxMsg.Payload[extOffset + 2];
        UINT32 time = NTOH32(&rxMsg.Payload[extOffset + 3]);

        QString timeString;
        U32TimeToString(timeString, time, true);

        list.AddKeyValue("RSSI", QString("%1 dBm").arg((float)rssi,0,'f',2));
        list.AddKeyValue("SNR", QString("SNR:%1 dB").arg((float)snr,0,'f',2));
        list.AddKeyValue("RxTime", timeString);
    }
}

//------------------------------------------------------------------------------
//
//  GetRadioLinkStatusString
//
//  @brief  convert radiolink status code to string
//
//------------------------------------------------------------------------------

const char*
TWiMODLRHCI::GetRadioLinkStatusString(UINT8 status)
{
    return GetStringFromTable(WiMODLRHCI_DataLinkStatusStrings, status);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  HCI Message functions
//
//  @brief: this section includes functions for HCI message handling
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  SendHCIMessage
//
//  @brief: this function send a HCI message to a radio module and waits
//          for a response
//
//------------------------------------------------------------------------------

TWiMODLRResult
TWiMODLRHCI::SendHCIMessage(UINT8 dstSapID, UINT8 msgID, UINT8 rxMsgID, UINT8* payload, UINT16 length)
{
    // send message
    TWiMODLRResult result = PostMessage(dstSapID, msgID, payload, length);

    // message sent ?
    if (result == WiMODLR_RESULT_OK)
    {
        // yes, wait for response from radio
        if (WaitForResponse(dstSapID, rxMsgID))
        {
            return WiMODLR_RESULT_OK;
        }
        return WiMODLR_RESULT_NO_RESPONSE;
    }
    // return error
    return result;
}

//Send without response
TWiMODLRResult
TWiMODLRHCI::SendHCIMessage2(UINT8 dstSapID, UINT8 msgID, UINT8 rxMsgID, UINT8* payload, UINT16 length)
{
    // send message
    PostMessage2(dstSapID, msgID, payload, length);
    //rxMsgID = 0;


    return WiMODLR_RESULT_OK;

}

//------------------------------------------------------------------------------
//
//  PostMessage
//
//  @brief  send a HCI message (with or without payload)
//
//------------------------------------------------------------------------------

TWiMODLRResult
TWiMODLRHCI::PostMessage(UINT8 sapID, UINT8 msgID, UINT8* payload, UINT16 length)
{
    // 1. check parameter
    //
    // 1.1 check length
    //
    if(length > WIMODLR_HCI_MSG_PAYLOAD_SIZE)
    {
        return WiMODLR_RESULT_PAYLOAD_LENGTH_ERROR;
    }
    // 1.2 check payload ptr
    //
    if(length && !payload)
    {
        return WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    // 2.  init TxMessage
    //
    // 2.1 init SAP ID
    //
    TxMessage.SapID = sapID;

    // 2.2 init Msg ID
    //
    TxMessage.MsgID = msgID;

    // 2.3 copy payload, if present
    //
    if(payload && length)
    {
        UINT8*  dstPtr  = TxMessage.Payload;
        int     n       = (int)length;

        // copy bytes
        while(n--)
            *dstPtr++ = *payload++;
    }

    // 3. Calculate CRC16 over header and optional payload
    //
    UINT16 crc16 = CRC16_Calc(&TxMessage.SapID, length + WIMODLR_HCI_MSG_HEADER_SIZE, CRC16_INIT_VALUE);

    // 3.1 get 1's complement
    //
    crc16 = ~crc16;

    // 3.2 attach CRC16 and correct length, lobyte first
    //
    TxMessage.Payload[length++] = LOBYTE(crc16);
    TxMessage.Payload[length++] = HIBYTE(crc16);

    // 4. forward message to SLIP layer
    //    - start transmission with SAP ID
    //    - correct length by header size

    return SendPacket(&TxMessage.SapID, length + WIMODLR_HCI_MSG_HEADER_SIZE);
}

TWiMODLRResult
TWiMODLRHCI::PostMessage2(UINT8 sapID, UINT8 msgID, UINT8* payload, UINT16 length)
{
    // 1. check parameter
    //
    // 1.1 check length
    //


    // 1.2 check payload ptr
    //
    //printf("222\n");
    if(length && !payload)
    {
        //printf("111\n");
        return WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    // 2.  init TxMessage
    //
    // 2.1 init SAP ID
    //
    TxMessage.SapID = sapID;

    // 2.2 init Msg ID
    //
    TxMessage.MsgID = msgID;

    // 2.3 copy payload, if present
    //
    if(payload && length)
    {
        UINT8*  dstPtr  = TxMessage.Payload;
        int     n       = (int)length;

        // copy bytes
        while(n--)
            *dstPtr++ = *payload++;
    }

    // 3. Calculate CRC16 over header and optional payload
    //
    UINT16 crc16 = CRC16_Calc(&TxMessage.SapID, length + WIMODLR_HCI_MSG_HEADER_SIZE, CRC16_INIT_VALUE);

    // 3.1 get 1's complement
    //
    crc16 = ~crc16;

    // 3.2 attach CRC16 and correct length, lobyte first
    //
    TxMessage.Payload[length++] = LOBYTE(crc16);
    TxMessage.Payload[length++] = HIBYTE(crc16);

    // 4. forward message to SLIP layer
    //    - start transmission with SAP ID
    //    - correct length by header size

    return SendPacket(&TxMessage.SapID, length + WIMODLR_HCI_MSG_HEADER_SIZE);
}


//------------------------------------------------------------------------------
//
//  SendPacket
//
//  @brief: send packet as SLIP stream via serial device
//
//------------------------------------------------------------------------------

TWiMODLRResult
TWiMODLRHCI::SendPacket(UINT8* txData, UINT16 length)
{
    // call SLIP encoder
    int txLength = ComSlip.EncodeData(TxBuffer, sizeof(TxBuffer), txData, length);

    // stream ok ?
    if (txLength > 0)
    {
        // send SLIP stream via serial device
        if (SerialDevice.SendData(TxBuffer, txLength))
            return WiMODLR_RESULT_OK;
        else
            return WiMODLR_RESULT_TRANMIT_ERROR;
    }
    return WiMODLR_RESULT_SLIP_ENCODER_ERROR;
}

static bool flag = false;
static int record_length;

TWiMODLRResult
TWiMODLRHCI::SendPacket2(UINT8* txData, UINT16 length)
{
    if(flag == false){
        // call SLIP encoder
        flag = true;
        int txLength = ComSlip.EncodeData(TxBuffer, sizeof(TxBuffer), txData, length);
        record_length = txLength;

        // stream ok ?
        if (txLength > 0)
        {
            // send SLIP stream via serial device
            if (SerialDevice.SendData(TxBuffer, txLength))
                return WiMODLR_RESULT_OK;
            else
                return WiMODLR_RESULT_TRANMIT_ERROR;
        }
        return WiMODLR_RESULT_SLIP_ENCODER_ERROR;
    }
    else {
        SerialDevice.SendData(TxBuffer, record_length);
        return WiMODLR_RESULT_OK;
    }

}


//------------------------------------------------------------------------------
//
//  WaitForResponse
//
//  @brief: wait for response HCI message
//
//------------------------------------------------------------------------------
bool
TWiMODLRHCI::WaitForResponse(UINT8 rxSapID, UINT8 rxMsgID)
{
    // init receiver struct
    Rx.Active = true;
    Rx.Done   = false;
    Rx.SapID  = rxSapID;
    Rx.MsgID  = rxMsgID;

    QTime time;
    time.start();

    // wwit for response ~1000ms
    while(time.elapsed() <= Rx.Timeout)
    {
        // call receiver path
        Process();

        // response received  ?
        if(Rx.Done)
        {
            // clear flag
            Rx.Active = false;

            // ok
            return true;
        }
    }
    // clear flag
    Rx.Active = false;

    // error - timeout
    return false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  Section Receiver Functions
//
//  @brief: this section includes functions that handle received HCI messages
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  ProcessRxMessage
//
//  @brief: handle received SLIP message
//
//------------------------------------------------------------------------------

UINT8*
TWiMODLRHCI::ProcessRxMessage(UINT8* rxBuffer, UINT16 length)
{
    // 1. check CRC
    if (CRC16_Check(rxBuffer, length, CRC16_INIT_VALUE))
    {
        // 2. check min length, 2 bytes for SapID + MsgID + 2 bytes CRC16
        if(length >= (WIMODLR_HCI_MSG_HEADER_SIZE + WIMODLR_HCI_MSG_FCS_SIZE))
        {
            // 3. Hack: since only one RxMessage buffer is used,
            //          rxBuffer must point to RxMessage.SapId, thus
            //          memcpy to RxMessage structure is not needed here

            // add length
            Rx.Message.Length = length - (WIMODLR_HCI_MSG_HEADER_SIZE + WIMODLR_HCI_MSG_FCS_SIZE);

            // dispatch completed RxMessage
            DispatchRxMessage(Rx.Message);
        }
    }
    else
    {
        // handle CRC error
        Rx.CRCError++;

        ShowMessage("CRC Error");
    }

    // return same buffer again, keep receiver enabled
    return &Rx.Message.SapID;
}

//------------------------------------------------------------------------------
//
//  DispatchRxMessage
//
//  @brief: dispatch CRC checked HCI message
//
//------------------------------------------------------------------------------

void
TWiMODLRHCI::DispatchRxMessage(TWiMODLR_HCIMessage& rxMsg)
{
    // 1. test if a reposne message is expected
    if(Rx.Active)
    {
        // expected response received ?
        if ((rxMsg.SapID == Rx.SapID) && (rxMsg.MsgID == Rx.MsgID))
        {
            // yes
            Rx.Done = true;

            // no further processing here !
            return;
        }
    }

    // 2. forward async received messages to corresponding SAP
    switch(rxMsg.SapID)
    {
        case    DEVMGMT_SAP_ID:
                DispatchDeviceMgmtMessage(rxMsg);
                break;

        case    DATALINK_SAP_ID:
                DispatchRadioLinkMessage(rxMsg);
                break;

        default:
                // handle unsupported SapIDs here
                ShowMessage("warning - unsupported messages received", rxMsg);
                break;
    }
}

//------------------------------------------------------------------------------
//
//  DisaptchDeviceMgmtMessage
//
//  @brief: dispatch Device Management Access Point messages here
//
//------------------------------------------------------------------------------

void
TWiMODLRHCI::DispatchDeviceMgmtMessage(TWiMODLR_HCIMessage& rxMsg)
{
    switch(rxMsg.MsgID)
    {
        case    DEVMGMT_MSG_PING_RSP:
                // handle ping response here
                break;

        default:
                // handle unsupported MsgIDs here
                ShowMessage("warning - unsupported DeviceMgmt message received", rxMsg);
                break;
    }
}

//------------------------------------------------------------------------------
//
//  DispatchRadioLinkMessage
//
//  @brief: dispatch Radio Link Access Point messages here
//
//------------------------------------------------------------------------------

void
TWiMODLRHCI::DispatchRadioLinkMessage(TWiMODLR_HCIMessage& rxMsg)
{
    switch(rxMsg.MsgID)
    {
        case    DATALINK_MSG_RECV_URADIO_MSG_IND:
                ShowMessage("Unreliable RadioLink message received", rxMsg);
                if (Client)
                {
                    // notify client
                    Client->evRadioLink_RxUMessage(rxMsg);
                }
                break;

        default:
                // handle unsupported MsgIDs here
                ShowMessage("warning - unsupported RadioLink message received", rxMsg);
                break;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  Debug Functions
//
//  @brief: this section includes some debug functions
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  ShowMessage
//
//  @brief: show debug message
//
//------------------------------------------------------------------------------

void
TWiMODLRHCI::ShowMessage(const QString& string, const TWiMODLR_HCIMessage& msg)
{
    if (!Client)
        return;

    Client->evRadio_ShowMessage("WiMODLRHCI", string);

    QString sapID = QString("%1").arg(msg.SapID, 2, 16, QLatin1Char('0')).toUpper();
    QString msgID = QString("%1").arg(msg.MsgID, 2, 16, QLatin1Char('0')).toUpper();
    QString str   = QString("RxMsg:SapID:") + sapID + QString(";MsgID:") + msgID + QString(";Length:%3;Payload:").arg(msg.Length);

    int i = 0;
    int length = msg.Length;
    while(length--)
        str.append(QString("%1 ").arg(msg.Payload[i++], 2, 16, QLatin1Char('0')).toUpper());

    Client->evRadio_ShowMessage("WiMODLRHCI", str);
}

//------------------------------------------------------------------------------
//
//  ShowMessage
//
//  @brief: show debug message
//
//------------------------------------------------------------------------------

void
TWiMODLRHCI::ShowMessage(const QString& string)
{
    if (Client)
        Client->evRadio_ShowMessage("WiMODLRHCI", string);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  Helper Functions
//
//  @brief: this section includes some helper functions
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  U32TimeToString
//
//  @brief: convert target time to time string
//
//------------------------------------------------------------------------------

#define RTC_GET_SECONDS(t)	((t)       & 0x3F)
#define RTC_GET_MINUTES(t)	((t >>  6) & 0x3F)
#define RTC_GET_MONTHS(t)	((t >> 12) & 0x0F)

#define RTC_GET_HOURS(t)	((t >> 16) & 0x1F)
#define RTC_GET_DAYS(t)		((t >> 21) & 0x1F)
#define RTC_GET_YEARS(t)   (((t >> 26) & 0x3F) + 2000)

void
TWiMODLRHCI::U32TimeToString(QString& timeString, UINT32 time, bool isoFormat)
{
    UINT8   seconds =   RTC_GET_SECONDS(time);
    UINT8   minutes =   RTC_GET_MINUTES(time);
    UINT8   months  =   RTC_GET_MONTHS(time);
    UINT8   hours   =   RTC_GET_HOURS(time);
    UINT8   days    =   RTC_GET_DAYS(time);
    UINT16  years   =   RTC_GET_YEARS(time);

    if (isoFormat)
        timeString = QString("%1-%2-%3 %4:%5:%6").arg(years)
                                                 .arg(months,   2, 10, QLatin1Char('0'))
                                                 .arg(days,     2, 10, QLatin1Char('0'))
                                                 .arg(hours,    2, 10, QLatin1Char('0'))
                                                 .arg(minutes,  2, 10, QLatin1Char('0'))
                                                 .arg(seconds,  2, 10, QLatin1Char('0'));
    else
        timeString = QString("%1.%2.%3 %4:%5:%6").arg(days,     2, 10, QLatin1Char('0'))
                                                 .arg(months,   2, 10, QLatin1Char('0'))
                                                 .arg(years)
                                                 .arg(hours,    2, 10, QLatin1Char('0'))
                                                 .arg(minutes,  2, 10, QLatin1Char('0'))
                                                 .arg(seconds,  2, 10, QLatin1Char('0'));
}

//------------------------------------------------------------------------------
//
//  GetFrequencyFromConfig
//
//  @brief: convert frequency register configuration to frequency value in [Hz]
//
//------------------------------------------------------------------------------

UINT32
TWiMODLRHCI::GetFrequencyFromConfig(UINT32 regConfig)
{
    //printf("%x\n",regConfig);
    double fstep    = SX1272_FOSC / double(1 << 18);

    double freq     = (regConfig * fstep);

    return (UINT32)freq;
}

//------------------------------------------------------------------------------
//
//  GetStringFromTable
//
//  @brief: convert ID into human readable string
//
//------------------------------------------------------------------------------

const char*
TWiMODLRHCI::GetStringFromTable(const TWiMODLRHCI_IDString* table, UINT8 id)
{
    while(table->IDString)
    {
        if (table->ID == id)
            return table->IDString;

        table++;
    }
    return "Unsupported ID";
}

//------------------------------------------------------------------------------
//
//  GetCombinedStringFromTable
//
//  @brief: convert bit settings into human readable string
//
//------------------------------------------------------------------------------

QString
TWiMODLRHCI::GetCombinedStringFromTable(const TWiMODLRHCI_IDString* table, UINT8 id, int numBits)
{
    QString string;

    UINT8 mask = 0x01;
    for(int i = 0; i < numBits; i++)
    {
        if (id & mask)
        {
            string.append(GetStringFromTable(table, i));
            string.append("/");
        }
        mask <<= 1;
    }

    string.chop(1);
    if (string.isEmpty())
        string = QString("off");

    return string;
}

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
