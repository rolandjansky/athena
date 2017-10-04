/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class IVP13DSystem                   //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/IVP13DSystem.h"
#include "VP1Base/SoCooperativeSelection.h"
#include "VP1Base/VP1Msg.h"

#include <Inventor/nodes/SoCamera.h>
#include <Inventor/Qt/viewers/SoQtViewer.h>

#include <QTimer>

#include <iostream>
#include <map>

//* Always use SoCooperativeSelections
//* A selection node should only appear in the subtree of just one system

//___________________________________________________________________________________________________________
class IVP13DSystem::Imp {
public:
  // List of selection callbacks.
  std::vector<SoSelectionPathCB *> _callbacks;
  //camera lists:
  std::set<SoCamera*> staticcameras;
  std::set<SoQtViewer*> viewers;

  //(De)selection callbacks:
  static void start_changeselection(void *userdata, SoSelection *sel);
  static void finished_changeselection(void *userdata, SoSelection *sel);
  static void made_selection( void * userdata, SoPath * path );
  static void made_deselection( void * userdata, SoPath * path );
  static void clickedoutside(void *userdata, SoCooperativeSelection *sel);

  static std::map<SoCooperativeSelection*,IVP13DSystem*> selection2system;
  //We add the selection pointers as userdata, so selection2sys is
  //used to get the system pointer(s) in the callback functions.

  QSet<SoCooperativeSelection *> selectionsWithDisabledNotifications;

  std::map<SoCooperativeSelection*,SoPathList> selection2lastpathlist;
  bool clickedoutsideScheduled;
};

std::map<SoCooperativeSelection*,IVP13DSystem*> IVP13DSystem::Imp::selection2system;

// Methods invoked upon selection / deselection (to be reimplemented in derived classes).
void IVP13DSystem::userSelectedSingleNode(SoCooperativeSelection*, SoNode* , SoPath*) {}
void IVP13DSystem::userDeselectedSingleNode(SoCooperativeSelection*, SoNode* , SoPath*) {}
void IVP13DSystem::userChangedSelection(SoCooperativeSelection*, QSet<SoNode*>, QSet<SoPath*>) {}
void IVP13DSystem::userClickedOnBgd() {}

//___________________________________________________________________________________________________________
void IVP13DSystem::Imp::clickedoutside(void *userdata, SoCooperativeSelection *selection)
{
  if (!selection) {
    std::cout<<"IVP13DSystem::Imp::clickedoutside Error: Got null selection pointer!"<<std::endl;
    return;
  }
  IVP13DSystem * system = dynamic_cast<IVP13DSystem *>(static_cast<IVP1System * >(userdata));
  if (!system) {
    std::cout<<"IVP13DSystem::Imp::clickedoutside Error: Could not find system pointer!"<<std::endl;
    return;
  }
  if (system->m_d->selectionsWithDisabledNotifications.contains(selection))
    return;
  if (system->m_d->clickedoutsideScheduled)
    return;
  system->m_d->clickedoutsideScheduled = true;
  QTimer::singleShot(0, system, SLOT(activateClickedOutside()));
}

//___________________________________________________________________________________________________________
void IVP13DSystem::activateClickedOutside()
{
  if (!m_d->clickedoutsideScheduled)
    return;
  m_d->clickedoutsideScheduled = false;
  userClickedOnBgd();
}

//___________________________________________________________________________________________________________
void IVP13DSystem::Imp::start_changeselection(void * userdata, SoSelection *sel)
{
  SoCooperativeSelection * selection = static_cast<SoCooperativeSelection*>(sel);
  if (!selection) {
    std::cout<<"IVP13DSystem::Imp::start_changeselection Error: Could not find selection pointer!"<<std::endl;
    return;
  }

  if (selection->policy.getValue()==SoSelection::SINGLE)
    return;

  IVP13DSystem * system = static_cast<IVP13DSystem *>(userdata);
  if (!system) {
    std::cout<<"IVP13DSystem::Imp::start_changeselection Error: Could not find system pointer!"<<std::endl;
    return;
  }

  if (system->m_d->selectionsWithDisabledNotifications.contains(selection))
    return;

  system->m_d->selection2lastpathlist[selection] = *(selection->getList());

  //redraw and emission of itemFromSystemSelected() take place in finished_changeselection!

}

