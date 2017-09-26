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
  : IVP13DSystem(name,information,contact_info), m_d(new Imp())
{
  m_d->theclass = this;
  m_d->controllerBuilt = false;
  m_d->first = true;
  //Root node is selection node for default selections causing invocations of userPickedNode:
  SoCooperativeSelection::ensureInitClass();
  SoCooperativeSelection * selection = new SoCooperativeSelection;
  selection->policy = SoSelection::SINGLE;
  registerSelectionNode(selection);
  setUserSelectionNotificationsEnabled(selection,false);
  selection->addSelectionCallback( Imp::made_selection, this );

  m_d->root = selection;
  m_d->rootR = new SoSeparator();
  m_d->rootE = new SoSeparator();
  m_d->root->addChild(m_d->rootR);
  m_d->root->addChild(m_d->rootE);
  m_d->root->ref();
  m_d->rootR->ref();
  m_d->rootE->ref();
  m_d->root->setName(QString(name+"_SceneGraph").replace(' ','_').toStdString().c_str());
  m_d->rootE->setName(QString(name+"_EventSceneGraph").replace(' ','_').toStdString().c_str());
  m_d->rootR->setName(QString(name+"_PermanentSceneGraph").replace(' ','_').toStdString().c_str());
  m_d->wasrefreshed=false;
  m_d->wascreated=false;
}

//___________________________________________________________
IVP13DSystemSimple::~IVP13DSystemSimple() {
  unregisterSelectionNode(m_d->root);
  m_d->root->unref();
  m_d->rootR->unref();
  m_d->rootE->unref();
  delete m_d;
  m_d=0;
}

//___________________________________________________________________________________________________________
void IVP13DSystemSimple::ensureBuildController()
{
  if (m_d->controllerBuilt)
    return;
  m_d->controllerBuilt=true;

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
  return static_cast<SoSeparator*>(m_d->root);
}

//___________________________________________________________
void IVP13DSystemSimple::create(StoreGateSvc* /*detstore*/)
{
  messageVerbose("IVP13DSystemSimple create");
  assert(!m_d->wasrefreshed);
  assert(!m_d->wascreated);
  ensureBuildController();//TODO: Move to refresh.
  m_d->wascreated=true;
  m_d->wasrefreshed=false;
}

//___________________________________________________________
void IVP13DSystemSimple::refresh(StoreGateSvc* sg)
{
  assert(m_d->wascreated);
  assert(!m_d->wasrefreshed);

  if (m_d->first) {
    messageVerbose("IVP13DSystemSimple first refresh - so calling create methods (i.e. delayed create).");
    systemcreate(detectorStore());
    m_d->first = false;
    m_d->root->removeChild(m_d->rootR);
    buildPermanentSceneGraph(detectorStore(),m_d->rootR);
    m_d->root->addChild(m_d->rootR);
  }

  m_d->root->removeChild(m_d->rootE);
  updateGUI();
  buildEventSceneGraph(sg, m_d->rootE);
  updateGUI();
  m_d->root->addChild(m_d->rootE);

  m_d->wasrefreshed=true;

}

//___________________________________________________________
void IVP13DSystemSimple::erase()
{
  messageVerbose("IVP13DSystemSimple::erase() start");
  assert(m_d->wascreated);
  assert(m_d->wasrefreshed);

  bool saveE = m_d->rootE->enableNotify(false);

  systemerase();
  messageVerbose("IVP13DSystemSimple::erase() Removing all event objects from scene");
  if (verbose())
    warnOnDisabledNotifications();
  m_d->rootE->removeAllChildren();

  if (saveE) {
    m_d->rootE->enableNotify(true);
    m_d->rootE->touch();
  }

  m_d->wasrefreshed=false;
  messageVerbose("IVP13DSystemSimple::erase() end");
}

//___________________________________________________________
void IVP13DSystemSimple::uncreate()
{
	messageDebug("uncreate()...");

  assert(m_d->wascreated);
  assert(!m_d->wasrefreshed);
  m_d->rootE->enableNotify(false);
  m_d->rootR->enableNotify(false);
  systemuncreate();
  m_d->root->removeAllChildren();
  m_d->rootE->removeAllChildren();
  m_d->rootR->removeAllChildren();
  m_d->wascreated=false;
}

//___________________________________________________________
void IVP13DSystemSimple::warnOnDisabledNotifications() const
{
  QList<SoNode*> nodesR;
  QList<SoNode*> nodesE;
  m_d->getNodesWithDisabledNotifications(m_d->rootR, nodesR);
  m_d->getNodesWithDisabledNotifications(m_d->rootE, nodesE);
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
