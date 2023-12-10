//------------------------------------------------------------------------------
//
//	File:		WiMODLRHCI.h
//
//	Abstract:	WiMODLR HCI Wrapper Class Declaration
//
//	Version:	0.2
//
//	Date:		07.06.2016
//
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//------------------------------------------------------------------------------

#ifndef WIMODLRHCI_H
#define WIMODLRHCI_H

//------------------------------------------------------------------------------
//
// Include Files
//
//------------------------------------------------------------------------------

#include "WMDefs.h"
#include "WiMODLRHCI_IDs.h"
#include "ComSlip.h"
#include "SerialDevice.h"
#include "KeyValueList.h"
//------------------------------------------------------------------------------
//
// General Declaration
//
//------------------------------------------------------------------------------

//#define HIBYTE(w)       (UINT8)((w) >> 8)
//#define LOBYTE(w)       (UINT8)(w)

typedef uint16_t        TWiMODLRResult;

//------------------------------------------------------------------------------
//
// Radio Configuration Defines
//
//------------------------------------------------------------------------------

#define WiMODLR_STORE_INTO_RAM              0
#define WiMODLR_STORE_INTO_NVM              1

#define WiMODLR_RADIO_CONFIG_RM_STANDARD    0
#define WiMODLR_RADIO_CONFIG_RM_ECHO        1
#define WiMODLR_RADIO_CONFIG_RM_SNIFFER     2

#define WiMODLR_RADIO_CONFIG_MOD_LORA       0
#define WiMODLR_RADIO_CONFIG_MOD_FSK        1

#define WiMODLR_RADIO_CONFIG_BW_200kHz      2
#define WiMODLR_RADIO_CONFIG_BW_400kHz      3
#define WiMODLR_RADIO_CONFIG_BW_800kHz      4
#define WiMODLR_RADIO_CONFIG_BW_1600kHz     5

#define WiMODLR_RADIO_CONFIG_SF3            3
#define WiMODLR_RADIO_CONFIG_SF4            4
#define WiMODLR_RADIO_CONFIG_SF5            5
#define WiMODLR_RADIO_CONFIG_SF6            6
#define WiMODLR_RADIO_CONFIG_SF7            7
#define WiMODLR_RADIO_CONFIG_SF8            8
#define WiMODLR_RADIO_CONFIG_SF9            9
#define WiMODLR_RADIO_CONFIG_SF10           10
#define WiMODLR_RADIO_CONFIG_SF11           11
#define WiMODLR_RADIO_CONFIG_SF12           12

#define WiMODLR_RADIO_CONFIG_EC_4_5         1
#define WiMODLR_RADIO_CONFIG_EC_4_6         2
#define WiMODLR_RADIO_CONFIG_EC_4_7         3
#define WiMODLR_RADIO_CONFIG_EC_4_8         4

//------------------------------------------------------------------------------
//
// HCI Message Declaration
//
//------------------------------------------------------------------------------

// message header size: 2 bytes for SapID + MsgID
#define WIMODLR_HCI_MSG_HEADER_SIZE     2

// message payload size
#define WIMODLR_HCI_MSG_PAYLOAD_SIZE    280

// frame check sequence field size: 2 bytes for CRC16
#define WIMODLR_HCI_MSG_FCS_SIZE        2

// visible max. buffer size for lower SLIP layer
#define WIMODLR_HCI_RX_MESSAGE_SIZE     (WIMODLR_HCI_MSG_HEADER_SIZE\
                                         + WIMODLR_HCI_MSG_PAYLOAD_SIZE\
                                         + WIMODLR_HCI_MSG_FCS_SIZE)

//------------------------------------------------------------------------------
//
// HCI Message
//
//------------------------------------------------------------------------------

typedef struct
{
    // Payload Length Information, not transmitted over UART interface !
    UINT16  Length;

    // Service Access Point Identifier
    UINT8   SapID;

    // Message Identifier
    UINT8   MsgID;

    // Payload Field
    UINT8   Payload[WIMODLR_HCI_MSG_PAYLOAD_SIZE];

    // Frame Check Sequence Field
    UINT8   CRC16[WIMODLR_HCI_MSG_FCS_SIZE];

}TWiMODLR_HCIMessage;

