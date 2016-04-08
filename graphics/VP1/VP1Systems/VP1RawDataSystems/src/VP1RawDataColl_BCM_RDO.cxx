/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1RawDataColl_BCM_RDO            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2009                             //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1RawDataSystems/VP1RawDataColl_BCM_RDO.h"
#include "VP1RawDataSystems/VP1RawDataHandle_BCM_RDO.h"
#include "VP1RawDataSystems/VP1RawDataCommonData.h"
#include "VP1RawDataSystems/VP1RawDataSysController.h"
#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Base/VP1MaterialButton.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "InDetBCM_RawData/BCM_RDO_Container.h"


//____________________________________________________________________
QStringList VP1RawDataColl_BCM_RDO::availableCollections(IVP1System*sys)
{
  return VP1SGContentsHelper(sys).getKeys<BCM_RDO_Container>();
}

//____________________________________________________________________
class VP1RawDataColl_BCM_RDO::Imp {
public:
  bool useSpecialBCMHighAttMaterial;
};


//____________________________________________________________________
VP1RawDataColl_BCM_RDO::VP1RawDataColl_BCM_RDO(VP1RawDataCommonData*common,const QString& key)
  : VP1RawDataCollBase(common,key), d(new Imp)
{
  d->useSpecialBCMHighAttMaterial = true;

  connect(common->controller(),SIGNAL(useSpecialBCMHighAttMaterialChanged(bool)),
	  this,SLOT(setUseSpecialBCMHighAttMaterial(bool)));
  setUseSpecialBCMHighAttMaterial(common->controller()->useSpecialBCMHighAttMaterial());
}

//____________________________________________________________________
VP1RawDataColl_BCM_RDO::~VP1RawDataColl_BCM_RDO()
{
  delete d;
}

//____________________________________________________________________
void VP1RawDataColl_BCM_RDO::assignDefaultMaterial(SoMaterial*m) const
{
  VP1MaterialButton::setMaterialParameters( m, 0.3, 1.0, 0.95, 0.1 );
}

//____________________________________________________________________
bool VP1RawDataColl_BCM_RDO::load()
{
  if (!VP1JobConfigInfo::hasBCMGeometry()) {
    message("BCM geometry not configured in job");
    return false;
  }
  const BCM_RDO_Container* rdoContainer;
  if (!VP1SGAccessHelper(systemBase()).retrieve(rdoContainer,text())) {
    message("Problems retrieving container with key "+text());
    return false;
  }

  //We collect multiple BCM_RawData objects in one VP1RawDataHandle.

  std::map<int,QList<const BCM_RawData*> > module2data;
  std::map<int,QList<const BCM_RawData*> >::iterator it;

  BCM_RDO_Container::const_iterator itColl(rdoContainer->begin()), itCollE(rdoContainer->end());
  for (;itColl!=itCollE;++itColl) {
    if (!*itColl||(*itColl)->empty())
      continue;
    BCM_RDO_Collection::const_iterator itRDO((*itColl)->begin()), itRDOE((*itColl)->end());
    for (;itRDO!=itRDOE;++itRDO) {
      //Ignore null-hits (since there doesn't seem to be zero-suppression):
      if (!(*itRDO)->getPulse1Position()&&!(*itRDO)->getPulse2Position()
	  &&!(*itRDO)->getPulse1Width()&&!(*itRDO)->getPulse2Width())
	continue;

      int modID = moduleID((*itRDO)->getChannel());
      it = module2data.find(modID);
      if (it==module2data.end()) {
	QList<const BCM_RawData*> l;
	l << *itRDO;
	module2data[modID] = l;
      } else {
	it->second << *itRDO;
      }

    }
  }

  for (it=module2data.begin();it!=module2data.end();++it)
    addHandle(new VP1RawDataHandle_BCM_RDO(this,it->first,it->second));

  return true;
}

//____________________________________________________________________
bool VP1RawDataColl_BCM_RDO::cut(VP1RawDataHandleBase*)
{
  return true;
}

//____________________________________________________________________
bool VP1RawDataColl_BCM_RDO::useSpecialBCMHighAttMaterial() const
{
  return d->useSpecialBCMHighAttMaterial;
}

//____________________________________________________________________
void VP1RawDataColl_BCM_RDO::setUseSpecialBCMHighAttMaterial(bool b)
{
  if (d->useSpecialBCMHighAttMaterial==b)
    return;
  d->useSpecialBCMHighAttMaterial=b;

  std::vector<VP1RawDataHandleBase*>::iterator it(getHandles().begin()),itE(getHandles().end());
  for (;it!=itE;++it) {
    if (static_cast<VP1RawDataHandle_BCM_RDO*>(*it)->numberOfHighAttenuationHits()>0)
      (*it)->update3DObjects();
  }
}
