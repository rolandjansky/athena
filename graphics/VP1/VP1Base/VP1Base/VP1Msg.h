/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1Msg                              //
//                                                            //
//  Description: Static methods for consistent message output //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1MSG_H
#define VP1MSG_H

#include <QStringList>
#include "VP1Base/VP1String.h"


/* utility function to declare that a variable or a function parameter is unused by design
 * and not that somebody has forgotten about it.
 * Used also for assert() statements; otherwise compiler throw warnings while building
 * in "opt" (release, because the assert() are removed and sometimes the variable used
 * as assert() parameters appear to not be used at all, to the compiler.
 */
#pragma once
#define _UNUSED(x) ((void)x)



class IVP1System;
class VP1Msg : public VP1String {
public:
  static bool verbose() { return s_verbose; } // Returns true if env var VP1_VERBOSE_OUTPUT=1

  static void message(const QString&, IVP1System*sys = 0);//Non-zero sys pointer to get message in GUI
  static void messageDebug(const QString&);
  static void messageWarning(const QString&);
  static void messageVerbose(const QString&);//This outputs only if verbose() is true.
  static void messageWarningRed( const QString& str );
  static void messageWarningAllRed( const QString& str );

  //Overloaded convenience methods, for outputting several lines:
  static void message(const QStringList&, const QString& addtoend = "", IVP1System*sys = 0 );
  static void messageDebug(const QStringList&, const QString& addtoend = "" );
  static void messageVerbose(const QStringList&, const QString& addtoend = "" );
  static void message(const QString& addtostart, const QStringList&, const QString& addtoend = "", IVP1System*sys = 0 );
  static void messageDebug(const QString& addtostart, const QStringList&, const QString& addtoend = "" );
  static void messageVerbose(const QString& addtostart, const QStringList&, const QString& addtoend = "" );

  static const char* prefix_msg() { return "VP1MESSAGE";}
  static const char* prefix_debug() { return "VP1MSG_DEBUG";}
  static const char* prefix_warning() { return "VP1MSG_WARNING";}
  static const char* prefix_verbose() { return "VP1MSG_VERBOSE";}

private:
  VP1Msg(){}
  ~VP1Msg(){}
  static bool s_verbose;
};

#endif