//------------------------------------------------------------------------------
//
// Definition of Result/Error Codes
//
//------------------------------------------------------------------------------

typedef enum
{
    WiMODLR_RESULT_OK = 0,
    WiMODLR_RESULT_PAYLOAD_LENGTH_ERROR,
    WiMODLR_RESULT_PAYLOAD_PTR_ERROR,
    WiMODLR_RESULT_TRANMIT_ERROR,
    WiMODLR_RESULT_SLIP_ENCODER_ERROR,
    WiMODLR_RESULT_NO_RESPONSE
}TWiMDLRResultCodes;

//------------------------------------------------------------------------------
//
// Radio Configuration
//
// see WiMODLR HCI Specification Chapter 3.1.5.3
//
//------------------------------------------------------------------------------

typedef struct
{
    UINT8   RadioMode;
    UINT8   GroupAddress;
    UINT8   TxGroupAddress;
    UINT16  DeviceAddress;
    UINT16  TxDeviceAddress;
    UINT8   Modulation;
    UINT32  Frequency;
    UINT8   Bandwidth;
    UINT8   SpreadingFactor;
    UINT8   ErrorCoding;
    UINT8   PowerLevel;
    UINT8   TxControl;
    UINT8   RxControl;
    UINT16  RxWindowTime;
    UINT8   LEDControl;
    UINT8   RadioOptions;
    // NEW
    UINT8   FSKDataRate;
    UINT8   PowerSavingMode;
    INT16   LBTThreshold;
}TWiMODLR_RadioConfig;

//------------------------------------------------------------------------------
//
// ID String Table Item
//
//------------------------------------------------------------------------------

typedef struct
{
    // ID Code
    UINT8       ID;

    // ID String
    const char* IDString;

}TWiMODLRHCI_IDString;

//------------------------------------------------------------------------------
//
// TWiMODLRHCIClient Class Declaration
//
//------------------------------------------------------------------------------

class TWiMODLRHCIClient
{
    public:
                        TWiMODLRHCIClient() {}
    virtual             ~TWiMODLRHCIClient() {}

    // define debug callback function
    virtual void        evRadio_ShowMessage(const QString& /* prefix */, const QString& /* msg */) {}

    // define handler for received unreliable messages
    virtual void        evRadioLink_RxUMessage(const TWiMODLR_HCIMessage& /* rxMsg */) {}
};

//------------------------------------------------------------------------------
//
// TWiMODLRHCI Class Declaration
//
//------------------------------------------------------------------------------

class TWiMODLRHCI : public TComSlipClient
{
    public:
                        TWiMODLRHCI();
                        ~TWiMODLRHCI();

                        UINT8   old_RadioMode;
                        UINT8   old_GroupAddress;
                        UINT8   old_TxGroupAddress;
                        UINT16  old_DeviceAddress;
                        UINT16  old_TxDeviceAddress;
                        UINT8   old_Modulation;
                        UINT32  old_Frequency;
                        UINT8   old_Bandwidth;
                        UINT8   old_SpreadingFactor;
                        UINT8   old_ErrorCoding;
                        UINT8   old_PowerLevel;
                        UINT8   old_TxControl;
                        UINT8   old_RxControl;
                        UINT16  old_RxWindowTime;
                        UINT8   old_LEDControl;
                        UINT8   old_RadioOptions;
                        // NEW
                        UINT8   old_FSKDataRate;
                        UINT8   old_PowerSavingMode;
                        INT16   old_LBTThreshold;

    void                RegisterClient(TWiMODLRHCIClient* client) { Client = client; }

    // connection handling
    bool                Open(const QString& comPort);
    bool                Close();
    void                Process();

