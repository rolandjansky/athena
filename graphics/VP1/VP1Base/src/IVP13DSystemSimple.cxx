/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class IVP13DSystemSimple             //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/IVP13DSystemSimple.h"
#include "VP1Base/SoCooperativeSelection.h"
#include "Inventor/SoPath.h"
#include "Inventor/nodes/SoSeparator.h"
#include <iostream>

// These have defaults that dont do anything:
void IVP13DSystemSimple::systemcreate(StoreGateSvc*) {}
QWidget * IVP13DSystemSimple::buildController() { return 0; }
void IVP13DSystemSimple::buildPermanentSceneGraph(StoreGateSvc*,SoSeparator *) {}
void IVP13DSystemSimple::userPickedNode(SoNode*, SoPath *){}
void IVP13DSystemSimple::systemuncreate() {}
void IVP13DSystemSimple::systemerase()
{
  //  messageVerbose("WARNING: Did not reimplement systemerase!");
}

class IVP13DSystemSimple::Imp {
public:
  IVP13DSystemSimple * theclass;
  SoCooperativeSelection *root;
  SoSeparator *rootR; // Present for the whole run.
  SoSeparator *rootE; // Present for one event;
  void getNodesWithDisabledNotifications(SoGroup*, QList<SoNode*>&) const;
  bool wasrefreshed;
  bool wascreated;
  bool controllerBuilt;
  bool first;
  //Due to userPickedNode() this class has its own callback (we disable the ones from the baseclass below by setUserSelectionNotificationsEnabled, to avoid duplication)
  static void made_selection( void * userdata, SoPath * path );

};

//___________________________________________________________
IVP13DSystemSimple::IVP13DSystemSimple(const QString & name, const QString & information, const QString & contact_info)
  : IVP13DSystem(name,information,contact_info), d(new Imp())
{
  d->theclass = this;
  d->controllerBuilt = false;
  d->first = true;
  //Root node is selection node for default selections causing invocations of userPickedNode:
  SoCooperativeSelection::ensureInitClass();
  SoCooperativeSelection * selection = new SoCooperativeSelection;
  selection->policy = SoSelection::SINGLE;
  registerSelectionNode(selection);
  setUserSelectionNotificationsEnabled(selection,false);
  selection->addSelectionCallback( Imp::made_selection, this );

  d->root = selection;
  d->rootR = new SoSeparator();
  d->rootE = new SoSeparator();
  d->root->addChild(d->rootR);
  d->root->addChild(d->rootE);
  d->root->ref();
  d->rootR->ref();
  d->rootE->ref();
  d->root->setName(QString(name+"_SceneGraph").replace(' ','_').toStdString().c_str());
  d->rootE->setName(QString(name+"_EventSceneGraph").replace(' ','_').toStdString().c_str());
  d->rootR->setName(QString(name+"_PermanentSceneGraph").replace(' ','_').toStdString().c_str());
  d->wasrefreshed=false;
  d->wascreated=false;
}

//___________________________________________________________
IVP13DSystemSimple::~IVP13DSystemSimple() {
  unregisterSelectionNode(d->root);
  d->root->unref();
  d->rootR->unref();
  d->rootE->unref();
  delete d;
  d=0;
}

//___________________________________________________________________________________________________________
void IVP13DSystemSimple::ensureBuildController()
{
  if (d->controllerBuilt)
    return;
  d->controllerBuilt=true;

  messageVerbose("IVP13DSystemSimple build controller");
  QWidget * controller = buildController();
  if (controller)
    registerController(controller);
  messageVerbose("IVP13DSystemSimple controller was = "+str(controller));
}

//___________________________________________________________________________________________________________
void IVP13DSystemSimple::Imp::made_selection( void * userdata, SoPath * path )
{
  IVP13DSystemSimple * system = static_cast<IVP13DSystemSimple *>(userdata);
  if (!system) {
    std::cout<<"IVP13DSystemSimple::Imp::made_selection Error: Could not find system pointer!"<<std::endl;
    return;
  }

  SoFullPath *fPath = static_cast<SoFullPath *>(path);
  if (!fPath)
    return;
  SoNode *selectedNode = fPath->getTail();
  if (!selectedNode)
    return;

//  std::cout << "calling system->userPickedNode()..." << std::endl;
  system->userPickedNode(selectedNode, path);
//  std::cout << "called system->userPickedNode()." << std::endl;
}