//___________________________________________________________________________________________________________
void IVP13DSystem::Imp::finished_changeselection(void *userdata, SoSelection *sel)
{
  SoCooperativeSelection * selection = static_cast<SoCooperativeSelection*>(sel);
  if (!selection) {
    std::cout<<"IVP13DSystem::Imp::finished_changeselection Error: Could not find selection pointer!"<<std::endl;
    return;
  }

  IVP13DSystem * system = static_cast<IVP13DSystem *>(userdata);
  if (!system) {
    std::cout<<"IVP13DSystem::Imp::finished_changeselection Error: Could not find system pointer!"<<std::endl;
    return;
  }

  selection->touch(); // to redraw

  if (selection->policy.getValue()==SoSelection::SINGLE)
    return;

  //Only take action when selection actually changed between start and finish:
  if (system->m_d->selection2lastpathlist.find(selection)==system->m_d->selection2lastpathlist.end()) {
    std::cout<<"IVP13DSystem::Imp::finished_changeselection Error: Could not find last selection path list!"<<std::endl;
    return;
  }

  int  nlastselection = system->m_d->selection2lastpathlist[selection].getLength();
  bool changed = false;
  if (nlastselection!=selection->getList()->getLength()) {
    changed = true;
  } else {
    for (int i = 0; i < selection->getList()->getLength(); ++i) {
      if (system->m_d->selection2lastpathlist[selection].get(i)!=selection->getList()->get(i)) {
	changed = true;
	break;
      }
    }
  }
  if (!changed) {
    system->m_d->selection2lastpathlist.erase(system->m_d->selection2lastpathlist.find(selection));
    return;
  }

  //To avoid having different systems emit itemFromSystemSelected due
  //to one user interaction, we only emit here if the action resulted
  //in an increase in the number of selected objects:
  if (selection->getList()->getLength()>nlastselection) {
    system->itemFromSystemSelected();//Emit this signal
  }

  if (system->m_d->selectionsWithDisabledNotifications.contains(selection))
    return;

  QSet<SoNode*> selnodes;
  QSet<SoPath*> selpaths;
  int n = selection->getList()->getLength();
  for (int i = 0; i < n; ++i) {
    SoPath * path = (*(selection->getList()))[i];
    if (!path)
      continue;
    SoFullPath *fPath = static_cast<SoFullPath *>(path);
    SoNode * node = fPath->getTail();
    if (!node)
      continue;
    selpaths << fPath;
    selnodes << node;
  }

  system->userChangedSelection(selection,selnodes,selpaths);
}

//___________________________________________________________________________________________________________
void IVP13DSystem::Imp::made_selection( void * userdata, SoPath * path )
{
  SoFullPath *fPath = static_cast<SoFullPath *>(path);
  if (!fPath)
    return;
  SoNode *selectedNode = fPath->getTail();
  if (!selectedNode)
    return;
  SoCooperativeSelection * selection = static_cast<SoCooperativeSelection*>(userdata);
  if (!selection) {
    std::cout<<"IVP13DSystem::Imp::made_selection Error: Could not find selection pointer!"<<std::endl;
    return;
  }
  if (selection->policy.getValue()!=SoSelection::SINGLE)
    return;

  if (Imp::selection2system.find(selection)==Imp::selection2system.end()) {
    std::cout << "IVP13DSystem::Imp::made_selection Error: Could not find system pointer!"<<std::endl;
    return;
  }

  IVP13DSystem * system = Imp::selection2system[selection];

  system->itemFromSystemSelected();//Emit this signal

  if (system->m_d->selectionsWithDisabledNotifications.contains(selection))
    return;


  system->userSelectedSingleNode(selection,selectedNode,fPath);

  //redraw takes place in finished_changeselection!
}

//___________________________________________________________________________________________________________
void IVP13DSystem::Imp::made_deselection( void * userdata, SoPath * path )
{
  SoFullPath *fPath = static_cast<SoFullPath *>(path);
  if (!fPath)
    return;
  SoNode *deselectedNode = fPath->getTail();
  if (!deselectedNode)
    return;

  SoCooperativeSelection * selection = static_cast<SoCooperativeSelection*>(userdata);
  if (!selection) {
    std::cout<<"IVP13DSystem::Imp::made_deselection Error: Could not find selection pointer!"<<std::endl;
    return;
  }
  if (selection->policy.getValue()!=SoSelection::SINGLE)
    return;

  if (Imp::selection2system.find(selection)==Imp::selection2system.end()) {
    std::cout << "IVP13DSystem::Imp::made_deselection Error: Could not find system pointer!"<<std::endl;
    return;
  }

  IVP13DSystem * system = Imp::selection2system[selection];
  if (system->m_d->selectionsWithDisabledNotifications.contains(selection))
    return;

  system->userDeselectedSingleNode(selection,deselectedNode,fPath);
}

//___________________________________________________________________________________________________________
void IVP13DSystem::registerSelectionNode( SoCooperativeSelection* selection )
{
  if (!selection) {
    message("registerSelectionNode Error: NULL selection pointer!");
    return;
  }
  if (Imp::selection2system.find(selection)!=Imp::selection2system.end()) {
    message("registerSelectionNode Error: Trying to register selection node more than once!");
    return;
  }

  selection->addSelectionCallback( Imp::made_selection, selection );
  selection->addDeselectionCallback( Imp::made_deselection, selection );
  selection->addStartCallback( Imp::start_changeselection, this );
  selection->addFinishCallback( Imp::finished_changeselection, this );
  selection->addClickOutsideCallback( Imp::clickedoutside, this );

  Imp::selection2system[selection] = this;
  selection->ref();

  messageVerbose("selection node registered");
}

