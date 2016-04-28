/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1RawDataColl_LUCID              //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2009                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1RawDataSystems/VP1RawDataColl_LUCID.h"
#include "VP1RawDataSystems/VP1RawDataHandle_LUCID_Digit.h"
#include "VP1RawDataSystems/VP1RawDataCommonData.h"
#include "VP1RawDataSystems/VP1RawDataSysController.h"
#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Base/VP1MaterialButton.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "LUCID_RawEvent/LUCID_DigitContainer.h"

//#include "LUCID_RawEvent/LUCID_RawEvent/LUCID_Digit.h" // old CMT
#include "LUCID_RawEvent/LUCID_Digit.h" // new CMake


//____________________________________________________________________
QStringList VP1RawDataColl_LUCID::availableCollections(IVP1System*sys)
{
  return VP1SGContentsHelper(sys).getKeys<LUCID_DigitContainer>();
}

//____________________________________________________________________
class VP1RawDataColl_LUCID::Imp {
public:
  unsigned minQDC;
};


//____________________________________________________________________
VP1RawDataColl_LUCID::VP1RawDataColl_LUCID(VP1RawDataCommonData*common,const QString& key)
  : VP1RawDataCollBase(common,key), d(new Imp)
{
  d->minQDC = 9999999;
  connect(common->controller(),SIGNAL(lucidMinQDCChanged(unsigned)),
	  this,SLOT(setMinQDC(unsigned)));
  setMinQDC(common->controller()->lucidMinQDC());
}

//____________________________________________________________________
VP1RawDataColl_LUCID::~VP1RawDataColl_LUCID()
{
  delete d;
}

//____________________________________________________________________
void VP1RawDataColl_LUCID::assignDefaultMaterial(SoMaterial*m) const
{
  VP1MaterialButton::setMaterialParameters( m, 0.3, 1.0, 0.95, 0.1 );
}

//____________________________________________________________________
bool VP1RawDataColl_LUCID::load()
{
  if (!VP1JobConfigInfo::hasLUCIDGeometry()) {
    message("LUCID geometry not configured in job");
    return false;
  }
  const LUCID_DigitContainer* digitContainer;
  if (!VP1SGAccessHelper(systemBase()).retrieve(digitContainer,text())) {
    message("Problems retrieving container with key "+text());
    return false;
  }

  LUCID_DigitContainer::const_iterator itDigit(digitContainer->begin()), itDigitE(digitContainer->end());
  for (;itDigit!=itDigitE;++itDigit) {
    if (*itDigit)
      addHandle(new VP1RawDataHandle_LUCID_Digit(this,*itDigit));
  }
  return true;
}

//____________________________________________________________________
bool VP1RawDataColl_LUCID::cut(VP1RawDataHandleBase*handle)
{
  return static_cast<VP1RawDataHandle_LUCID*>(handle)->getQDC() >= d->minQDC;
}

//____________________________________________________________________
void VP1RawDataColl_LUCID::setMinQDC(unsigned i)
{
  if (d->minQDC==i)
    return;
  bool relaxed(i<d->minQDC);
  d->minQDC=i;
  if (relaxed)
    recheckCutStatusOfAllNotVisibleHandles();
  else
    recheckCutStatusOfAllVisibleHandles();
}