    // device management commands
    TWiMODLRResult      PingRequest();
    TWiMODLRResult      FactoryReset();
    TWiMODLRResult      GetRadioConfiguration(TWiMODLR_RadioConfig& config, UINT8& status);
    TWiMODLRResult      SetRadioConfiguration(TWiMODLR_RadioConfig& config, UINT8 destMemory, UINT8& status);
    TWiMODLRResult      SetRadioConfiguration2(TWiMODLR_RadioConfig& config, UINT8 destMemory, UINT8& status);

    void                ConvertRadioConfiguration(TKeyValueList& list, const TWiMODLR_RadioConfig& config);
    const char*         GetDeviceMgmtStatusString(UINT8 status);

    // radio link services
    TWiMODLRResult      SendURadioMessage(UINT8* txMessage, UINT16 length, UINT8& status);
    TWiMODLRResult      SendURadioMessage2(UINT8* txMessage, UINT16 length, UINT8& status);
    void                ConvertRadioRxMessage(TKeyValueList& list, const TWiMODLR_HCIMessage& rxMsg);


    const char*         GetRadioLinkStatusString(UINT8 status);

    // other helper functions
    void                U32TimeToString(QString& timeString, UINT32 time, bool isoFormat = true);
    UINT32              GetFrequencyFromConfig(UINT32 regConfig);
    const char*         GetStringFromTable(const TWiMODLRHCI_IDString* table, UINT8 id);
    QString             GetCombinedStringFromTable(const TWiMODLRHCI_IDString* table, UINT8 id, int numBits);


    private:

    // transmit functions
    TWiMODLRResult      SendHCIMessage(UINT8 sapId, UINT8 msgID, UINT8 rxMsgID, UINT8* payload = 0, UINT16 length = 0);
    TWiMODLRResult      SendHCIMessage2(UINT8 sapId, UINT8 msgID, UINT8 rxMsgID, UINT8* payload = 0, UINT16 length = 0);
    TWiMODLRResult      PostMessage(UINT8 sapId, UINT8 msgID, UINT8* payload = 0, UINT16 length = 0);
    TWiMODLRResult      PostMessage2(UINT8 sapId, UINT8 msgID, UINT8* payload = 0, UINT16 length = 0);
    TWiMODLRResult      SendPacket(UINT8* txData, UINT16 length);
    TWiMODLRResult      SendPacket2(UINT8* txData, UINT16 length);

    // receiver functions
    bool                WaitForResponse(UINT8 rxSapID, UINT8 rxMsgID);
    UINT8*              ProcessRxMessage(UINT8* rxBuffer, UINT16 length);

    // dispatcher functions
    void                DispatchRxMessage           (TWiMODLR_HCIMessage& rxMsg);
    void                DispatchDeviceMgmtMessage   (TWiMODLR_HCIMessage& rxMsg);
    void                DispatchRadioLinkMessage    (TWiMODLR_HCIMessage& rxMsg);

    // debug support
    void                ShowMessage(const QString& msg, const TWiMODLR_HCIMessage& rxMsg);
    void                ShowMessage(const QString& msg);


    private:
    // reserve one Tx-Message-Buffer
    TWiMODLR_HCIMessage TxMessage;

    // receiver struct
    typedef struct
    {
        // flag indicating that a respons eis expected
        bool        Active;
        // flag indicating response successfully received
        bool        Done;
        // SAP ID of expected response
        UINT8       SapID;
        // Msg ID  of expected response
        UINT8       MsgID;
        // reserve one Rx-Message-Buffer
        TWiMODLR_HCIMessage Message;
        // CRC error counter
        int         CRCError;
        // Timeout (~1000ms)
        int         Timeout;
        // reserve one rx-buffer for recepton of SLIP encoded octet sequence
        UINT8       Buffer[512];
    }TReceiver;
    // receiver instance
    TReceiver           Rx;

    // reserve one tx-buffer for transmission of SLIP encoded octet sequence
    UINT8               TxBuffer[512];

    // SLIP communication layer instance
    TComSlip            ComSlip;

    // Serial Device (Comport Abstraction)
    TSerialDevice       SerialDevice;

    TWiMODLRHCIClient*  Client;




};

#endif // WIMODLRHCI_H

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
