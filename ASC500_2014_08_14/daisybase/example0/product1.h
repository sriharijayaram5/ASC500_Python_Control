/******************************************************************************
 *
 *  Project:        Daisy Client Library
 *
 *  Filename:       product1.h
 *
 *  Purpose:        Trivial sample for product specific include file
 *
 *  Author:         NHands GmbH & Co KG
 *
 */
/******************************************************************************/
/** @file product1.h
 *  Trivial sample for product specific include file
 *
 *  Product specific constants for use with the daisybase library.
 *  The file belongs to the fictious product "product1", but addresses
 *  only basic functionality that will be available in the servers
 *  spmctrl, cfmctrl and others.
 */
/******************************************************************************/
/* $Id: product1.h,v 1.3 2010/11/22 19:27:05 trurl Exp $ */

#ifndef __PRODUCT1_H
#define __PRODUCT1_H

// Global constants
#define PRODUCT_NAME   "product1" //!< Product name
#define PORT_NUMBER          7000 //!< Product specific TCP port
#define CHANNEL_COUNT           7 //!< Number of available data channels

// Constants for the scanner state
#define SCANRUN_OFF          0x00 //!< Scanner on
#define SCANRUN_ON           0x01 //!< Scanner off
#define SCANRUN_PAUSE        0x02 //!< Scanner pause

// Constants for ID_CHAN_CONNECT
#define CHANCONN_DISABLED    0x00 //!< Channel disconnected
#define CHANCONN_SCANNER     0x01 //!< Channel triggered by scanner
#define CHANCONN_PERMANENT   0x02 //!< Runs permanently

// Constants for ID_CHAN_ADC
#define CHANADC_ADC_MIN         0 //!< First "real" ADC
#define CHANADC_ADC_MAX         5 //!< Last "real" ADC
#define CHANADC_ZOUT            9 //!< Readback Z
#define CHANADC_XOUT           10 //!< Scanner Readback X
#define CHANADC_YOUT           11 //!< Scanner Readback Y

// Constants for ID_SRV_TRACEFLG (Bitmasks)
#define TRACE_FULL         0x0001 //!< Trace full length of data
#define TRACE_GLOBAL       0x0004 //!< Trace warnings and infos
#define TRACE_CS_IN        0x0008 //!< Trace telegrams from clients
#define TRACE_CS_OUT       0x0010 //!< Trace telegrams to clients
#define TRACE_UC_IN        0x0020 //!< Trace telegrams from controller
#define TRACE_UC_OUT       0x0040 //!< Trace telegrams to controller
#define TRACE_DATA         0x0080 //!< Include data telegrams in trace
#define TRACE_EVT          0x0100 //!< Include event telegrams in trace

// Parameter IDs (DYB_Address)
#define ID_CHAN_CONNECT    0x0030 //!< Acquisition trigger for data channel; Index = channelNo
#define ID_CHAN_ADC        0x0031 //!< Data source for data channel; Index = channelNo
#define ID_CHAN_POINTS     0x0032 //!< Measurements per result; Index = channelNo
#define ID_OUTPUT_STATUS   0x0140 //!< Status of the outputs, 1: active, read-only
#define ID_OUTPUT_ACTIVATE 0x0141 //!< Activate outputs (bool)
#define ID_SCAN_RUNNING    0x0100 //!< Scanner running (see SCANRUN_...)
#define ID_SRV_TRACEFLG    0x10A0 //!< Server Tracing Flags (see TRACE_...)
#define ID_DATA_EN         0x0146 //!< Enable (1) / disable (0) all data channels

#endif
