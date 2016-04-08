/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1MissingEtTruthHandle             //
//                                                            //
//  Description: Specialisation for MissingEtTruth objects    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1MISSINGETTRUTHHANDLE_H
#define VP1MISSINGETTRUTHHANDLE_H

#include "VP1MissingEtSystems/VP1MissingEtHandle.h"

class VP1MissingEtTruthHandle : public VP1MissingEtHandle {
public:

  static QList<VP1StdCollection*> createCollections(IVP1System*sys);

  VP1MissingEtTruthHandle(IVP1System*sys,const QString& key);
  virtual ~VP1MissingEtTruthHandle();

  virtual QStringList clicked(SoPath*) const;
protected:
  virtual QString provideSection() const { return "Truth Et Miss"; }
  virtual QString provideSectionToolTip() const { return "MissingEtTruth objects"; }
  virtual QString checkBoxToolTip() const { return "MissingEtTruth object with key "+key(); }
  virtual bool load();
  virtual qint32 provideCollTypeID() const { return 1; }
  virtual QColor baseCol() const;

private:

  class Imp;
  Imp * d;

};

#endif
