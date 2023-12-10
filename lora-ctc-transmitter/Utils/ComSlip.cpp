//------------------------------------------------------------------------------
//
//	File:		ComSlip.cpp
//
//	Abstract:	SLIP Wrapper Class Implementation
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

#include "ComSlip.h"

//------------------------------------------------------------------------------
//
//  Protocol Definitions
//
//------------------------------------------------------------------------------

// SLIP Protocol Characters
#define SLIP_END					0xC0
#define	SLIP_ESC					0xDB
#define	SLIP_ESC_END				0xDC
#define	SLIP_ESC_ESC				0xDD

// SLIP Receiver/Decoder States
#define SLIPDEC_IDLE_STATE          0
#define	SLIPDEC_START_STATE			1
#define	SLIPDEC_IN_FRAME_STATE		2
#define	SLIPDEC_ESC_STATE			3

//------------------------------------------------------------------------------
//
//  Class Constructor
//
//------------------------------------------------------------------------------

TComSlip::TComSlip()
{
    // init to idle state, no rx-buffer avaliable
    RxState         =   SLIPDEC_IDLE_STATE;
    RxIndex         =   0;
    RxBuffer        =   0;
    RxBufferSize    =   0;
    RxClient        =   0;
}

//------------------------------------------------------------------------------
//
//  SendMessage
//
//  @brief: send a message as SLIP frame
//
//------------------------------------------------------------------------------

int
TComSlip::EncodeData(UINT8* dstBuffer, UINT16 dstBufferSize, UINT8* srcPtr, UINT16 msgLength)
{
    // save start pointer
    //int txLength = 0;

    // init TxBuffer
    TxBuffer = dstBuffer;

    // init TxIndex
    TxIndex  = 0;

    // init size
    TxBufferSize = dstBufferSize;

    // send start of SLIP message
    StoreTxByte(SLIP_END);

    // iterate over all message bytes
    while(msgLength--)
    {
        switch (*srcPtr)
        {
                case SLIP_END:
                    StoreTxByte(SLIP_ESC);
                    StoreTxByte(SLIP_ESC_END);
                    break;

                case SLIP_ESC:
                    StoreTxByte(SLIP_ESC);
                    StoreTxByte(SLIP_ESC_ESC);
                    break;

                default:
                    StoreTxByte(*srcPtr);
                    break;
        }
        // next byte
        srcPtr++;
    }

    // send end of SLIP message
    StoreTxByte(SLIP_END);

    // length ok ?
    if (TxIndex <= TxBufferSize)
        return TxIndex;

    // return tx length error
    return -1;
}

void
TComSlip::StoreTxByte(UINT8 txByte)
{
  if (TxIndex < TxBufferSize)
      TxBuffer[TxIndex++] = txByte;
}

//------------------------------------------------------------------------------
//
//  SetRxBuffer
//
//  @brief: configure a rx-buffer and enable receiver/decoder
//
//------------------------------------------------------------------------------

bool
TComSlip::SetRxBuffer(UINT8* rxBuffer, UINT16  rxBufferSize)
{
    // receiver in IDLE state and client already registered ?
    if ((RxState == SLIPDEC_IDLE_STATE) && RxClient)
    {
        // same buffer params
        RxBuffer        = rxBuffer;
        RxBufferSize    = rxBufferSize;

        // enable decoder
        RxState = SLIPDEC_START_STATE;

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
//
//  DecodeData
//
//  @brief: process received byte stream
//
//------------------------------------------------------------------------------

void
TComSlip::DecodeData(UINT8* rxData, UINT16 length)
{
    // init result
    //int result = 0;

    // iterate over all received bytes
    while(length--)
    {
        // get rxByte
        UINT8 rxByte = *rxData++;

        // decode according to current state
        switch(RxState)
        {
            case    SLIPDEC_START_STATE:
                    // start of SLIP frame ?
                    if(rxByte == SLIP_END)
                    {
                        // init read index
                        RxIndex = 0;

                        // next state
                        RxState = SLIPDEC_IN_FRAME_STATE;
                    }
                    break;

            case    SLIPDEC_IN_FRAME_STATE:
                    switch(rxByte)
                    {
                        case    SLIP_END:
                                // data received ?
                                if(RxIndex > 0)
                                {
                                    // yes, return received decoded length
                                    if (RxClient)
                                    {
                                        RxBuffer = RxClient->ProcessRxMessage(RxBuffer, RxIndex);
                                        if (!RxBuffer)
                                        {
                                            RxState = SLIPDEC_IDLE_STATE;
                                        }
                                        else
                                        {
                                            RxState = SLIPDEC_START_STATE;
                                        }
                                    }
                                    else
                                    {
                                        // disable decoder, temp. no buffer avaliable
                                        RxState = SLIPDEC_IDLE_STATE;
                                    }
                                }
                                // init read index
                                RxIndex = 0;
                                break;

                        case  SLIP_ESC:
                                // enter escape sequence state
                                RxState = SLIPDEC_ESC_STATE;
                                break;

                        default:
                                // store byte
                                StoreRxByte(rxByte);
                                break;
                    }
                    break;

            case    SLIPDEC_ESC_STATE:
                    switch(rxByte)
                    {
                        case    SLIP_ESC_END:
                                StoreRxByte(SLIP_END);
                                // quit escape sequence state
                                RxState = SLIPDEC_IN_FRAME_STATE;
                                break;

                        case    SLIP_ESC_ESC:
                                StoreRxByte(SLIP_ESC);
                                // quit escape sequence state
                                RxState = SLIPDEC_IN_FRAME_STATE;
                                break;

                        default:
                                // abort frame receiption
                                RxState = SLIPDEC_START_STATE;
                                break;
                    }
                    break;

            default:
                    break;
        }
    }
}

//------------------------------------------------------------------------------
//
//  StoreRxByte
//
//  @brief: store SLIP decoded rxByte
//
//------------------------------------------------------------------------------

void
TComSlip::StoreRxByte(UINT8 rxByte)
{
    if (RxIndex < RxBufferSize)
        RxBuffer[RxIndex++] = rxByte;
}

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
