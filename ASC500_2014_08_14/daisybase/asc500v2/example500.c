/*******************************************************************************
 *
 *  Project:        Daisy Client Library
 *
 *  Filename:       example500.c
 *
 *  Purpose:        Example for using daisybase lib with ASC500
 *
 *  Author:         NHands GmbH & Co KG
 *
 *******************************************************************************/
/* $Id: example500.c,v 1.7 2010/11/22 19:27:05 trurl Exp $ */
 
#include <stdio.h>
#include <assert.h>
#include "daisybase.h"
#include "asc500.h"
#include "asc500helpers.h"

/*
 *  Adapt the path and filenames to your installation
 */
#ifdef unix
#define BIN_PATH     "../.."              /* Daisy installation directory */
#define PROFILE_FILE "../../afm.ngp"      /* Profile for base settings    */
#include <unistd.h>                       /* unistd.h for usleep          */
#define SLEEP(x) usleep(x*1000)

#else

#define BIN_PATH     "..\\.."             /* Daisy installation directory */
#define PROFILE_FILE "..\\..\\afm.ngp"    /* Profile for base settings    */
#include <windows.h>                      /* windows.h for Sleep          */
#define SLEEP(x) Sleep(x)
#endif


/*
 *  Some arbitrary parameter values used here
 */
#define CHANNELNO                0        /* Channel for data transfer    */
#define COLUMNS                100        /* Scanrange number of columns  */
#define LINES                  150        /* Scanrange number of lines    */
#define PIXELSIZE  ((Flt32) 1.e-9)        /* Width of a column / line     */
#define SAMPLETIME ((Flt32) 8.e-5)        /* Scanner sample time          */


/* Static data for communication callback - main */
static int    _frameStarted = 0,   _frameIsFull = 0;
static FILE * _fileFwd      = 0;


/* simple helper function */
static const char * printUnit( DYB_Unit unit )
{
  switch ( unit ) {
  case DYB_UnitMm:  return "mm";
  case DYB_UnitUm:  return "um";
  case DYB_UnitNm:  return "nm";
  case DYB_UnitPm:  return "pm";
  case DYB_UnitV:   return "V";
  case DYB_UnitMv:  return "mV";
  case DYB_UnitUv:  return "uV";
  case DYB_UnitNv:  return "nV";
  case DYB_UnitMhz: return "MHz";
  case DYB_UnitKhz: return "kHz";
  case DYB_UnitHz:  return "Hz";
  case DYB_UnitIhz: return "mHz";
  case DYB_UnitS:   return "s";
  case DYB_UnitMs:  return "ms";
  case DYB_UnitUs:  return "us";
  case DYB_UnitNs:  return "ns";
  case DYB_UnitA:   return "A";
  case DYB_UnitMa:  return "mA";
  case DYB_UnitUa:  return "uA";
  case DYB_UnitNa:  return "nA";
  case DYB_UnitDeg: return "deg";
  case DYB_UnitCos: return "[cos]";
  case DYB_UnitDB:  return "dB";
  case DYB_UnitW:   return "W";
  case DYB_UnitMw:  return "mW";
  case DYB_UnitUw:  return "uW";
  case DYB_UnitNw:  return "nW";
  default:          return "?";
  }
}

