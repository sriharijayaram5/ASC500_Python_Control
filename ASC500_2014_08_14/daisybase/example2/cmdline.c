/*******************************************************************************
 *
 *  Project:        Daisy Client Library
 *
 *  Filename:       cmdline.c
 *
 *  Purpose:        Daisybase commandline interface
 *
 *  Author:         NHands GmbH & Co KG
 *
 *******************************************************************************/
/* $Id: cmdline.c,v 1.6 2010/11/22 19:27:05 trurl Exp $ */

#include <stdio.h>
#include <string.h>
#include "daisybase.h"

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

/* Product specific port number, should be found in <product>.h */
#define PORT_NO      7000

/* Number of data channels we want to observe */
#define CHANNEL_COUNT 4

/* Data callback. Show activity but ignore contents. */
static void dataCallback( Int32 channel, Int32 length, Int32 index,
                          const Int32 * data, const DYB_Meta * meta ) 
{
  printf( "%1d", channel );
}


/* Event callback. */
static void catchallCallback( DYB_Address addr, Int32 index, Int32 value )
{
  printf( "Received: ( %4x, %2d ) = %d\n", addr, index, value );
}

/* Print out return codes */
static const char * printRc( DYB_Rc rc )
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

/* Print some helptext */
static DYB_Rc printHelp()
{
  printf( "    Commands (all parameters in hex!):\n"
          "    exit, help, init, run, stop, reset,\n"
          "    data [0|1], events [<address>|0|-1],\n"
          "    get <address> <index>,\n"
          "    set <address> <index> <value>\n" );
  return DYB_Ok;
}

/* Execute a single command line */
static int execute( int args, const char * cmd, int adr, int idx, int val )
{
  Int32 i, ead, rc = -9999;
  DYB_DataCallback  dcb;
  DYB_EventCallback ecb;

  switch ( args ) {
  case 1:
    if      ( !strcmp( cmd, "quit"  ) ) return 1;
    else if ( !strcmp( cmd, "exit"  ) ) return 1;
    else if ( !strcmp( cmd, "help"  ) ) rc = printHelp();
    else if ( !strcmp( cmd, "init"  ) ) rc = DYB_init( 0, BIN_PATH, 0, PORT_NO );
    else if ( !strcmp( cmd, "run"   ) ) rc = DYB_run();
    else if ( !strcmp( cmd, "stop"  ) ) rc = DYB_stop();
    else if ( !strcmp( cmd, "reset" ) ) rc = DYB_reset();
    break;
  case 2:
    if      ( !strcmp( cmd, "events") ) {
      ecb = adr ? catchallCallback : 0;
      ead = adr ? adr          : -1;
      rc = DYB_setEventCallback( ead, ecb );
    }
    else if ( !strcmp( cmd, "data"  ) ) {
      dcb = adr ? dataCallback : 0;
      /* Monitor all data channels with the same function. */
      for ( i = 0; i < CHANNEL_COUNT; ++i ) {
        rc = DYB_setDataCallback( i, dcb );
      }
    }
    break;    
  case 3:
    if      ( !strcmp( cmd, "get"   ) ) {
      rc = DYB_getParameterAsync( adr, idx );
      DYB_setEventCallback( adr, catchallCallback );
    }
    break;
  case 4:
    if      ( !strcmp( cmd, "set"   ) ) {
      rc = DYB_setParameterAsync( adr, idx, val );
      DYB_setEventCallback( adr, catchallCallback );
    }
    break;
  default:;
  }

  if ( rc == -9999 ) {
    printf( "    Command not recognized" );
  }
  else {
    printf( "    %s", printRc( rc ) );
  }

  return 0;
}


/* An extremely simple command line parser. */
int main( int argc, char ** argv )
{
  /* Command input loop */
  int finish = 0;
  while ( !finish ) {
    char cmd[128], line[128];
    int  adr, idx, val, cmdCnt;

    printf( "\n>>> " );
    gets( line );

    /* Shortcut to switch off noisy callback functions */
    if ( !strcmp( line, " " ) ) {
      execute( 2, "data",   0, 0, 0 );
      execute( 2, "events", 0, 0, 0 );
    }
    else {
      /* The generic way */
      cmdCnt = sscanf( line, "%s %x %x %x", cmd, &adr, &idx, &val );
      if ( cmdCnt > 0 ) {
        finish = execute( cmdCnt, cmd, adr, idx, val );
      }
    }
  }

  return 0;
}