//___________________________________________________________
SoSeparator * IVP13DSystemSimple::getSceneGraph() const
{
  return static_cast<SoSeparator*>(d->root);
}

//___________________________________________________________
void IVP13DSystemSimple::create(StoreGateSvc* /*detstore*/)
{
  messageVerbose("IVP13DSystemSimple create");
  assert(!d->wasrefreshed);
  assert(!d->wascreated);
  ensureBuildController();//TODO: Move to refresh.
  d->wascreated=true;
  d->wasrefreshed=false;
}

//___________________________________________________________
void IVP13DSystemSimple::refresh(StoreGateSvc* sg)
{
  assert(d->wascreated);
  assert(!d->wasrefreshed);

  if (d->first) {
    messageVerbose("IVP13DSystemSimple first refresh - so calling create methods (i.e. delayed create).");
    systemcreate(detectorStore());
    d->first = false;
    d->root->removeChild(d->rootR);
    buildPermanentSceneGraph(detectorStore(),d->rootR);
    d->root->addChild(d->rootR);
  }

  d->root->removeChild(d->rootE);
  updateGUI();
  buildEventSceneGraph(sg, d->rootE);
  updateGUI();
  d->root->addChild(d->rootE);

  d->wasrefreshed=true;

}

//___________________________________________________________
void IVP13DSystemSimple::erase()
{
  messageVerbose("IVP13DSystemSimple::erase() start");
  assert(d->wascreated);
  assert(d->wasrefreshed);

  bool saveE = d->rootE->enableNotify(false);

  systemerase();
  messageVerbose("IVP13DSystemSimple::erase() Removing all event objects from scene");
  if (verbose())
    warnOnDisabledNotifications();
  d->rootE->removeAllChildren();

  if (saveE) {
    d->rootE->enableNotify(true);
    d->rootE->touch();
  }

  d->wasrefreshed=false;
  messageVerbose("IVP13DSystemSimple::erase() end");
}

//___________________________________________________________
void IVP13DSystemSimple::uncreate()
{
	messageDebug("uncreate()...");

  assert(d->wascreated);
  assert(!d->wasrefreshed);
  d->rootE->enableNotify(false);
  d->rootR->enableNotify(false);
  systemuncreate();
  d->root->removeAllChildren();
  d->rootE->removeAllChildren();
  d->rootR->removeAllChildren();
  d->wascreated=false;
}

//___________________________________________________________
void IVP13DSystemSimple::warnOnDisabledNotifications() const
{
  QList<SoNode*> nodesR;
  QList<SoNode*> nodesE;
  d->getNodesWithDisabledNotifications(d->rootR, nodesR);
  d->getNodesWithDisabledNotifications(d->rootE, nodesE);
  if (!nodesR.isEmpty()) {
    message("WARNING: Found "+str(nodesR.count())+" node"+QString(nodesR.count()>1?"s":0)+" with disabled notifications in permanent scenegraph:");
    foreach (SoNode * node, nodesR)
      message("  => Node ("+str(node)+") of type "+QString(node->getTypeId().getName().getString())+", named "+QString(node->getName().getString()));
  }
  if (!nodesE.isEmpty()) {
    message("WARNING: Found "+str(nodesE.count())+" node"+QString(nodesE.count()>1?"s":0)+" with disabled notifications in event scenegraph:");
    foreach (SoNode * node, nodesE)
      message("  => Node ("+str(node)+") of type "+QString(node->getTypeId().getName().getString())+", named "+QString(node->getName().getString()));
  }
}

//___________________________________________________________
void IVP13DSystemSimple::Imp::getNodesWithDisabledNotifications(SoGroup* gr, QList<SoNode*>& l) const
{
  const int n = gr->getNumChildren();
  for (int i = 0; i < n; ++i) {
    SoNode * child = gr->getChild(i);
    if (!child->isNotifyEnabled())
      l << child;
    if (child->getTypeId().isDerivedFrom(SoGroup::getClassTypeId()))
      getNodesWithDisabledNotifications(static_cast<SoGroup*>(child),l);
  }
}
