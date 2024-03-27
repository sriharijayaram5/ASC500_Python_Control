/******************************************************************************
 *
 *  Project:        Daisy Client Library
 *
 *  Filename:       asc500helpers.h
 *
 *  Purpose:        Convenience Functions supporting Daisybase for ASC500
 *
 *  Author:         NHands GmbH & Co KG
 */
/*****************************************************************************/
/** @file asc500helpers.h
 *  @brief Convenience Functions supporting Daisybase for ASC500
 *
 *  Defines functions that simplify the use of the daisybase library for
 *  the control of the ASC500 SPM controller.
 *  The functions are not part of the daisybase lib; the source code may
 *  be included in application projects.
 *  These functions must not be called from the context of a daisybase
 *  callback function.
 */
/*****************************************************************************/
/* $Id: asc500helpers.h,v 1.11 2012/03/23 17:48:34 trurl Exp $ */

#ifndef __ASC500HELPERS_H
#define __ASC500HELPERS_H

#include "daisydecl.h"
#include "daisybase.h"
#include "asc500.h"


/** @brief  status codes of the frame callback functions                        */
typedef enum {
  ASC500_frameTimeout,                /**< Reason of callback is timeout        */
  ASC500_frameLength,                 /**< Reason of callback is reached length */
  ASC500_frameIndex                   /**< Reason of callback is index of zero  */
} ASC500_bufferStatus;


/** @brief  status codes of the feedback functions                      */
typedef enum {
  ASC500_feedbackOff,                 /**< Feedback off                 */
  ASC500_feedbackOn,                  /**< Feedback on                  */
  ASC500_feedbackRetracted            /**< Feedback retracted           */
} ASC500_feedbackStatus;



/** @brief Frame Callback Function
 *
 *  Functions of this type can be registered as callback functions for frame
 *  data. They will be called by the event loop as soon as one of the
 *  following conditions occur:
 *  Received data is exeeding the given maximum buffer length, a timeout
 *  while receiving data occurs, an index of 0 in the received data stream 
 *  appears for a second time (indicating data belong to a new frame).
 *
 *  @param  channel  Data channel that has sent the data
 *  @param  status   Reason for calling function
 */
typedef void (* ASC500_BufferCallback) ( Int32 channel,
                                         ASC500_bufferStatus status );


/** @brief Path Callback Function
 *
 *  Functions of this type can be registered as callback functions for 
 *  path mode. They will be called by the event loop as soon as the scanner
 *  reached a destination position on the given path.
 */
typedef void (* ASC500_PathCallback) ( void );



/** @brief Configure a Channel
 *
 *  Configures what kind of data is sent on a specific data channel.
 *
 *  @param  number   Number of the channel to be configured
 *  @param  trigger  Trigger source for data output
 *  @param  source   Data source for the channel
 *  @param  average  If the controller should average data over
 *                   the sample time (boolean)
 *  @param  smpTime  Time per sample in [s]. Has no effect unless the
 *                   trigger parameter is "timer based".
 *  @return
 *     @ref DYB_Ok           - Success
 *  @n @ref DYB_OutOfRange   - Invalid channel number
 *  @n @ref DYB_ServerLost   - Can't send - server not running?
 */
DYB_API DYB_Rc ASC500_configureChannel( Int32 number,
                                        Int32 trigger,
                                        Int32 source,
                                        Bln32 average,
                                        float smpTime );


/** @brief Control Scanner Offset
 *
 *  Sets and retreives the offset point of the scan range.
 *  The parameters are used for input of the demanded value
 *  and for output of the value actually achieved.
 *  @param  x        In/Out: x component [m].
 *  @param  y        In/Out: y component [m].
 *  @param  set      1: Send the supplied values to the controller
 *                   0: Ignore input; only retreive the results
 *  @return          see @ref DYB_setParameterSync()
 */
DYB_API DYB_Rc ASC500_controlScanOffset( float * x,
                                         float * y,
                                         Bln32 set );


/** @brief Control Scan Range
 *
 *  Sets and retreives the extent of the scan range and the number of samples
 *  @param  columns    In/Out: Number of scan columns, i.e. samples per line.
 *  @param  lines      In/Out: Number of scan lines.
 *  @param  pixelSize  In/Out: Width of a line / column [m].
 *  @param  set        1: Send the supplied values to the controller
 *                     0: Ignore input; only retreive the results
 *  @return            see @ref DYB_setParameterSync
 */
DYB_API DYB_Rc ASC500_controlScanPixels( Int32 * columns,
                                         Int32 * lines,
                                         float * pixelSize,
                                         Bln32 set );


/** @brief Control Scan Rotation
 *
 *  Sets and retreives the rotation angle of the scan range.
 *  @param  angle    In/Out: Rotation anlge [degrees]
 *  @param  set      1: Send the supplied values to the controller
 *                   0: Ignore input; only retreive the results
 *  @return          see @ref DYB_setParameterSync
 */
DYB_API DYB_Rc ASC500_controlScanRotation( float * angle,
                                           Bln32 set );


/** @brief Control Scanner Sample Time
 *
 *  Sets and retreives the scanner's sample time.
 *  @param  time     In/Out: Sample time [s]
 *  @param  set      1: Send the supplied values to the controller
 *                   0: Ignore input; only retreive the results
 *  @return          see @ref DYB_setParameterSync
 */
DYB_API DYB_Rc ASC500_controlSampleTime( float * time,
                                         Bln32 set );


