/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1MissingEtTruthHandle           //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1MissingEtSystems/VP1MissingEtTruthHandle.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Base/VP1MaterialButton.h"

#include "MissingETEvent/MissingEtTruth.h"
#include <QStringList>
#include <QColor>

//____________________________________________________________________
QList<VP1StdCollection*> VP1MissingEtTruthHandle::createCollections(IVP1System*sys)
{
  QList<VP1StdCollection*> l;
  foreach (QString key, VP1SGContentsHelper(sys).getKeys<MissingEtTruth>()) {
    VP1MissingEtTruthHandle * col = new VP1MissingEtTruthHandle(sys,key);
    col->init();
    l << col;
  }
  return l;
}

//____________________________________________________________________
class VP1MissingEtTruthHandle::Imp {
public:
  const MissingET* metTruth;
};


//____________________________________________________________________
VP1MissingEtTruthHandle::VP1MissingEtTruthHandle(IVP1System*sys,const QString& key)
  : VP1MissingEtHandle(sys,key), d(new Imp)
{
}

//____________________________________________________________________
VP1MissingEtTruthHandle::~VP1MissingEtTruthHandle()
{
  delete d;
}

//____________________________________________________________________
QStringList VP1MissingEtTruthHandle::clicked(SoPath*pickedPath) const
{
  //Fixme: add MissingEtTruth specific info.
  truncateToCollSep(pickedPath);
  return baseInfo(d->metTruth);
}

//____________________________________________________________________
QColor VP1MissingEtTruthHandle::baseCol() const
{
  return QColor::fromRgbF(38./255,49./255,1.0);
}

//____________________________________________________________________
bool VP1MissingEtTruthHandle::load()
{
  if (!VP1SGAccessHelper(systemBase()).retrieve(d->metTruth,key()))
    return false;

  addArrowToCollSep(d->metTruth);
  return true;
}
