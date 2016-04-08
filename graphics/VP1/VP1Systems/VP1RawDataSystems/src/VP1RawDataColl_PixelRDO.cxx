/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1RawDataColl_PixelRDO           //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2009                             //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1RawDataSystems/VP1RawDataColl_PixelRDO.h"
#include "VP1RawDataSystems/VP1RawDataHandle_PixelRDO.h"
#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Base/VP1MaterialButton.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "InDetRawData/PixelRDO_Container.h"


//____________________________________________________________________
QStringList VP1RawDataColl_PixelRDO::availableCollections(IVP1System*sys)
{
  return VP1SGContentsHelper(sys).getKeys<PixelRDO_Container>();
}

//____________________________________________________________________
class VP1RawDataColl_PixelRDO::Imp {
public:
};


//____________________________________________________________________
VP1RawDataColl_PixelRDO::VP1RawDataColl_PixelRDO(VP1RawDataCommonData*common,const QString& key)
  : VP1RawDataCollBase(common,key), d(new Imp)
{
}

//____________________________________________________________________
VP1RawDataColl_PixelRDO::~VP1RawDataColl_PixelRDO()
{
  delete d;
}

//____________________________________________________________________
void VP1RawDataColl_PixelRDO::assignDefaultMaterial(SoMaterial*m) const
{
  VP1MaterialButton::setMaterialParameters( m, 0.0, 1.0, 0.0, 0.1 );
}

//____________________________________________________________________
bool VP1RawDataColl_PixelRDO::load()
{
  if (!VP1JobConfigInfo::hasPixelGeometry()) {
    message("Pixel geometry not configured in job");
    return false;
  }
  const PixelRDO_Container* rdoContainer;
  if (!VP1SGAccessHelper(systemBase()).retrieve(rdoContainer,text())) {
    message("Problems retrieving container with key "+text());
    return false;
  }
  PixelRDO_Container::const_iterator itColl(rdoContainer->begin()), itCollE(rdoContainer->end());
  for (;itColl!=itCollE;++itColl) {
    if (!*itColl||(*itColl)->empty())
      continue;
    PixelRDO_Collection::const_iterator itRDO((*itColl)->begin()), itRDOE((*itColl)->end());
    for (;itRDO!=itRDOE;++itRDO)
      addHandle(new VP1RawDataHandle_PixelRDO(this,*itRDO));
  }

//   recheckCutStatusOfAllHandles();
  return true;
}

//____________________________________________________________________
bool VP1RawDataColl_PixelRDO::cut(VP1RawDataHandleBase*)
{
  return true;
}
