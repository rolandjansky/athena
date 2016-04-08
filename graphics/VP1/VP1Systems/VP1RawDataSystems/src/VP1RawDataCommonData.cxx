/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1RawDataCommonData              //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2009                             //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1RawDataSystems/VP1RawDataCommonData.h"
#include "VP1RawDataSystems/VP1RawDataSysController.h"
#include "VP1RawDataSystems/VP1RawDataHandleBase.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1Base/IVP13DSystem.h"
#include <map>
#include "VP1HEPVis/nodes/SoGenericBox.h"
#include <Inventor/nodes/SoGroup.h>

//____________________________________________________________________
class VP1RawDataCommonData::Imp {
public:
  std::map<SoTransform*,VP1RawDataHandleBase*> transform2handle;
  static void setShowVolumeOutlines(SoGroup*nodegroup,bool showvol);
  bool showVolOutlines;
};



//____________________________________________________________________
VP1RawDataCommonData::VP1RawDataCommonData(IVP13DSystem * sys,VP1RawDataSysController* controller)
  : VP1HelperClassBase(sys,"VP1RawDataCommonData"),
    m_controller(controller), m_system(sys),
    m_nodeManager(new HitsSoNodeManager(sys)), d(new Imp)
{
  SoGenericBox::initClass();
  d->showVolOutlines = controller->showVolumeOutLines();
  connect(controller,SIGNAL(showVolumeOutLinesChanged(bool)),
	  this,SLOT(setShowVolumeOutLines(bool)));
}

//____________________________________________________________________
VP1RawDataCommonData::~VP1RawDataCommonData()
{
  delete m_nodeManager;
  delete d;
}

//____________________________________________________________________
void VP1RawDataCommonData::registerTransformAndHandle(SoTransform*t,VP1RawDataHandleBase*h)
{
  if (t&&h) {
    d->transform2handle[t]=h;
  }
}

//____________________________________________________________________
void VP1RawDataCommonData::clearEventData()
{
  d->transform2handle.clear();
}

//____________________________________________________________________
VP1RawDataHandleBase* VP1RawDataCommonData::getHandle(SoTransform*t)
{
  if (!t)
    return 0;
  std::map<SoTransform*,VP1RawDataHandleBase*>::iterator it = d->transform2handle.find(t);
  return it==d->transform2handle.end() ? 0 : it->second;
}

//_____________________________________________________________________________________
void VP1RawDataCommonData::Imp::setShowVolumeOutlines(SoGroup*nodegroup,bool showvol)
{
  for (int i = 0; i<nodegroup->getNumChildren();++i) {
    SoNode * n = nodegroup->getChild(i);
    if (n->getTypeId().isDerivedFrom(SoGenericBox::getClassTypeId())) {
      if (static_cast<SoGenericBox*>(n)->drawEdgeLines.getValue()!=showvol)
	static_cast<SoGenericBox*>(n)->drawEdgeLines.setValue(showvol);
    } else if (n->getTypeId().isDerivedFrom(SoGroup::getClassTypeId())) {
      setShowVolumeOutlines(static_cast<SoGroup*>(n),showvol);
    }
  }
}

//_____________________________________________________________________________________
void VP1RawDataCommonData::updateVolumeOutlines(SoGroup * g)
{
  Imp::setShowVolumeOutlines(g,d->showVolOutlines);
}

//_____________________________________________________________________________________
void VP1RawDataCommonData::setShowVolumeOutLines(bool b)
{
  if (d->showVolOutlines==b)
    return;
  d->showVolOutlines = b;
  std::map<SoTransform*,VP1RawDataHandleBase*>::iterator it, itE = d->transform2handle.end();
  for (it=d->transform2handle.begin();it!=itE;++it)
    it->second->updateShownOutlines();
}

