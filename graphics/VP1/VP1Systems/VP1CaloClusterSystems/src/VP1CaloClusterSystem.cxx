/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1CaloClusterSystem              //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1CaloClusterSystems/VP1CaloClusterSystem.h"
#include "VP1CaloClusterSystems/CaloClusterSysController.h"
#include "VP1CaloClusterSystems/VP1CaloClusterCollection.h"

#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1CollectionWidget.h"
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/SoPath.h>

//____________________________________________________________________
class VP1CaloClusterSystem::Imp {
public:
  Imp(VP1CaloClusterSystem*tc) : theclass(tc), controller(0) {}
  VP1CaloClusterSystem* theclass;
  CaloClusterSysController * controller;
};

//_____________________________________________________________________________________
VP1CaloClusterSystem::VP1CaloClusterSystem()
  : IVP13DSystemSimple("Clusters",
		       "System showing Calorimeter Clusters",
		       "Thomas.Kittelmann@cern.ch"),
    d(new Imp(this))
{
}

//____________________________________________________________________
VP1CaloClusterSystem::~VP1CaloClusterSystem()
 {
  delete d;
}

//_____________________________________________________________________________________
void VP1CaloClusterSystem::systemerase()
{
  //Save present states and then clear all event data and related gui elements.
  d->controller->collWidget()->clear();
}

//_____________________________________________________________________________________
void VP1CaloClusterSystem::buildEventSceneGraph(StoreGateSvc*, SoSeparator *root)
{
  root->addChild(d->controller->drawOptions());

  //Create collection list based on contents of event store, populate
  //gui and apply states:
  d->controller->collWidget()->setCollections(VP1CaloClusterCollection::createCollections(this,d->controller));

  //Add collections to event scenegraph:
  foreach (VP1StdCollection* col,d->controller->collWidget()->collections<VP1StdCollection>())
    root->addChild(col->collSwitch());
}

//_____________________________________________________________________________________
QWidget * VP1CaloClusterSystem::buildController()
{
  d->controller = new CaloClusterSysController(this);
  return d->controller;
}

//_____________________________________________________________________________________
void VP1CaloClusterSystem::userPickedNode(SoNode*, SoPath * pickedPath) {

  foreach (VP1CaloClusterCollection* col,d->controller->collWidget()->collections<VP1CaloClusterCollection>()) {
    if (col->visible()&&pickedPath->containsNode(col->collSep())) {
      message(col->infoOnClicked(pickedPath));
      return;
    }
  }
  message("Error: Does not have cluster information for picked node");
}

//_____________________________________________________________________________________
QByteArray VP1CaloClusterSystem::saveState()
{
  VP1Serialise serialise(1/*version*/,this);
  ensureBuildController();

  serialise.save(IVP13DSystemSimple::saveState());

  serialise.save(d->controller->collWidget());
  serialise.save(d->controller->saveSettings());//1+

  serialise.disableUnsavedChecks();//We do the testing in the controller

  return serialise.result();
}

//_____________________________________________________________________________________
void VP1CaloClusterSystem::restoreFromState(QByteArray ba)
{
  VP1Deserialise state(ba,this);
  if (state.version()<0||state.version()>1) {
    message("Warning: State data in .vp1 file is in wrong format - ignoring!");
    return;
  }
  ensureBuildController();
  IVP13DSystemSimple::restoreFromState(state.restoreByteArray());

  state.restore(d->controller->collWidget());
  if (state.version()>=1)
    d->controller->restoreSettings(state.restoreByteArray());

  state.disableUnrestoredChecks();//We do the testing in the controller
}
