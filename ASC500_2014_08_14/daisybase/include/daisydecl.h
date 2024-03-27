/******************************************************************************
 *
 *  Project:        Daisy Client Library
 *
 *  Filename:       daisydecl.h
 *
 *  Purpose:        Variants of Interface Declarations, Base Types
 *
 *  Author:         NHands GmbH & Co KG
 *
 */
/******************************************************************************/
/** @file daisydecl.h
 *  Variants of Interface Declarations, Base Types
 *
 *  Declaration of macros required for use with C and C++ and for the Windows
 *  DLL interface. Basic types.
 */
/******************************************************************************/
/* $Id: daisydecl.h,v 1.5 2013/02/13 11:12:24 trurl Exp $ */

#ifndef __DAISYDECL_H
#define __DAISYDECL_H

#ifdef __cplusplus
#define EXTC extern "C"                          /**< For use with C++        */
#else
#define EXTC extern                              /**< For use with C          */
#endif

#ifdef unix
#define DYB_API EXTC                             /**< Not required for Unix   */
#define DYB_CC                                   /**< Not required for Unix   */
#else
#define DYB_CC       __stdcall                   /**< Calling convention      */
#ifdef  DYB_EXPORTS
#define DYB_API EXTC __declspec(dllexport)       /**< For internal use        */
#else
#define DYB_API EXTC __declspec(dllimport)       /**< For external use        */
#endif
#endif

#ifdef _MSC_VER
typedef __int32 Int32;                           /**< 32 bit signed integer   */
#else
#include <inttypes.h>
typedef int32_t Int32;                           /**< 32 bit signed integer   */
#endif

typedef float  Flt32;                            /**< Single precision float  */
typedef int    Bln32;                            /**< Boolean (documentation) */
typedef Int32  DYB_Address;                      /**< Control parameter id    */

#endif
