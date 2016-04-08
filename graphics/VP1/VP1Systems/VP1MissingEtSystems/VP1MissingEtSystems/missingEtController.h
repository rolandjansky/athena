/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class missingEtController                 //
//                                                            //
//  Description: missing Et system controller  .              //
//                                                            //
//  Author: Giorgi Gvaberidze (ggvaberi@cern.ch)              //
//  Initial version: July 2010                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef MISSINGETCONTROLLER_H
#define MISSINGETCONTROLLER_H

#include "VP1Base/VP1Controller.h"

class missingEtController : public VP1Controller {

  Q_OBJECT

public:

  missingEtController(IVP1System* sys);
  virtual ~missingEtController();

  void actualRestoreSettings(VP1Deserialise&);
  int  currentSettingsVersion() const;
  void actualSaveSettings(VP1Serialise&) const;

  VP1CollectionWidget* collWidget() const;
  bool changeShape() const;

 signals:
  void changeShapeChanged(bool);

private slots:
  void setArrow(bool);
  void setDashline(bool);
  void changeThickness();
  void changeScale();

  void possibleChange_changeShape();

private:
  void refreshPivots();

private:
  class Imp;
  Imp* d;
};

#endif
