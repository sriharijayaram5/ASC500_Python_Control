/******************************************************************************
 *
 *  Project:        Daisy Client Library
 *
 *  Filename:       asc500helpers.c
 *
 *  Purpose:        Convenience Functions supporting Daisybase for ASC500
 *
 *  Author:         NHands GmbH & Co KG
 */
/*****************************************************************************/
/* $Id: asc500helpers.c,v 1.9 2010/11/22 19:27:05 trurl Exp $ */

#ifdef unix
#include <string.h>                       /* string.h for memcpy          */
#include <unistd.h>                       /* unistd.h for usleep          */
#define SLEEP(x) usleep(x*1000)
#else
#include <windows.h>                      /* windows.h for Sleep          */
#define SLEEP(x) Sleep(x)
#endif

#include "asc500helpers.h"


#define SAMPLE_TIMEBASE 2.5e-6         // Sampling time base; unit of sample time: 2.5us
#define LENGTH_UNIT     1.e-12         // Length unit: pm
#define ROT_UNIT        (360./0x10000) // Internal unit for angles



/* Print out return codes */
const char * ASC500_printRc( DYB_Rc rc )
{
  switch ( rc ) {
  case DYB_Ok:            return "Ok";
  case DYB_Error:         return "Unknown / other error";
  case DYB_Timeout:       return "Communication timeout";
  case DYB_NotConnected:  return "No contact to controller via USB";
  case DYB_DriverError:   return "Error when calling USB driver";
  case DYB_FileNotFound:  return "Controller boot image not found";
  case DYB_SrvNotFound:   return "Server executable not found";
  case DYB_ServerLost:    return "No contact to the server";
  case DYB_OutOfRange:    return "Invalid parameter in fct. call";
  case DYB_WrongContext:  return "Call in invalid thread context";
  case DYB_XmlError:      return "Invalid format of profile file";
  case DYB_OpenError:     return "Can't open specified file";
  default:                return "????";
  }
}


DYB_Rc ASC500_configureChannel( Int32 number,  Int32 trigger, Int32 source,
                                Bln32 average, float smpTime )
{
  DYB_Rc rc = DYB_Ok;
  Int32  returned;

  if ( number < 0 || number >= ASC500_DATA_CHANNELS )
    rc = DYB_OutOfRange;

  if ( rc == DYB_Ok ) {
    rc = DYB_setParameterSync( ID_CHAN_CONNECT, number, trigger, &returned );
    if ( rc == DYB_Ok && returned != trigger ) {
      /* Not accepted for some reason. It is useless to continue. */
      rc = DYB_Error;
    }
  }

  if ( rc == DYB_Ok ) {
    rc = DYB_setParameterSync( ID_CHAN_ADC, number, source, &returned );
    if ( rc == DYB_Ok && returned != source )
      /* Not accepted for some reason. It is useless to continue. */
      rc = DYB_Error;
  }

  if ( rc == DYB_Ok )
    rc = DYB_setParameterSync( ID_CHAN_AVG_MAX, number, average, &returned );

  if ( rc == DYB_Ok && trigger == CHANCONN_PERMANENT ) {
    Int32 points = (Int32) (smpTime / SAMPLE_TIMEBASE + 0.5);  // in [2.5us]
    rc = DYB_setParameterSync( ID_CHAN_POINTS, number, points, &returned );
  }
  
  return rc;
}


DYB_Rc ASC500_controlScanOffset( float * x, float * y, Bln32 set )
{
  DYB_Rc rc1, rc2;
  Int32  ix, iy;

  if ( set ) {
    ix = (Int32) (*x / LENGTH_UNIT);     /* convert meters to pm  */
    iy = (Int32) (*y / LENGTH_UNIT);
    rc1 = DYB_setParameterSync( ID_SCAN_OF_IN_X, 0, ix, &ix );
    rc2 = DYB_setParameterSync( ID_SCAN_OF_IN_Y, 0, iy, &iy );
  }
  else {
    rc1 = DYB_getParameterSync( ID_SCAN_OF_IN_X, 0, &ix );
    rc2 = DYB_getParameterSync( ID_SCAN_OF_IN_Y, 0, &iy );
  }

  *x = (float) (ix * LENGTH_UNIT);       /* convert back to meters */
  *y = (float) (iy * LENGTH_UNIT);
  return rc1 == DYB_Ok ? rc2 : rc1;      /* find worst return code */
}