/** @brief Control Scanner State
 *
 *  Sets and retreives the scanner's running state.
 *  @param  state    In/Out: Running state (SCANRUN_...)
 *  @param  set      1: Send the supplied values to the controller
 *                   0: Ignore input; only retreive the results
 *  @return          see @ref DYB_setParameterSync
 */
DYB_API DYB_Rc ASC500_controlScannerState( Int32 * state,
                                           Bln32 set );


/** @brief Activate Outputs
 *
 *  Sets and retreives the activation state of electric outputs.
 *  It encapsulates the explicitly asynchronous activation protocol
 *  and may therefore take up to a second.
 *  @param  state    In/Out: Activate outputs (bool)
 *  @param  set      1: Send the supplied values to the controller
 *                   0: Ignore input; only retreive the results
 *  @return          see @ref DYB_setParameterSync
 */
DYB_API DYB_Rc ASC500_controlOutputActivation( Int32 * state,
                                               Bln32 set );


/** @brief Register Frame Callback Function
 *
 *  Registers a callback function for a data channel associated to the 
 *  scanner. That function will be called when a condition occurs, defined
 *  in ASC500_setFrameBuffer. A callback function registered previously 
 *  is unregistered.
 *
 *  The function is called in the context of a thread that serves the
 *  event loop. If it is not processed fast enough, events or data may be lost.
 *
 *  @param  channel    Number of the data channel. Numbers begin with 0,
 *                     the maximum is product specifc.
 *  @param  callback   Callback function for that channel,
 *                     use NULL to unregister a function
 *  @return
 *     @ref DYB_Ok           - Successful
 *  @n @ref DYB_ServerLost   - Data request failed - server not running?
 */
DYB_API DYB_Rc ASC500_setFrameCallback( Int32 channel,
                                        ASC500_BufferCallback callback );

/** @brief Set Frame Buffer
 *
 *  Sets the frame buffer and callback conditions for the channel
 *  @param  channel    Number of the data channel. Numbers begin with 0,
 *                     the maximum is product specifc.
 *  @param  buffer     Pointer to the data buffer
 *  @param  meta       Pointer to the corresponding meta data
 *  @param  maxLength  Maximum count of data words the buffer can hold
 *  @return            see @ref DYB_setParameterSync
 */
DYB_API DYB_Rc ASC500_setFrameBuffer( Int32 channel,
                                      const Int32 * buffer,
                                      const DYB_Meta * meta,
                                      Int32 maxLength);


/** @brief Get Z Position
 *
 *  Retrieves the actual z position.
 *  @param  pos        Z position in pm
 *  @return            see @ref DYB_setParameterSync
 */
DYB_API DYB_Rc ASC500_getZPos( Int32 * pos );



/** @brief Get X and Y Position
 *
 *  Retrieves the actual x and y position.
 *  @param   xpos      Output: X position in pm
 *  @param   ypos      Output: Y position in pm
 *  @return            see @ref DYB_setParameterSync
 */
DYB_API DYB_Rc ASC500_getXYPos( Int32 * xpos,
                                Int32 * ypos );


/** @brief Control Auto Approach State
 *
 *  Sets and retreives the auto approach's running state.
 *  @param  on       In/Out: Running state (true/false)
 *  @param  set      1: Send the supplied values to the controller
 *                   0: Ignore input; only retreive the results
 *  @return          see @ref DYB_setParameterSync
 */
DYB_API DYB_Rc ASC500_controlAutoAproach( Bln32 * on,
                                          Int32 set );


/** @brief Control Coarse Continuous State
 *
 *  Sets the coarse continous running state.
 *  @param  axis     selected axis
 *  @param  dir      direction: 0 backward, 1 forward
 *  @param  on       In/Out: Running state (true/false)
 *  @return          see @ref DYB_setParameterSync
 */
DYB_API DYB_Rc ASC500_controlCoarseCont( Int32 axis,
                                         Int32 dir,
                                         Bln32 on );


/** @brief Control Coarse Single
 *
 *  Causes a single step.
 *  @param  axis     selected axis
 *  @param  dir      direction: 0 backward, 1 forward
 *  @return          see @ref DYB_setParameterSync
 */
DYB_API DYB_Rc ASC500_controlCoarseSingle( Int32 axis,
                                           Int32 dir );


/** @brief Control Feedback State
 *
 *  Sets and retreives the feddback's running state.
 *  @param  state    In/Out: Feedback state (ASC500_feedbackStatus)
 *  @param  set      1: Send the supplied values to the controller
 *                   0: Ignore input; only retreive the results
 *  @return          see @ref DYB_setParameterSync
 */
DYB_API DYB_Rc ASC500_controlFeedback( Int32 * state,
                                       Int32 set );


/** @brief Control Feedback Proportional and Integral Part
 *
 *  Sets the proportional and integral part for the feedback.
 *  @param  p        proportional part 
 *  @param  i        integral part in Hz
 *  @param  set      1: Send the supplied values to the controller
 *                   0: Ignore input; only retreive the results
 *  @return          see @ref DYB_setParameterSync
 */
DYB_API DYB_Rc ASC500_controlFeedbackPI( float * p,
                                         float * i,
                                         Int32 set );


/** @brief Interpret return codes
 *
 *  Returns a descriptive text for a given daisybase return code
 *  @param  rc       return code of a daisybase function
 *  @return          error description (if any) or "Ok";
 *                   "????" indicates an invalid error code
 */
DYB_API const char * ASC500_printRc( DYB_Rc rc );


#endif
