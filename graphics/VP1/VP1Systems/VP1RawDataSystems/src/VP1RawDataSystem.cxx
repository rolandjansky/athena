/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1RawDataSystem                  //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2008 (rewritten January 2009)       //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1RawDataSystems/VP1RawDataSystem.h"
#include "VP1RawDataSystems/VP1RawDataSysController.h"
#include "VP1RawDataSystems/VP1RawDataColl_PixelRDO.h"
#include "VP1RawDataSystems/VP1RawDataColl_SCT_RDO.h"
#include "VP1RawDataSystems/VP1RawDataColl_TRT_RDO.h"
#include "VP1RawDataSystems/VP1RawDataColl_BCM_RDO.h"
#include "VP1RawDataSystems/VP1RawDataColl_LUCID.h"
#include "VP1RawDataSystems/VP1RawDataCommonData.h"
#include "VP1RawDataSystems/VP1RawDataHandleBase.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1CollectionWidget.h"
#include "VP1Base/VP1CameraHelper.h"
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/SoPath.h>

//____________________________________________________________________
class VP1RawDataSystem::Imp {
public:
  Imp(VP1RawDataSystem*tc) : theclass(tc), controller(0), common(0) {}
  VP1RawDataSystem* theclass;
  VP1RawDataSysController * controller;
  VP1RawDataCommonData * common;

  template <class T>
  QList<VP1RawDataCollBase*> createSpecificCollections() {
    QList<VP1RawDataCollBase*> l;
    foreach (QString name, T::availableCollections(theclass)) {
      ensureInitCommonData();
      T * col = new T(common,name);
      col->init();
      l << col;
    }
    return l;
  }
  QList<VP1RawDataCollBase*> createCollections() {
    QList<VP1RawDataCollBase*> l;
    l << createSpecificCollections<VP1RawDataColl_PixelRDO>();
    l << createSpecificCollections<VP1RawDataColl_SCT_RDO>();
    l << createSpecificCollections<VP1RawDataColl_TRT_RDO>();
    l << createSpecificCollections<VP1RawDataColl_BCM_RDO>();
    l << createSpecificCollections<VP1RawDataColl_LUCID>();
    return l;
  }

  void ensureInitCommonData() {
    if (!common) {
      theclass->ensureBuildController();
      common = new VP1RawDataCommonData(theclass,controller);
    }
  }

};

//_____________________________________________________________________________________
VP1RawDataSystem::VP1RawDataSystem()
  : IVP13DSystemSimple("RawHits",
		       "System showing Raw Data (RDO's, BCM hits, ...)",
		       "Thomas.Kittelmann@cern.ch"),
    d(new Imp(this))
{
}

//____________________________________________________________________
VP1RawDataSystem::~VP1RawDataSystem()
 {
  delete d;
}

//_____________________________________________________________________________________
void VP1RawDataSystem::systemerase()
{
  //Save present states and then clear all event data and related gui elements.
  if (d->controller)
    d->controller->collWidget()->clear();

  if (d->common)
    d->common->clearEventData();
}

//_____________________________________________________________________________________
void VP1RawDataSystem::systemuncreate()
{
  delete d->common;
  d->common = 0;
}

//_____________________________________________________________________________________
void VP1RawDataSystem::buildEventSceneGraph(StoreGateSvc*, SoSeparator *root)
{
  root->addChild(d->controller->drawOptions());

  //Create collection list based on contents of event store, populate
  //gui and apply states:
  d->controller->collWidget()->setCollections(d->createCollections());

  //Add collections to event scenegraph:
  foreach (VP1StdCollection* col,d->controller->collWidget()->collections<VP1StdCollection>())
    root->addChild(col->collSwitch());
}

//_____________________________________________________________________________________
QWidget * VP1RawDataSystem::buildController()
{
  d->controller = new VP1RawDataSysController(this);
  return d->controller;
}

//_____________________________________________________________________________________
void VP1RawDataSystem::userPickedNode(SoNode*, SoPath * pickedPath) {
  messageVerbose("userPickedNode");

  //Look for separator whose first child is known transform (also pop a few times).
  d->ensureInitCommonData();
  VP1RawDataHandleBase* handle(0);
  int i(1);
  for (; i < pickedPath->getLength(); ++i) {
    if (pickedPath->getNodeFromTail(i)->getTypeId()==SoSeparator::getClassTypeId()) {
      SoSeparator * sep = static_cast<SoSeparator*>(pickedPath->getNodeFromTail(i));
      if (sep->getNumChildren()>0&&sep->getChild(0)->getTypeId()==SoTransform::getClassTypeId()) {
	SoTransform * transform = static_cast<SoTransform*>(sep->getChild(0));
	handle = d->common->getHandle(transform);
	if (handle)
	  break;
      }
    }
  }
  for (int j=0;j<i;++j)
    pickedPath->pop();

  if (!handle) {
    message("WARNING: Unknown node clicked.");
    return;
  }

  if (d->controller->printInfoOnClick()) {
    message(handle->clicked(d->controller->printVerboseInfoOnClick()));
  }
  if (d->controller->zoomOnClick()) {
    std::set<SoCamera*> cameras(getCameraList());
    std::set<SoCamera*>::iterator it,itE = cameras.end();
    for (it=cameras.begin();it!=itE;++it)
      VP1CameraHelper::animatedZoomToPath(*it,handle->coll()->collSep(),pickedPath,2.0,1.0);
  }

}

//_____________________________________________________________________________________
QByteArray VP1RawDataSystem::saveState()
{
  VP1Serialise serialise(0/*version*/,this);
  ensureBuildController();

  serialise.save(IVP13DSystemSimple::saveState());

  serialise.save(d->controller->collWidget());
  serialise.save(d->controller->saveSettings());

  serialise.disableUnsavedChecks();//We do the testing in the controller

  return serialise.result();
}

//_____________________________________________________________________________________
void VP1RawDataSystem::restoreFromState(QByteArray ba)
{
  VP1Deserialise state(ba,this);
  if (state.version()<0||state.version()>0) {
    message("Warning: State data in .vp1 file is in wrong format - ignoring!");
    return;
  }
  ensureBuildController();
  IVP13DSystemSimple::restoreFromState(state.restoreByteArray());

  state.restore(d->controller->collWidget());
  d->controller->restoreSettings(state.restoreByteArray());

  state.disableUnrestoredChecks();//We do the testing in the controller
}
