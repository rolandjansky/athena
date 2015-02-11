/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1Msg                            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Base/VP1Msg.h"
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1QtUtils.h"
#include <iostream>

bool VP1Msg::s_verbose = VP1QtUtils::environmentVariableIsOn("VP1_VERBOSE_OUTPUT");

//____________________________________________________________________
void VP1Msg::message( const QString& str, IVP1System*sys )
{
  if (sys)
    sys->message(str);
  else
    std::cout << prefix_msg() << ": "<< str.toStdString()<<std::endl;
}

//____________________________________________________________________
void VP1Msg::messageDebug( const QString& str )
{
  std::cout << prefix_debug() << ": "<< str.toStdString()<<std::endl;
}

//____________________________________________________________________
void VP1Msg::messageWarning( const QString& str )
{
  std::cout << prefix_warning() << ": "<< str.toStdString()<<std::endl;
}

// only the 'Warning' label is printed in red
//____________________________________________________________________
void VP1Msg::messageWarningRed( const QString& str )
{
	// colors, see:
	// - http://brianmilco.blogspot.ch/2011/11/color-debug-output-with-qt-and-qdebug.html
	// - http://misc.flogisoft.com/bash/tip_colors_and_formatting
	std::string msg = str.toStdString();
	fprintf(stderr, "\033[1m\033[31mWarning:\033[21m\033[0m: %s\n", msg.c_str() );
}

// The whole 'Warning' message is printed in red
//____________________________________________________________________
void VP1Msg::messageWarningAllRed( const QString& str )
{
	// colors, see:
	// - http://brianmilco.blogspot.ch/2011/11/color-debug-output-with-qt-and-qdebug.html
	// - http://misc.flogisoft.com/bash/tip_colors_and_formatting
	std::string msg = str.toStdString();
	fprintf(stderr, "\033[1m\033[31mWarning:\033[21m %s\033[0m\n", msg.c_str() );
}


//____________________________________________________________________
void VP1Msg::messageVerbose( const QString& str )
{
  if (!verbose())
    return;
  std::cout<<prefix_verbose()<<": "<<str.toStdString()<<std::endl;
}

//____________________________________________________________________
void VP1Msg::message(const QStringList& l, const QString& addtoend, IVP1System*sys )
{
  if (addtoend.isEmpty()) {
    foreach(QString s, l)
      message(s,sys);
  } else {
    foreach(QString s, l)
      message(s+addtoend,sys);
  }
}

//____________________________________________________________________
void VP1Msg::messageDebug(const QStringList& l, const QString& addtoend )
{
  if (addtoend.isEmpty()) {
    foreach(QString s, l)
      messageDebug(s);
  } else {
    foreach(QString s, l)
      messageDebug(s+addtoend);
  }
}

//____________________________________________________________________
void VP1Msg::messageVerbose(const QStringList& l, const QString& addtoend )
{
  if (!verbose())
    return;
  if (addtoend.isEmpty()) {
    foreach(QString s, l)
      messageVerbose(s);
  } else {
    foreach(QString s, l)
      messageVerbose(s+addtoend);
  }
}

//____________________________________________________________________
void VP1Msg::message(const QString& addtostart, const QStringList& l,
		     const QString& addtoend, IVP1System*sys )
{
  if (addtostart.isEmpty()) {
    message(l,addtoend,sys);
    return;
  }
  if (addtoend.isEmpty()) {
    foreach(QString s, l)
      message(addtostart+s,sys);
  } else {
    foreach(QString s, l)
      message(addtostart+s+addtoend,sys);
  }
}

//____________________________________________________________________
void VP1Msg::messageDebug(const QString& addtostart, const QStringList& l, const QString& addtoend )
{
  if (addtostart.isEmpty()) {
    messageDebug(l,addtoend);
    return;
  }
  if (addtoend.isEmpty()) {
    foreach(QString s, l)
      messageDebug(addtostart+s);
  } else {
    foreach(QString s, l)
      messageDebug(addtostart+s+addtoend);
  }
}

//____________________________________________________________________
void VP1Msg::messageVerbose(const QString& addtostart, const QStringList& l, const QString& addtoend )
{
  if (!verbose())
    return;
  if (addtostart.isEmpty()) {
    messageVerbose(l,addtoend);
    return;
  }
  if (addtoend.isEmpty()) {
    foreach(QString s, l)
      messageVerbose(addtostart+s);
  } else {
    foreach(QString s, l)
      messageVerbose(addtostart+s+addtoend);
  }
}

