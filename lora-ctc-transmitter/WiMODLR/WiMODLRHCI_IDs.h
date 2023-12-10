//------------------------------------------------------------------------------
//
//	File:		WiMODLRHCI_IDs.h
//
//	Abstract:	WiMODLR HCI Message Identifier
//
//	Version:	0.1
//
//	Date:		09.15.2014
//
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//------------------------------------------------------------------------------

#ifndef WIMODLR_HCI_IDS_H
#define WIMODLR_HCI_IDS_H

//------------------------------------------------------------------------------
//
// Service Access Point Identifier
//
//------------------------------------------------------------------------------

#define DEVMGMT_SAP_ID                      0x01
#define DATALINK_SAP_ID                     0x03

//------------------------------------------------------------------------------
//
// 1. Device Management Message Identifier
//
//------------------------------------------------------------------------------

// Status Codes
#define	DEVMGMT_STATUS_OK					0x00
#define	DEVMGMT_STATUS_ERROR				0x01
#define	DEVMGMT_STATUS_CMD_NOT_SUPPORTED	0x02
#define	DEVMGMT_STATUS_WRONG_PARAMETER		0x03
#define DEVMGMT_STATUS_WRONG_DEVICE_MODE    0x04

// Message IDs
#define DEVMGMT_MSG_PING_REQ                0x01
#define DEVMGMT_MSG_PING_RSP                0x02

#define DEVMGMT_MSG_GET_DEVICEINFO_REQ      0x03
#define DEVMGMT_MSG_GET_DEVICEINFO_RSP      0x04

#define DEVMGMT_MSG_GET_FW_VERSION_REQ      0x05
#define DEVMGMT_MSG_GET_FW_VERSION_RSP      0x06

#define	DEVMGMT_MSG_RESET_REQ				0x07
#define	DEVMGMT_MSG_RESET_RSP				0x08

#define	DEVMGMT_MSG_SET_RTC_REQ				0x0D
#define	DEVMGMT_MSG_SET_RTC_RSP				0x0E
#define	DEVMGMT_MSG_GET_RTC_REQ				0x0F
#define	DEVMGMT_MSG_GET_RTC_RSP				0x10

#define DEVMGMT_MSG_SET_RADIO_CONFIG_REQ    0x11
#define DEVMGMT_MSG_SET_RADIO_CONFIG_RSP    0x12
#define DEVMGMT_MSG_GET_RADIO_CONFIG_REQ    0x13
#define DEVMGMT_MSG_GET_RADIO_CONFIG_RSP    0x14

#define	DEVMGMT_MSG_RESET_RADIO_CONFIG_REQ  0x15
#define	DEVMGMT_MSG_RESET_RADIO_CONFIG_RSP  0x16

#define	DEVMGMT_MSG_GET_SYSTEM_STATUS_REQ	0x17
#define	DEVMGMT_MSG_GET_SYSTEM_STATUS_RSP	0x18

#define DEVMGMT_MSG_SET_PSV_MODE_REQ        0x1B
#define DEVMGMT_MSG_SET_PSV_MODE_RSP        0x1C

//------------------------------------------------------------------------------
//
// 3. Radio Data Link
//
//------------------------------------------------------------------------------

// HCI Radio Message Defines
// 3 Byte Header: 1 Byte Dst Group Address, 2 Byte Dst Device Address

#define DATALINK_RADIO_HEADER_SIZE          3

// Status Codes
#define DATALINK_STATUS_OK                  0x00
#define DATALINK_STATUS_ERROR               0x01
#define	DATALINK_STATUS_CMD_NOT_SUPPORTED   0x02
#define	DATALINK_STATUS_WRONG_PARAMETER		0x03
#define DATALINK_STATUS_WRONG_DEVICE_MODE   0x04
#define	DATALINK_STATUS_MEDIA_BUSY    		0x05
#define	DATALINK_STATUS_DEVICE_BUSY   		0x06
#define DATALINK_STATUS_QUEUE_FULL   		0x07
#define DATALINK_STATUS_MSGLENGTH_ERROR     0x08

// Message IDs
#define DATALINK_MSG_SEND_URADIO_MSG_REQ    0x01
#define DATALINK_MSG_SEND_URADIO_MSG_RSP    0x02
#define DATALINK_MSG_RECV_URADIO_MSG_IND    0x04

#define DATALINK_MSG_SENT_URADIO_MSG_IND    0x06

#define DATALINK_MSG_RECV_RAWRADIO_MSG_IND  0x08

// Radio Packet Defines
#define DATALINK_MSG_HEADER_SIZE            7
#define DATALINK_MSG_FOOTER_SIZE            7

#endif // WIMODLRHCI_IDS_H

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
