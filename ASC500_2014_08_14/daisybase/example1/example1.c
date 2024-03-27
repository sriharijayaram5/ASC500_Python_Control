/*******************************************************************************
 *
 *  Project:        Daisy Client Library
 *
 *  Filename:       example1.c
 *
 *  Purpose:        Trivial example for use of daisybase lib
 *
 *  Author:         NHands GmbH & Co KG
 *
 *******************************************************************************/
/* $Id: example1.c,v 1.8 2010/11/22 16:37:29 trurl Exp $ */

#include <stdio.h>
#include "daisybase.h"
#include "product1.h"

#ifdef unix
#define BIN_PATH     "../.."
#define PROFILE_FILE "../../afm.ngp"
#define I64FMT "%llx"
#include <unistd.h>
#define SLEEP(x) usleep(x*1000)

#else

#define BIN_PATH     "..\\.."
#define PROFILE_FILE "..\\..\\afm.ngp"
/* Non-standard formatstring for MSVC 2003 */
#define I64FMT "%I64x"
/* windows.h for Sleep */
#include <windows.h>
#define SLEEP(x) Sleep(x)
#endif


/* Do I have started the scanner? */
static int _scannerShouldRun = 0;

/* Number of data received */
static unsigned int _sampleCount = 0;

/* Data callback function */
static void dataCallback( Int32 channel, Int32 length, Int32 index,
                          const Int32 * data, const DYB_Meta * meta ) 
{
  /* Translate data to physical units */
  Flt32 x, y;
  if ( DYB_MetaOk == DYB_convIndex2Phys2( meta, index, &x, &y ) ) {
    Flt32 z = DYB_convValue2Phys( meta, data[0] );
    /* produces lots of traffic */
    /* printf( "dataCallback: x=%f, y=%f, z=%f\n", x, y, z ); */
 }
  else
    printf( "dataCallback: convIndex2Phys failed!!\n" );

  /* count received data */
  _sampleCount += length;
}


/* Event callback function for scanner state */
static void runCallback( DYB_Address addr, Int32 index, Int32 value )
{
  if ( value == SCANRUN_ON ) {
    printf( "runCallback: Scanner started\n" );
  }
  else {
    printf( "runCallback: Scanner stopped\n" );

    /* This is a gimmick: restart the scanner if somebody else stopped it */
    if ( _scannerShouldRun ) {
      printf( "                  --> Restarting\n" );
      DYB_setParameterAsync( ID_SCAN_RUNNING, 0, SCANRUN_ON );
    }
  }
}


/* Event callback function for all other parameters */
static void catchallCallback( DYB_Address addr, Int32 index, Int32 value )
{
  printf( "Catchall: ( %4x, %2d ) = %d\n", addr, index, value );
}


/*
 * The application starts the scanner, runs it for 10 seconds, and stops it.
 * It restarts the scanner if it is stopped by another client.
 * Data are received and evaluated.
 * For simplicity, the app doesn't configure the scanner directly but uses
 * an existing profile.
 */
int main( int argc, char ** argv )
{
  Int32 rc, run;

  /* Initialize & start */
  DYB_init( 0, BIN_PATH, 0, PORT_NUMBER );
  rc = DYB_run();
  printf( "DYB_run returned %d\n", rc );
  if ( rc != DYB_Ok ) {
    return rc;
  }

  /* Register scannerstate and data callback for channel 0 */
  DYB_setEventCallback( ID_SCAN_RUNNING, runCallback );
  DYB_setDataCallback(                0, dataCallback );

  /* Configure the scanner by sending a profile. */
  rc = DYB_sendProfile( PROFILE_FILE );
  printf( "DYB_sendProfile returned %d\n", rc );

  /* Register the catchall callback after configuration
   * to avoid too much noise.  */
  DYB_setEventCallback( -1,    catchallCallback );

  /* Enable server tracing for telegrams to me */
  DYB_setParameterAsync( ID_SRV_TRACEFLG, 0, TRACE_CS_OUT );

  /* Enable and start the scanner.
   * Enabling takes place asynchronously, we will have to wait for it */
  DYB_setParameterAsync( ID_OUTPUT_ACTIVATE, 0, 1 );
  do {
    SLEEP( 50 );
    rc = DYB_setParameterSync( ID_SCAN_RUNNING, 0, SCANRUN_ON, &run );
  } while ( rc == DYB_Ok && run != SCANRUN_ON );
  _scannerShouldRun = 1;

  /* Recieve Data for 5 seconds */
  SLEEP( 5000 );

  /* Check if the scanner keeps running */
  rc = DYB_getParameterSync( ID_SCAN_RUNNING, 0, &run );
  printf( "DYB_getConfigSync returned %d, result is %d\n", rc, run );

  /* Recieve Data for 5 more seconds */
  SLEEP( 5000 );

  /* Stop it and exit */
  _scannerShouldRun = 0;
  DYB_setParameterAsync( ID_SCAN_RUNNING,    0, 0 );
  DYB_setParameterAsync( ID_OUTPUT_ACTIVATE, 0, 0 );
  SLEEP( 1000 );
  DYB_stop();
  printf( "Received %u samples. Good bye.\n", _sampleCount );
  return 0;
}
