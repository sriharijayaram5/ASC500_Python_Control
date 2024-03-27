/*******************************************************************************
 *
 *  Project:        Daisy Client Library
 *
 *  Filename:       example0.c
 *
 *  Purpose:        Trivial example for use of daisybase lib
 *
 *  Author:         NHands GmbH & Co KG
 *
 *******************************************************************************/
/* $Id: example0.c,v 1.5 2010/11/20 18:22:40 trurl Exp $ */

#include <stdio.h>
#include <math.h>
#include "daisybase.h"
#include "product1.h"

#ifdef unix
#define BIN_PATH     "../.."
#define I64FMT "%llx"
#include <unistd.h>
#define SLEEP(x) usleep(x*1000)

#else

#define BIN_PATH     "..\\.."
/* Non-standard formatstring for MSVC 2003 */
#define I64FMT "%I64x"
/* windows.h for Sleep */
#include <windows.h>
#define SLEEP(x) Sleep(x)
#endif

/* Reduction of the datarate: 400kHz / 10000 -> 40 Hz */
#define RATE_REDUCTION 10000

/* Runtime of the measurement [ms] */
#define MS_RUNTIME      5000

/* Sample counter */
static unsigned int _sampleCount = 0;


/* Print data value with physical unit */
static char * printValue( Flt32 value, DYB_Unit u, char * buffer )
{
  /* The 2 LSB encode a reduction factor */
  int base   = u & ~0x03;
  int reduct = u &  0x03;
  char prefix;
  const char * unit;
  static const char * const prefixes = "TGMk munpf";

  switch ( base ) {
  case DYB_UnitMm:  unit = "m";     reduct += 1; break;  // begins with Milli ...
  case DYB_UnitV:   unit = "V";                  break;
  case DYB_UnitMhz: unit = "Hz";    reduct -= 2; break;  // begins with Mega ...
  case DYB_UnitS:   unit = "s";                  break;
  case DYB_UnitA:   unit = "A";                  break;
  case DYB_UnitCos: unit = "[cos]"; reduct = 99; break;  // No scaling!
  case DYB_UnitDB:  unit = "[dB]";  reduct = 99; break;
  case DYB_UnitDeg: unit = "deg";   reduct = 99; break;
  case DYB_UnitW:   unit = "W";                  break;
  default:          unit = "?";
  }

  /* Now that we have the unit, normalize the value to 1...1000 */
  while ( reduct != 99 && fabs( value ) < 1. ) {
    value *= 1000.;
    reduct++;
  }
  while ( reduct != 99 && fabs( value ) >= 1000. ) {
    value /= 1000.;
    reduct--;
  }

  /* Get prefix from the list if allowed */
  prefix = reduct == 99 ? ' ' : prefixes[reduct+4];

  /* print everything to the buffer */
  sprintf( buffer, "%8.4f %c%s", value, prefix, unit );
  return buffer;
}


/* Data callback function */
static void dataCallback( Int32 channel, Int32 length, Int32 index,
                          const Int32 * data, const DYB_Meta * meta ) 
{
  /* Print out the first sample of every packet. */
  Flt32 t, v;
  char buffer1[128], buffer2[128];

  /* Translate data to physical units */
  if ( DYB_MetaOk == DYB_convIndex2Phys1( meta, index, &t ) ) {
    v = DYB_convValue2Phys( meta, data[0] );
    printf( "   Sample: t=%s, v=%s (%d more...)\n",
            printValue( t, DYB_getUnitXY( meta ),  buffer1 ),
            printValue( v, DYB_getUnitVal( meta ), buffer2 ),
            length - 1 );
  }
  else
    printf( "   dataCallback: convIndex2Phys1 failed!!\n" );

  _sampleCount += length;
}


/*
 * Without any configuration of the system, the example
 * connects data channel 2 with ADC1 and receives continously.
 */
#define CHANNEL_NO 2   /* Data channel 2   */
#define ADC_NO     0   /* ADC1 has index 0 */

int main( int argc, char ** argv )
{
  Int32 rc;

  /* Initialize & start */
  DYB_init( 0, BIN_PATH, 0, PORT_NUMBER );
  rc = DYB_run();
  printf( "DYB_run returned %d\n", rc );
  if ( rc != DYB_Ok ) {
    return rc;
  }

  /* Register data callback for channel 2 */
  DYB_setDataCallback( CHANNEL_NO, dataCallback );
  /* Enable data channels */
  DYB_setParameterAsync( ID_DATA_EN,      0,          1 );
  /* Connect ADC1 to channel 2 */
  DYB_setParameterAsync( ID_CHAN_ADC,     CHANNEL_NO, ADC_NO );
  /* Set the datarate */
  DYB_setParameterAsync( ID_CHAN_POINTS,  CHANNEL_NO, RATE_REDUCTION );
  /* Set triggering by time. */
  DYB_setParameterAsync( ID_CHAN_CONNECT, CHANNEL_NO, CHANCONN_PERMANENT );

  /* Recieve data for some seconds */
  SLEEP( MS_RUNTIME );

  /* Stop it and exit */
  DYB_stop();

  printf( "Received %d samples; expected %d\n", _sampleCount,
          400 * MS_RUNTIME / RATE_REDUCTION );

  return 0;
}