//___________________________________________________________________________________________________________
void IVP13DSystem::unregisterSelectionNode( SoCooperativeSelection * selection )
{
  if (!selection) {
    message("unregisterSelectionNode Error: NULL selection pointer!");
    return;
  }
  if (Imp::selection2system.find(selection)==Imp::selection2system.end()) {
    message("registerSelectionNode Error: Trying to unregister unknown selection node!");
    return;
  }

  selection->removeSelectionCallback( Imp::made_selection, selection );
  selection->removeDeselectionCallback( Imp::made_deselection, selection );
  selection->removeStartCallback( Imp::start_changeselection, this );
  selection->removeFinishCallback( Imp::finished_changeselection, this );
  selection->removeClickOutsideCallback( Imp::clickedoutside, this );

  Imp::selection2system.erase(Imp::selection2system.find(selection));
  if (m_d->selectionsWithDisabledNotifications.contains(selection))
    m_d->selectionsWithDisabledNotifications.remove(selection);
  selection->unref();

  messageVerbose("selection node unregistered");

}



//___________________________________________________________________________________________________________
void IVP13DSystem::setUserSelectionNotificationsEnabled( SoCooperativeSelection * selection, bool enabled )
{
  if (!selection) {
    message("setUserSelectionNotificationsEnabled Error: NULL selection pointer!");
    return;
  }
  if (Imp::selection2system.find(selection)==Imp::selection2system.end()) {
    message("setUserSelectionNotificationsEnabled Error: Called for selection which was never registered!");
    return;
  }
  if (enabled != m_d->selectionsWithDisabledNotifications.contains(selection))
    return;

  if (enabled)
    m_d->selectionsWithDisabledNotifications.remove(selection);
  else
    m_d->selectionsWithDisabledNotifications << selection;

}

//___________________________________________________________________________________________________________
void IVP13DSystem::deselectAll(SoCooperativeSelection* exception_sel)
{
  static std::map<SoCooperativeSelection*,IVP13DSystem*>::iterator it, itE = Imp::selection2system.end();
  for (it = Imp::selection2system.begin(); it!=itE;++it) {
    if (it->second!=this)
      continue;
    if (it->first!=exception_sel) {
      if (it->first->policy.getValue()!=SoCooperativeSelection::SINGLE) {
 	Imp::start_changeselection(this, it->first);
	it->first->deselectAll();
 	Imp::finished_changeselection(this, it->first);
      } else {
	if (it->first->getList()->getLength()==1) {
 	  Imp::start_changeselection(this, it->first);
 	  SoPath * path = static_cast<SoPath*>(it->first->getList()->get(0));
 	  Imp::made_deselection(it->first,path);
	  it->first->deselectAll();
 	  Imp::finished_changeselection(this, it->first);
	}
      }
    }
  }
}

//___________________________________________________________________________________________________________
IVP13DSystem::IVP13DSystem( const QString & name, const QString & information, const QString & contact_info):
  IVP1System(name,information,contact_info), m_d(new Imp)
{
  SoCooperativeSelection::ensureInitClass();
  m_d->clickedoutsideScheduled = false;
}

//___________________________________________________________________________________________________________
IVP13DSystem::~IVP13DSystem()
{
  messageDebug("~IVP13DSystem()");

  m_d->selection2lastpathlist.clear();

  //Unregister all nodes for this system:
  std::set<SoCooperativeSelection*> sel2unregister;
  std::map<SoCooperativeSelection*,IVP13DSystem*>::iterator it, itE = Imp::selection2system.end();
  for (it = Imp::selection2system.begin();it!=itE;++it)
    if (it->second == this)
      sel2unregister.insert(it->first);
  std::set<SoCooperativeSelection*>::iterator itSel, itSelE = sel2unregister.end();

  for (itSel = sel2unregister.begin();itSel!=itSelE;++itSel) {
    unregisterSelectionNode(*itSel);
  }

  messageDebug("Unregistered all nodes. Unref all camera pointers...");

  //Unref all camera pointers:
  std::set<SoCamera*> ::iterator itCam, itCamE = m_d->staticcameras.end();
  for (itCam = m_d->staticcameras.begin();itCam!=itCamE;++itCam)
    (*itCam)->unref();

  messageDebug("Unref all camera pointers: done.");

  delete m_d; m_d=0;
}

//___________________________________________________________________________________________________________
std::set<SoCamera*> IVP13DSystem::getCameraList()
{
  std::set<SoCamera*> cameralist = m_d->staticcameras;
  std::set<SoQtViewer*>::const_iterator it, itE=m_d->viewers.end();
  for (it=m_d->viewers.begin();it!=itE;++it) {
    SoCamera*cam = (*it)->getCamera();
    if (cam)
      cameralist.insert(cam);
  }

  //m_d->camerasfromviewer
  return cameralist;
}

//___________________________________________________________________________________________________________
void IVP13DSystem::registerCamera(SoCamera *cam) {
  if (!cam)
    return;
  if (m_d->staticcameras.find(cam)==m_d->staticcameras.end())
    m_d->staticcameras.insert(cam);
  cam->ref();
}

//___________________________________________________________________________________________________________
void IVP13DSystem::registerViewer(SoQtViewer *viewer)
{
  if (!viewer)
    return;
  if (m_d->viewers.find(viewer)==m_d->viewers.end())
    m_d->viewers.insert(viewer);
}
