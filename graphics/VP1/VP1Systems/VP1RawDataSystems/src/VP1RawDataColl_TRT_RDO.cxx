/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1RawDataColl_TRT_RDO            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2009                             //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1RawDataSystems/VP1RawDataCommonData.h"
#include "VP1RawDataSystems/VP1RawDataSysController.h"
#include "VP1RawDataSystems/VP1RawDataColl_TRT_RDO.h"
#include "VP1RawDataSystems/VP1RawDataHandle_TRT_RDO.h"
#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Base/VP1MaterialButton.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetRawData/TRT_LoLumRawData.h"

//____________________________________________________________________
QStringList VP1RawDataColl_TRT_RDO::availableCollections(IVP1System*sys)
{
  return VP1SGContentsHelper(sys).getKeys<TRT_RDO_Container>();
}

//____________________________________________________________________
class VP1RawDataColl_TRT_RDO::Imp {
public:
  VP1Interval allowedToT;
  bool requireHT;
  bool useSpecialTRTHTMaterial;
};


//____________________________________________________________________
VP1RawDataColl_TRT_RDO::VP1RawDataColl_TRT_RDO(VP1RawDataCommonData*common,const QString& key)
  : VP1RawDataCollBase(common,key), d(new Imp)
{
  d->requireHT = true;
  d->useSpecialTRTHTMaterial = true;
  d->allowedToT = VP1Interval();

  connect(common->controller(),SIGNAL(trtAllowedToTChanged(const VP1Interval&)),
	  this,SLOT(setAllowedToT(const VP1Interval&)));
  setAllowedToT(common->controller()->trtAllowedToT());

  connect(common->controller(),SIGNAL(trtRequireHTChanged(bool)),
	  this,SLOT(setRequireHT(bool)));
  setRequireHT(common->controller()->trtRequireHT());

  connect(common->controller(),SIGNAL(useSpecialTRTHTMaterialChanged(bool)),
	  this,SLOT(setUseSpecialTRTHTMaterial(bool)));
  setUseSpecialTRTHTMaterial(common->controller()->useSpecialTRTHTMaterial());
}

//____________________________________________________________________
VP1RawDataColl_TRT_RDO::~VP1RawDataColl_TRT_RDO()
{
  delete d;
}

//____________________________________________________________________
void VP1RawDataColl_TRT_RDO::assignDefaultMaterial(SoMaterial*m) const
{
  VP1MaterialButton::setMaterialParameters( m, 0.85, 0.85, 0.85, 0.1 );
}

//____________________________________________________________________
bool VP1RawDataColl_TRT_RDO::load()
{
  if (!VP1JobConfigInfo::hasTRTGeometry()) {
    message("TRT geometry not configured in job");
    return false;
  }
  const TRT_RDO_Container* rdoContainer;
  if (!VP1SGAccessHelper(systemBase()).retrieve(rdoContainer,text())) {
    message("Problems retrieving container with key "+text());
    return false;
  }
  TRT_RDO_Container::const_iterator itColl(rdoContainer->begin()), itCollE(rdoContainer->end());
  for (;itColl!=itCollE;++itColl) {
    if (!*itColl||(*itColl)->empty())
      continue;
    TRT_RDO_Collection::const_iterator itRDO((*itColl)->begin()), itRDOE((*itColl)->end());
    for (;itRDO!=itRDOE;++itRDO) {
      const TRT_LoLumRawData * rdo = dynamic_cast<const TRT_LoLumRawData *>(*itRDO);
      if (rdo)
	addHandle(new VP1RawDataHandle_TRT_RDO(this,rdo));
    }
  }

//   recheckCutStatusOfAllHandles();
  return true;
}

//____________________________________________________________________
bool VP1RawDataColl_TRT_RDO::cut(VP1RawDataHandleBase* handle)
{

  if (d->requireHT && !static_cast<VP1RawDataHandle_TRT_RDO*>(handle)->highThreshold())
    return false;
  return d->allowedToT.contains(static_cast<VP1RawDataHandle_TRT_RDO*>(handle)->timeOverThreshold());
}

//____________________________________________________________________
void VP1RawDataColl_TRT_RDO::setAllowedToT(const VP1Interval& i)
{
  if (d->allowedToT==i)
    return;
  bool relaxed(i.contains(d->allowedToT));
  bool tightened(d->allowedToT.contains(i));
  d->allowedToT=i;
  if (relaxed)
    recheckCutStatusOfAllNotVisibleHandles();
  else if (tightened)
    recheckCutStatusOfAllVisibleHandles();
  else
    recheckCutStatusOfAllHandles();
}

//____________________________________________________________________
void VP1RawDataColl_TRT_RDO::setRequireHT(bool b)
{
  if (d->requireHT==b)
    return;
  d->requireHT=b;
  if (b)
    recheckCutStatusOfAllVisibleHandles();
  else
    recheckCutStatusOfAllNotVisibleHandles();
}

//____________________________________________________________________
void VP1RawDataColl_TRT_RDO::setUseSpecialTRTHTMaterial(bool b)
{
  if (d->useSpecialTRTHTMaterial==b)
    return;
  d->useSpecialTRTHTMaterial=b;

  std::vector<VP1RawDataHandleBase*>::iterator it(getHandles().begin()),itE(getHandles().end());
  for (;it!=itE;++it) {
    if (static_cast<VP1RawDataHandle_TRT_RDO*>(*it)->highThreshold())
      (*it)->update3DObjects();
  }

}

//____________________________________________________________________
bool VP1RawDataColl_TRT_RDO::useSpecialHTMat()
{
  return d->useSpecialTRTHTMaterial;
}
