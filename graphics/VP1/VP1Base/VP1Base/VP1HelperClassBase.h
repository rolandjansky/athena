/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1HelperClassBase                  //
//                                                            //
//  Description: Class that VP1 helper classes can derive     //
//               from in order to get proper message() and    //
//               messagedebug() protected methods (for        //
//               consistent output to messagebox/stdout, and  //
//               for making copy/assignment forbidden. Also   //
//               provides system pointer if applicable        //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1HELPERCLASSBASE_H
#define VP1HELPERCLASSBASE_H

#include "VP1Base/VP1String.h"
class IVP1System;

class VP1HelperClassBase :  public VP1String {
public:
  virtual ~VP1HelperClassBase();

  static bool verbose() { return s_vp1verbose; } // Returns true if env var VP1_VERBOSE_OUTPUT=1

  void setSystemBasePointer(IVP1System*sys);

  //Both of the following will show up in stdout. The former will also show up in the VP1 gui if m_system!=0.
  void message(const QString&) const;
  void messageDebug(const QString&) const;
  //If verbose() is true, this outputs to stdout:
  void messageVerbose(const QString&) const;

  //Overloaded convenience methods, for outputting several lines:
  void message(const QStringList&, const QString& addtoend = "" ) const;
  void messageDebug(const QStringList&, const QString& addtoend = "") const;
  void messageVerbose(const QStringList&, const QString& addtoend = "") const;
  void message(const QString& addtostart, const QStringList&, const QString& addtoend = "" ) const;
  void messageDebug(const QString& addtostart, const QStringList&, const QString& addtoend = "") const;
  void messageVerbose(const QString& addtostart, const QStringList&, const QString& addtoend = "") const;

  IVP1System * systemBase() const { return m_system; }
  QString helperClassName() const { return m_helpername; }

  //Called by the framework at shutdown - cheap mem. leak check (in verbose mode):
  static void warnUndeletedInstances();

protected:

  VP1HelperClassBase(IVP1System * sys = 0, QString helpername = "");
  void setHelperClassName(const QString& n) { m_helpername = n; }

private:

  // It is illegal to copy/assign a VP1HelperClassBase:
  VP1HelperClassBase( const VP1HelperClassBase & );
  VP1HelperClassBase & operator= (const VP1HelperClassBase & );

  QString m_helpername;
  IVP1System * m_system;
  static bool s_vp1verbose;
};

#endif
