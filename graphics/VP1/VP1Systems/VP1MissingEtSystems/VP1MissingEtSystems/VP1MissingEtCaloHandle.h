/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1MissingEtCaloHandle              //
//                                                            //
//  Description: Specialisation for MissingEtCalo objects     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1MISSINGETCALOHANDLE_H
#define VP1MISSINGETCALOHANDLE_H

#include "VP1MissingEtSystems/VP1MissingEtHandle.h"

class VP1MissingEtCaloHandle : public VP1MissingEtHandle {
public:

  static QList<VP1StdCollection*> createCollections(IVP1System*sys);

  VP1MissingEtCaloHandle(IVP1System*sys,const QString& key);
  virtual ~VP1MissingEtCaloHandle();

  virtual QStringList clicked(SoPath*) const;
protected:
  virtual QString provideSection() const { return "Calo Et Miss"; }
  virtual QString provideSectionToolTip() const { return "MissingEtCalo objects"; }
  virtual QString checkBoxToolTip() const { return "MissingEtCalo object with key "+key(); }
  virtual bool load();
  virtual qint32 provideCollTypeID() const { return 2; }
  virtual QColor baseCol() const;

private:

  class Imp;
  Imp * d;

};

#endif
