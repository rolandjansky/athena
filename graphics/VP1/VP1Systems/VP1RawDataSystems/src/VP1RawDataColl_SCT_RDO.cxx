/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1RawDataColl_SCT_RDO            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2009                             //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1RawDataSystems/VP1RawDataColl_SCT_RDO.h"
#include "VP1RawDataSystems/VP1RawDataHandle_SCT_RDO.h"
#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Base/VP1MaterialButton.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "InDetRawData/SCT_RDO_Container.h"


//____________________________________________________________________
QStringList VP1RawDataColl_SCT_RDO::availableCollections(IVP1System*sys)
{
  return VP1SGContentsHelper(sys).getKeys<SCT_RDO_Container>();
}

//____________________________________________________________________
class VP1RawDataColl_SCT_RDO::Imp {
public:
};


//____________________________________________________________________
VP1RawDataColl_SCT_RDO::VP1RawDataColl_SCT_RDO(VP1RawDataCommonData*common,const QString& key)
  : VP1RawDataCollBase(common,key), d(new Imp)
{
}

//____________________________________________________________________
VP1RawDataColl_SCT_RDO::~VP1RawDataColl_SCT_RDO()
{
  delete d;
}

//____________________________________________________________________
void VP1RawDataColl_SCT_RDO::assignDefaultMaterial(SoMaterial*m) const
{
  VP1MaterialButton::setMaterialParameters( m, 1.0, 1.0, 0.0, 0.1 );
}

//____________________________________________________________________
bool VP1RawDataColl_SCT_RDO::load()
{
  if (!VP1JobConfigInfo::hasSCTGeometry()) {
    message("SCT geometry not configured in job");
    return false;
  }
  const SCT_RDO_Container* rdoContainer;
  if (!VP1SGAccessHelper(systemBase()).retrieve(rdoContainer,text())) {
    message("Problems retrieving container with key "+text());
    return false;
  }
  SCT_RDO_Container::const_iterator itColl(rdoContainer->begin()), itCollE(rdoContainer->end());
  for (;itColl!=itCollE;++itColl) {
    if (!*itColl||(*itColl)->empty())
      continue;
    SCT_RDO_Collection::const_iterator itRDO((*itColl)->begin()), itRDOE((*itColl)->end());
    for (;itRDO!=itRDOE;++itRDO)
      addHandle(new VP1RawDataHandle_SCT_RDO(this,*itRDO));
  }

//   recheckCutStatusOfAllHandles();
  return true;
}

//____________________________________________________________________
bool VP1RawDataColl_SCT_RDO::cut(VP1RawDataHandleBase*)
{
  return true;
}
