#*******************************************************************************
#*
#*  Project:        Daisy Client Library
#*
#*  Filename:       makefile.win
#*
#*  Purpose:        MSVC makefile for daisybase ASC500 example
#*
#*  Author:         NHands GmbH & Co KG
#*
#********************************************************************************
# $Id: makefile.win,v 1.1 2008/07/31 16:25:20 trurl Exp $

example500.exe : example500.obj asc500helpers.obj makefile.win ..\lib\daisybase.lib
	link /NOLOGO /DEBUG /SUBSYSTEM:CONSOLE /OUT:example500.exe example500.obj asc500helpers.obj ..\lib\daisybase.lib

asc500helpers.obj : asc500helpers.c asc500.h asc500helpers.h makefile.win ..\include\daisybase.h ..\include\metadata.h
	cl -nologo -c -W3 -MDd -Zi -I..\include -o asc500helpers.obj asc500helpers.c

example500.obj : example500.c asc500.h asc500helpers.h makefile.win ..\include\daisybase.h ..\include\metadata.h
	cl -nologo -c -W3 -MDd -Zi -I..\include -o example500.obj example500.c