DYB_Rc ASC500_controlScanPixels( Int32 * columns, Int32 * lines,
                                 float * pixSize, Bln32 set )
{
  DYB_Rc rc1, rc2, rc3;
  Int32  iPs;

  if ( set ) {
    iPs = (Int32) (*pixSize / LENGTH_UNIT); /* convert meters to pm  */
    rc1 = DYB_setParameterSync( ID_SCAN_COLUMNS, 0, *columns, columns );
    rc2 = DYB_setParameterSync( ID_SCAN_LINES,   0, *lines,   lines   );
    rc3 = DYB_setParameterSync( ID_SCAN_PIXEL,   0, iPs,      &iPs    );
  }
  else {
    rc1 = DYB_getParameterSync( ID_SCAN_COLUMNS, 0, columns );
    rc2 = DYB_getParameterSync( ID_SCAN_LINES,   0, lines   );
    rc3 = DYB_getParameterSync( ID_SCAN_PIXEL,   0, &iPs    );
  }

  *pixSize = (float) (iPs * LENGTH_UNIT); /* convert back to meters */
  return rc1 == DYB_Ok ? ( rc2 == DYB_Ok ? rc3 : rc2 ) : rc1;
}

DYB_Rc ASC500_controlScanRotation( float * angle, Bln32 set )
{
  DYB_Rc rc1;
  Int32  rot;

  if ( set ) {
    rot = (Int32) (*angle / ROT_UNIT + .5);
    rc1 = DYB_setParameterSync( ID_SCAN_ROTATION, 0, rot, &rot );
  }
  else {
    rc1 = DYB_getParameterSync( ID_SCAN_ROTATION, 0, &rot );
  }

  *angle = (float) (rot * ROT_UNIT);
  return rc1;
}

DYB_Rc ASC500_controlSampleTime( float * time, Bln32 set )
{
  DYB_Rc rc1;
  Int32  points;

  if ( set ) {
    points = (Int32) (*time / SAMPLE_TIMEBASE + 0.5);  // in [2.5us]
    rc1 = DYB_setParameterSync( ID_SCAN_MSPPX, 0, points, &points );
  }
  else {
    rc1 = DYB_getParameterSync( ID_SCAN_MSPPX, 0, &points );
  }

  *time = (float) (points * SAMPLE_TIMEBASE);
  return rc1;
}

DYB_Rc ASC500_controlScannerState( Int32 * state, Bln32 set )
{
  return set
    ? DYB_setParameterSync( ID_SCAN_RUNNING, 0, *state, state )
    : DYB_getParameterSync( ID_SCAN_RUNNING, 0, state );
}

DYB_Rc ASC500_controlOutputActivation( Int32 * state, Bln32 set )
{
  DYB_Rc rc1;
  Int32  readback, tries = 0;

  if ( set ) {
    // Output Activation is requested by ID_OUTPUT_ACTIVATE but takes some time
    // and must be acknowledged by ID_OUTPUT_STATE
    rc1 = DYB_setParameterAsync( ID_OUTPUT_ACTIVATE, 0, *state );
    do {
      SLEEP( 50 );
      rc1 = DYB_getParameterSync( ID_OUTPUT_STATUS, 0, &readback );
      tries++;
    } while ( rc1 == DYB_Ok && readback != *state && tries < 20 );
    *state = readback;
  }
  else {
    rc1 = DYB_getParameterSync( ID_OUTPUT_STATUS, 0, state );
  }

  return rc1;
}



typedef struct {
  Int32 *               _buffer;
  DYB_Meta *            _meta;
  Int32                 _maxLength;
  Int32                 _timeout;
  Int32                 _actualLength;
  Bln32                 _startDetected;
  void *                _callback;
} FrameBufferInfo;


static FrameBufferInfo _bufferInfo[ASC500_DATA_CHANNELS];


DYB_Rc ASC500_setFrameBuffer( Int32 channel, const Int32 * buffer, const DYB_Meta * meta, Int32 maxLength)
{
  if ((0 > channel) || (ASC500_DATA_CHANNELS - 1 < channel))
    return DYB_OutOfRange;

  _bufferInfo[channel]._buffer = (Int32*)buffer;
  _bufferInfo[channel]._meta = (DYB_Meta*)meta;
  _bufferInfo[channel]._maxLength = maxLength;
  _bufferInfo[channel]._timeout = 0;
  _bufferInfo[channel]._actualLength = 0;
  _bufferInfo[channel]._startDetected = 0;

  return DYB_Ok;
}


