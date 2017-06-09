/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1MissingEtCaloHandle            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Updated: Giorgi Gvaberidze (ggvaberi@cern.ch),            //
//  Updated: July 2010                                        //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1MissingEtSystems/VP1MissingEtCaloHandle.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Utils/VP1SGAccessHelper.h"

#include "MissingETEvent/MissingEtCalo.h"
#include <QStringList>
#include <QColor>

#include "CLHEP/Units/SystemOfUnits.h"

//____________________________________________________________________
QList<VP1StdCollection*> VP1MissingEtCaloHandle::createCollections(IVP1System*sys)
{
  //updated: for prevent loading zero length system
  static const MissingET* met = 0;

  QList<VP1StdCollection*> l;
  foreach (QString key, VP1SGContentsHelper(sys).getKeys<MissingEtCalo>()) {
    //updated: for prevent loading zero length system
    met = 0;
    if(!VP1SGAccessHelper(sys).retrieve(met, key))
	 continue;
    double length = met->et() * (200.0*CLHEP::cm/(100.0*CLHEP::GeV));
    if(MISSINGET_ZEROLENGTH(length))
     continue;
    //------------------------------------------------

    VP1MissingEtCaloHandle * col = new VP1MissingEtCaloHandle(sys,key);
    col->init();
    l << col;
  }
  return l;
}

//____________________________________________________________________
class VP1MissingEtCaloHandle::Imp {
public:
  const MissingET* metCalo;
};


//____________________________________________________________________
VP1MissingEtCaloHandle::VP1MissingEtCaloHandle(IVP1System*sys,const QString& key)
  : VP1MissingEtHandle(sys,key), d(new Imp)
{
}

//____________________________________________________________________
VP1MissingEtCaloHandle::~VP1MissingEtCaloHandle()
{
  delete d;
}

//____________________________________________________________________
QStringList VP1MissingEtCaloHandle::clicked(SoPath*pickedPath) const
{
  //Fixme: add MissingEtCalo specific info.
  truncateToCollSep(pickedPath);
  return baseInfo(d->metCalo);
}


//____________________________________________________________________
QColor VP1MissingEtCaloHandle::baseCol() const
{
  return QColor::fromRgbF(147./255,170./255,30./255);
}

//____________________________________________________________________
bool VP1MissingEtCaloHandle::load()
{
  if (!VP1SGAccessHelper(systemBase()).retrieve(d->metCalo,key()))
    return false;

  addArrowToCollSep(d->metCalo);
  return true;
}
