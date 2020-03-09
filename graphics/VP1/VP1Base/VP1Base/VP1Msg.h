/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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


#pragma once



class IVP1System;
class VP1Msg : public VP1String {
public:
  static bool verbose() { return m_verbose; } // Returns true if env var VP1_VERBOSE_OUTPUT=1
  static bool debug() { return m_debug; } // Returns true if env var VP1_DEBUG_OUTPUT=1

  #if defined BUILDVP1LIGHT
    static void enableMsg(const QString&, const QString&);  
  #else
    static void enableMsg(const QString&);
  #endif

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
  static bool m_verbose;
  static bool m_debug;
};

#endif