/* Data callback function for frame data*/
static void frameDataCallback( Int32 channel, Int32 length, Int32 index,
                          const Int32 * data, const DYB_Meta * meta ) 
{
  int copyLen = 0;
  int remaining = 0;
  ASC500_BufferCallback cb = (ASC500_BufferCallback)(_bufferInfo[channel]._callback);
  int byteOffset = 0;

  // check for registered callback
  if (NULL == _bufferInfo[channel]._callback)
    return;

  if (NULL == _bufferInfo[channel]._buffer)
    return;

  // check index for 0
  if (0 == index){
    if (0 == _bufferInfo[channel]._startDetected){
      _bufferInfo[channel]._startDetected = 1;
      // write data to buffer
    }
    else{
      // new frame data, callback condition
      DYB_setDataCallback( channel, NULL );
      cb(channel, ASC500_frameIndex);
      return;
    }
  }
  else{
    if (0 == _bufferInfo[channel]._startDetected){
      // discard data
      return;
    }
    else{
      // write data to buffer
    }
  }

  // write data to buffer
  remaining = _bufferInfo[channel]._maxLength - _bufferInfo[channel]._actualLength;

  copyLen = length > remaining ? remaining : length;
  copyLen = copyLen < 0 ? 0 : copyLen;
  byteOffset = _bufferInfo[channel]._actualLength/* * sizeof(Int32)*/;
  memcpy(_bufferInfo[channel]._buffer + byteOffset, data, copyLen * sizeof(Int32));
  _bufferInfo[channel]._actualLength += copyLen;

  if (NULL != _bufferInfo[channel]._meta)
    memcpy((void*)(_bufferInfo[channel]._meta), (void*)meta, sizeof(DYB_Meta));

  // check length
  remaining = _bufferInfo[channel]._maxLength - _bufferInfo[channel]._actualLength;
  if (remaining <= 0){
    // data complete, callback condition
    DYB_setDataCallback( channel, NULL );
    cb(channel, ASC500_frameLength);
  }
}


DYB_Rc ASC500_setFrameCallback( Int32 channel, ASC500_BufferCallback callback )
{
  // un-/register data callback function
  DYB_Rc rc;
  if (NULL != callback)
    rc = DYB_setDataCallback( channel, frameDataCallback );
  else
    rc = DYB_setDataCallback( channel, NULL );

  _bufferInfo[channel]._callback = (void*)callback;

  return rc;
}


DYB_Rc ASC500_getZPos( Int32 * pos )
{
  DYB_Rc rc = DYB_OutOfRange;
  if (NULL != pos){
    Int32 value = 0;
    rc = DYB_getParameterSync( ID_REG_GET_Z_M, 0, &value );
    *pos = value;
  }
  return rc;
}


DYB_Rc ASC500_getXYPos( Int32 * xpos, Int32 * ypos )
{
  Int32 volt = 0, gaugex = 0, gaugey = 0, xraw = 0, yraw = 0;
  Int32 offx = 0, offy = 0;
  double dx = 0, dy = 0;
  DYB_Rc rc = DYB_OutOfRange;
  if ((NULL != xpos) && (NULL != ypos)){
    rc = DYB_getParameterSync( ID_ACT_VOLT_LIM, 0, &volt );
    if (DYB_Ok != rc)
      return rc;
    rc = DYB_getParameterSync( ID_ACT_GAUGE_X, 0, &gaugex );
    if (DYB_Ok != rc)
      return rc;
    rc = DYB_getParameterSync( ID_ACT_GAUGE_Y, 0, &gaugey );
    if (DYB_Ok != rc)
      return rc;
    rc = DYB_getParameterSync( ID_SCAN_OF_IN_X, 0, &offx );
    if (DYB_Ok != rc)
      return rc;
    rc = DYB_getParameterSync( ID_SCAN_OF_IN_Y, 0, &offy );
    if (DYB_Ok != rc)
      return rc;

    rc = DYB_getParameterSync( ID_SCAN_CURR_X, 0, &xraw );
    if (DYB_Ok != rc)
      return rc;
    rc = DYB_getParameterSync( ID_SCAN_CURR_Y, 0, &yraw );
    if (DYB_Ok != rc)
      return rc;

    dx = (xraw / 131072.) * ((double)gaugex / (1000. * volt / 32767.) );
    dy = (yraw / 131072.) * ((double)gaugey / (1000. * volt / 32767.) );

    *xpos = offx + (Int32) dx;
    *ypos = offy + (Int32) dy;
  }
  return rc;
}