/* Write ASCII header of BCRF file format */
static void writeBcrfHeader( const DYB_Meta * meta, FILE * file )
{
  int   textLen = 0;
  int   xPixel, yPixel;
  Flt32 xRange, yRange, xOrigin, yOrigin;
  DYB_getPointsX( meta, &xPixel );
  DYB_getPointsY( meta, &yPixel );
  DYB_getPhysRangeX( meta, &xRange );
  DYB_getPhysRangeY( meta, &yRange );
  DYB_convIndex2Phys2( meta, 0, &xOrigin, &yOrigin );

  /* Not sure about the meaning of xoffset, yoffset in bcrf.
   * Daisy compatible: origin is the center of the scan range */
  xOrigin += xRange / 2;
  yOrigin += yRange / 2;

  textLen += fprintf( file, "fileformat = bcrf\n" );
  textLen += fprintf( file, "starttime 02 08 08 18:52:50\n" );  /* Dummy only!! */
  textLen += fprintf( file, "xpixels = %d\n", xPixel );
  textLen += fprintf( file, "ypixels = %d\n", yPixel );
  textLen += fprintf( file, "xlength = %f\n", xRange );
  textLen += fprintf( file, "ylength = %f\n", yRange );
  textLen += fprintf( file, "xoffset = %f\n", xOrigin );
  textLen += fprintf( file, "yoffset = %f\n", yOrigin );
  textLen += fprintf( file, "xunit = %s\n",   printUnit( DYB_getUnitXY( meta )  ) );
  textLen += fprintf( file, "yunit = %s\n",   printUnit( DYB_getUnitXY( meta )  ) );
  textLen += fprintf( file, "zunit = %s\n",   printUnit( DYB_getUnitVal( meta ) ) );
  textLen += fprintf( file, "scanspeed = 0\n" );                /* Dummy only!! */
  textLen += fprintf( file, "voidpixels = 0\n" );
  textLen += fprintf( file, "intelmode = 1\n" );
  /* Header must be 2048 bytes long */
  while ( textLen < 2047 )
    textLen += fprintf( file, " " );
  fprintf( file, "\n" );
}


/*
 * Data callback function
 * Collects data of a complete frame and writes results of the forward
 * scan into forward.bcrf.
 */
static void dataCallback( Int32 channel, Int32 length, Int32 index,
                          const Int32 * data, const DYB_Meta * meta ) 
{
  Int32 lines, cols, i;
  Flt32 z;
  /* Counter to check frame for completeness                              */
  static unsigned int _sampleCnt = 0;

  /* We expect only scan data of channel 0 with adjusted parameters.
     Checks are good for crash safety but should not be necessary.        */
  assert( channel                        == CHANNELNO  &&
          DYB_getOrder( meta )           == DYB_FbScan &&
          DYB_getPointsX( meta, &cols  ) == DYB_MetaOk &&
          DYB_getPointsY( meta, &lines ) == DYB_MetaOk &&
          cols                           == COLUMNS    &&
          lines                          == LINES         );

  /* Examine every single data point. That is certainly not optimized for
   * performance - but simple                                             */
  for ( i = 0; i < length; ++i ) {
    Int32 x, y;
    Bln32 forwd, upwd;
    assert( DYB_convIndex2Pixel(     meta, index + i, &x,     &y    ) == DYB_MetaOk );
    assert( DYB_convIndex2Direction( meta, index + i, &forwd, &upwd ) == DYB_MetaOk );

    if ( !_frameStarted ) {
    /* We can't be sure that the data stream begins with at the origin
     * of a frame. Find the start point to save a full frame.
     * We are looking for upward / forward frames only because others
     * would require sorting of the data.                                 */
      if ( x == 0 && y == 0 && forwd && upwd ) {
        printf( "Frame starts now.\n" );
        _frameStarted = 1;
        _sampleCnt    = 1;  /* We already have one */
        /* Must open files binary to match header size exactly            */
        assert( (_fileFwd = fopen( "forward.bcrf",  "wb" )) );
        writeBcrfHeader( meta, _fileFwd );
        z = DYB_convValue2Phys( meta, data[i] );
        fwrite( &z, sizeof( z ), 1, _fileFwd );
      }
    }
    else if ( !_frameIsFull ) {
      /* Frame has been started, now look for its end.                    */
      if ( forwd ) {
        z = DYB_convValue2Phys( meta, data[i] );
        fwrite( &z, sizeof( z ), 1, _fileFwd );
        _sampleCnt++;
        if ( _sampleCnt % 100 == 0 ) {
          printf( "." );
          fflush( stdout );
        }
      }
      if ( x == 0 && y == LINES - 1 && !forwd && upwd ) {
        printf( "\nFull frame with %d data.\n", _sampleCnt );
        _frameIsFull = 1;
      }
    }
  }
}


/*
 * The application loads the afm profile, configures the scanner and one data channel.
 * It starts the scanner and runs it until the first full frame is received, then stops it.
 * Data are received and stored in a file.
 */
