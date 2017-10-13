/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1VertexSystems/VP1VertexSystem.h"
#include "VP1VertexSystems/VP1VertexCollection.h"
#include "VP1VertexSystems/VertexSysController.h"
#include "VP1VertexSystems/VP1TruthVertexCollection.h"

#include "VP1Base/VP1CollectionWidget.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/SoPath.h>

//____________________________________________________________________
class VP1VertexSystem::Imp {
public:
  VertexSysController * controller;
  std::map<const VP1StdCollection*,QList< std::pair<const SoMaterial*, QList< const Trk::Track*> > > > tracksFromVertices ;
};

//_____________________________________________________________________________________
VP1VertexSystem::VP1VertexSystem()
  : IVP13DSystemSimple ( "Vertex","Vertex system",
			 "Thomas.Kittelmann@cern.ch, Andreas.Wildauer@cern.ch" ),
    m_d(new Imp)
{
  m_d->controller = 0;
}

//____________________________________________________________________
VP1VertexSystem::~VP1VertexSystem()
{
  delete m_d;
}

//_____________________________________________________________________________________
void VP1VertexSystem::systemerase ( )
{
  //Save present states and then clear all event data and related gui elements.
  m_d->controller->collWidget()->clear();
  m_d->tracksFromVertices.clear();
}

//_____________________________________________________________________________________
void VP1VertexSystem::buildEventSceneGraph ( StoreGateSvc*, SoSeparator *root )
{
  root->addChild(m_d->controller->drawOptions());

  QList<VP1StdCollection*> cols;
  //Create collection list based on contents of event store:
  cols << VP1VertexCollection::createCollections(m_d->controller);
  cols << VP1TruthVertexCollection::createCollections(m_d->controller);

  //Populate gui (also applies states):
  m_d->controller->collWidget()->setCollections(cols);

  //Add collections to event scenegraph:
  foreach (VP1StdCollection* col,cols){
    root->addChild(col->collSwitch());
    VP1VertexCollection* vertColl = dynamic_cast<VP1VertexCollection*> (col);
    if (vertColl) vertColl->recheckAllCuts(); //bit of a hack - the idea is to force the emission of the signal about track from vertices
  }
  
//  QList< std::pair<const SoMaterial*, QList< const Trk::Track*> > > tmpList; // fill and emit
//  
//  std::map<const VP1StdCollection*,QList< std::pair<const SoMaterial*, QList< const Trk::Track*> > > >::const_iterator 
//  it=m_d->tracksFromVertices.begin(), itEnd=m_d->tracksFromVertices.end();
//  
//  for ( ;it!=itEnd; it++){
//    tmpList+=it->second;
//  }
//  messageVerbose(" emitting tracksFromVerticesChanged "+QString::number(tmpList.size())+" vertices.");
//  
//  emit tracksFromVertexChanged(tmpList);
  
}

//_____________________________________________________________________________________
QWidget * VP1VertexSystem::buildController()
{
  m_d->controller = new VertexSysController(this);
  return m_d->controller;
}

//_____________________________________________________________________________________
void VP1VertexSystem::userPickedNode(SoNode*, SoPath * pickedPath) {

  //Find in which collection an object was picked:
  VP1StdCollection* pickedCol(0);
  foreach (VP1StdCollection* col,m_d->controller->collWidget()->collections<VP1StdCollection>()) {
    if (col->visible()&&pickedPath->containsNode(col->collSep())) {
      pickedCol = col;
      break;
    }
  }
  if (!pickedCol) {
    message("Error: Could not determine in which collection click took place.");
    return;
  }

  //React to click:
  VP1VertexCollection * recoCol = dynamic_cast<VP1VertexCollection *>(pickedCol);
  if (recoCol) {
    message(recoCol->infoOnClicked(pickedPath));
    return;
  }
  VP1TruthVertexCollection * truthCol = dynamic_cast<VP1TruthVertexCollection *>(pickedCol);
  if (truthCol) {
    message(truthCol->infoOnClicked(pickedPath));
    return;
  }
  message("Error: Unknown vertex collection type.");
}

//_____________________________________________________________________________________
QByteArray VP1VertexSystem::saveState()
{
  ensureBuildController();
  VP1Serialise serialise(1/*version*/,this);
  serialise.save(IVP13DSystemSimple::saveState());
  serialise.save(m_d->controller->saveSettings());//Version 1+
  serialise.save(m_d->controller->collWidget());
  serialise.disableUnsavedChecks();//We do the testing in the controller
  return serialise.result();
}

//_____________________________________________________________________________________
void VP1VertexSystem::restoreFromState(QByteArray ba)
{
  VP1Deserialise state(ba,this);
  if (state.version()<0||state.version()>1) {
    message("Warning: State data in .vp1 file is in wrong format - ignoring!");
    return;
  }
  ensureBuildController();
  IVP13DSystemSimple::restoreFromState(state.restoreByteArray());
  if (state.version()>=1)
    m_d->controller->restoreSettings(state.restoreByteArray());
  state.restore(m_d->controller->collWidget());

  state.disableUnrestoredChecks();//We do the testing in the controller
}

void VP1VertexSystem::updateVertexToTracks(QList< std::pair<const SoMaterial*, QList< const Trk::Track*> > >& newTracksFromVertices)
{
  messageVerbose(" updateVertexToTracks with "+QString::number(newTracksFromVertices.size())+" vertices.");
  const VP1StdCollection* coll = static_cast<const VP1StdCollection*> (QObject::sender());
  if (!coll) return;
  m_d->tracksFromVertices[coll]=newTracksFromVertices;
  QList< std::pair<const SoMaterial*, QList< const Trk::Track*> > > tmpList; // fill and emit
  
  std::map<const VP1StdCollection*,QList< std::pair<const SoMaterial*, QList< const Trk::Track*> > > >::const_iterator 
    it=m_d->tracksFromVertices.begin(), itEnd=m_d->tracksFromVertices.end();
  
  for ( ;it!=itEnd; it++){
    tmpList+=it->second;
  }
  messageVerbose(" emitting tracksFromVerticesChanged "+QString::number(tmpList.size())+" collections.");

  emit tracksFromVertexChanged(tmpList);
}