DYB_Rc ASC500_controlAutoAproach( Bln32 * on, Int32 set )
{
  DYB_Rc rc = DYB_Ok;
  if (set){
    Int32 returned = 0;
    Int32 value = *on;
    rc = DYB_setParameterSync( ID_AAP_CTRL, 0, value, &returned );
    if ( rc == DYB_Ok && returned != value ) {
      /* Not accepted for some reason. It is useless to continue. */
      rc = DYB_Error;
    }
  }
  else{
    Int32 value = 0;
    rc = DYB_getParameterSync( ID_AAP_CTRL, 0, &value );
    *on = value;
  }

  return rc;
}




DYB_Rc ASC500_controlCoarseCont( Int32 axis, Int32 dir, Bln32 on )
{
  DYB_Rc rc = DYB_Ok;
  Int32 returned = 0;
  Int32 value = on;
  Int32 adr;

  if ((0 > dir) || (1 < dir) || (0 > axis) || (2 < axis))
    return DYB_OutOfRange;

  if (dir == 0)
    adr = ID_CRS_AXIS_CDN;
  else 
    adr = ID_CRS_AXIS_CUP;
  rc = DYB_setParameterSync( adr, axis, value, &returned );
  if ( rc == DYB_Ok && returned != value ) {
    // Not accepted for some reason. It is useless to continue.
    rc = DYB_Error;
  }
  return rc;
}


DYB_Rc ASC500_controlCoarseSingle( Int32 axis, Int32 dir )
{
  DYB_Rc rc = DYB_Ok;
  Int32 returned = 0;
  Int32 value = 1;
  Int32 adr;

  if ( (0 > dir) || (1 < dir) || (0 > axis) || (2 < axis) ){
    return DYB_OutOfRange;
  }

  if (dir == 0)
    adr = ID_CRS_AXIS_DN;
  else 
    adr = ID_CRS_AXIS_UP;
  rc = DYB_setParameterSync( adr, axis, value, &returned );
  
  return rc;
}


DYB_Rc ASC500_controlFeedback( Int32 * state, Int32 set )
{
  DYB_Rc rc = DYB_Ok;
  if (set){
    Int32 returned = 0;
    Int32 value = *state;
    rc = DYB_setParameterSync( ID_REG_LOOP_ON, 0, value, &returned );
    if ( rc == DYB_Ok && returned != value ) {
      // Not accepted for some reason. It is useless to continue.
      rc = DYB_Error;
    }
  }
  else{
    Int32 value = 0;
    rc = DYB_getParameterSync( ID_REG_LOOP_ON, 0, &value );
    *state = value;
  }

  return rc;
}



DYB_Rc ASC500_controlFeedbackPI( float * p, float * i, Int32 set )
{
  DYB_Rc rc = DYB_Ok;
  Int32 returnedI = 0, returnedP = 0;
  if (set){
    Int32 pInt = (Int32)((*p) * 1000000.);
    Int32 iInt = (Int32)((*i) * 1000.);
    rc = DYB_setParameterSync( ID_REG_KI_DISP, 0, iInt, &returnedI );
    if ( rc == DYB_Ok && returnedI != iInt) {
      // Not accepted for some reason. It is useless to continue.
      rc = DYB_Error;
    }
    rc = DYB_setParameterSync( ID_REG_KP_DISP, 0, pInt, &returnedP );
    if ( rc == DYB_Ok && returnedP != pInt) {
      // Not accepted for some reason. It is useless to continue.
      rc = DYB_Error;
    }
  }
  else{
    rc = DYB_getParameterSync( ID_REG_KI_DISP, 0, &returnedI );
    if (DYB_Ok == rc)
      rc = DYB_getParameterSync( ID_REG_KP_DISP, 0, &returnedP );
    if (DYB_Ok == rc){
      *p = (float)(returnedP / 1000000.);
      *i = (float)(returnedI / 1000.);
    }
  }

  return rc;
}