int main( int argc, char ** argv )
{
  DYB_Rc rc;
  Int32  run, active, cols, lines, maxseconds;
  Flt32  offsetX, offsetY, pixSize, smpTime;

  /* Initialize & start */
  rc = DYB_init( 0, BIN_PATH, 0, ASC500_PORT_NUMBER );
  printf( "DYB_Init                       returned \"%s\"\n", ASC500_printRc( rc ) );

  rc = DYB_run();
  printf( "DYB_run                        returned \"%s\"\n", ASC500_printRc( rc ) );

  /* Configure the scanner by sending a profile. */
  rc = DYB_sendProfile( PROFILE_FILE );
  printf( "DYB_sendProfile                returned \"%s\"\n", ASC500_printRc( rc ) );

  /* Configure data channel 0 */
  rc = ASC500_configureChannel( 0,                    /* Channel 0           */
                                CHANCONN_SCANNER,     /* Trigger by scanner  */ 
                                CHANADC_ADC_MIN + 1,  /* Source is ADC 2     */
                                0,                    /* Don't average       */
                                0 );                  /* Sample time ignored */
  printf( "ASC500_configureChannel        returned \"%s\"\n", ASC500_printRc( rc ) );

  /* Register data callback for channel 0 */
  DYB_setDataCallback( 0, dataCallback );

  /* Switch off annoying automatics that are useful only for GUI users */
  rc = DYB_setParameterAsync( ID_SCAN_X_EQ_Y,  0, 0 );
  printf( "DYB_setParameterAsync          returned \"%s\"\n", ASC500_printRc( rc ) );
  rc = DYB_setParameterAsync( ID_SCAN_GEOMODE, 0, 0 );
  printf( "DYB_setParameterAsync          returned \"%s\"\n", ASC500_printRc( rc ) );

  /* Adjust scanner parameters */
  offsetX = offsetY = 150 * PIXELSIZE;
  pixSize = PIXELSIZE;
  cols    = COLUMNS;
  lines   = LINES;
  smpTime = SAMPLETIME;
  rc = ASC500_controlScanPixels( &cols, &lines, &pixSize, 1 );
  printf( "ASC500_controlScanPixels       returned \"%s\"\n", ASC500_printRc( rc ) );
  rc = ASC500_controlScanOffset( &offsetX, &offsetY, 1 );
  printf( "ASC500_controlScanOffset       returned \"%s\"\n", ASC500_printRc( rc ) );
  rc = ASC500_controlSampleTime( &smpTime, 1 );
  printf( "ASC500_controlSampleTime       returned \"%s\"; Sample time: %f\n",
          ASC500_printRc( rc ), smpTime );

  /* Enable and start the scanner. */
  active = 1;
  run    = SCANRUN_ON;
  rc = ASC500_controlOutputActivation( &active, 1 );
  printf( "ASC500_controlOutputActivation returned \"%s\"\n", ASC500_printRc( rc ) );

  rc = ASC500_controlOutputActivation( &active, 0 );
  printf( "ASC500_controlOutputActivation returned \"%s\", active=%d\n",
	  ASC500_printRc( rc ), active );
  /* Must wait here for reliable results. Don't really understand why. */
  SLEEP( 1000 );
  rc = ASC500_controlScannerState( &run, 1 );
  printf( "ASC500_controlScannerState     returned \"%s\"\n", ASC500_printRc( rc ) );

  /* Recieve until frame is full or timeout */
  maxseconds = 30;
  while ( !_frameIsFull && maxseconds > 0 ) {
    maxseconds--;
    SLEEP( 1000 );
  }

  if ( !_frameIsFull ) {
    printf( "\n!!TIMEOUT!!.  Aborted before frame was complete.\n" );
  }

  /* Stop it and exit */
  active = 0;
  run    = SCANRUN_OFF;
  rc = ASC500_controlScannerState( &run, 1 );
  printf( "ASC500_controlScannerState     returned \"%s\"\n", ASC500_printRc( rc ) );
  rc = ASC500_controlOutputActivation( &active, 1 );
  printf( "ASC500_controlOutputActivation returned \"%s\"\n", ASC500_printRc( rc ) );

  DYB_stop();
  return 0;
}
