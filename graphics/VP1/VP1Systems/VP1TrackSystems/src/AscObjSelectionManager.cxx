/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class AscObjSelectionManager            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/AscObjSelectionManager.h"
#include "VP1TrackSystems/TrackSystemController.h"
#include "VP1TrackSystems/AssociatedObjectHandleBase.h"
#include "VP1Base/IVP13DSystem.h"
#include "VP1Base/SoCooperativeSelection.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/SoPath.h>
#include <QApplication>
#include<map>

//____________________________________________________________________
class AscObjSelectionManager::Imp {
public:
  Imp(AscObjSelectionManager * tc,SoSeparator* root,IVP13DSystem * sys,TrackSystemController*c)
    : theclass(tc),
      eventRoot(root),
      system(sys),
      controller(c),
      mode(SINGLE)
  {
    eventRoot->ref();
    sel_assocobjs_click = new SoCooperativeSelection;
    sel_assocobjs_click->setName("sel_assocobjs_click");
    sel_assocobjs_click->ref();
    sel_assocobjs_click->activePolicy = SoCooperativeSelection::ACTIVE;
    sel_assocobjs_click->policy = SoCooperativeSelection::SINGLE;
    sys->registerSelectionNode(sel_assocobjs_click);

    sel_assocobjs = new SoCooperativeSelection;
    sel_assocobjs->setName("sel_assocobjs");
    sel_assocobjs->ref();
    sel_assocobjs->activePolicy = SoCooperativeSelection::INERT;
    sel_assocobjs_click->policy = SoCooperativeSelection::SINGLE;
    sys->registerSelectionNode(sel_assocobjs);

    sel_assocobjs->addChild(sel_assocobjs_click);
    root->addChild(sel_assocobjs);
  }
  ~Imp()
  {
    system->unregisterSelectionNode(sel_assocobjs_click);
    sel_assocobjs_click->unref();
    sel_assocobjs->unref();
    eventRoot->unref();
  }

  AscObjSelectionManager * theclass;
  SoSeparator* eventRoot;
  IVP13DSystem * system;
  TrackSystemController*controller;
  MODE mode;
  SoCooperativeSelection * sel_assocobjs_click;
  SoCooperativeSelection * sel_assocobjs;

  std::map<SoSeparator*,AssociatedObjectHandleBase*> ascobjs_simpleToHandle;
  std::map<SoSeparator*,AssociatedObjectHandleBase*> ascobjs_detailedToHandle;

//   bool ascObjHandleFixSelectionPath(AssociatedObjectHandleBase*,SoPath*);
  AssociatedObjectHandleBase* ascObjHandle(const SoPath*);
  AssociatedObjectHandleBase* ascObjHandle(const SoSeparator*);
  QList<AssociatedObjectHandleBase*> selAscObjHandles;

  void updateSelectionVisuals();
  static bool shiftIsDown() { return Qt::ShiftModifier & QApplication::keyboardModifiers(); }

};



//____________________________________________________________________
AscObjSelectionManager::AscObjSelectionManager(SoSeparator* event_root,IVP13DSystem * sys,TrackSystemController*controller)
  : QObject(sys), VP1HelperClassBase(sys,"AscObjSelectionManager"), m_d(new Imp(this,event_root,sys,controller))
{
  connect(controller,SIGNAL(assocObjDetailLevelChanged(TrackCommonFlags::DETAILLEVEL)),this,SLOT(ascObjDetailLevelChanged()));
  ascObjDetailLevelChanged();
}

//____________________________________________________________________
void AscObjSelectionManager::aboutTodelete()
{
  //fixme: clear maps and emit signals!
}

//____________________________________________________________________
AscObjSelectionManager::~AscObjSelectionManager()
{
  //Fixme: sanity check that maps are cleared.
  delete m_d;
}

//____________________________________________________________________
QString AscObjSelectionManager::toString(MODE m) const
{
  if (m==SINGLE) return "SINGLE";
  else if (m==TOGGLE) return "TOGGLE";
  else if (m==SHIFT) return "SHIFT";
  else return "UNKNOWN(ERROR)";
}

//____________________________________________________________________
AscObjSelectionManager::MODE AscObjSelectionManager::mode() const
{
  return m_d->mode;
}

//____________________________________________________________________
void AscObjSelectionManager::setMode(MODE m)
{
  if (m_d->mode==m)
    return;
  message("Mode changed to "+toString(m));
  m_d->mode=m;
}


//____________________________________________________________________
SoSeparator * AscObjSelectionManager::getAscObjAttachSep() const
{
  return m_d->sel_assocobjs_click;
}
//____________________________________________________________________
void AscObjSelectionManager::registerAscObj(SoSeparator*simple,SoSeparator *detailed,AssociatedObjectHandleBase*handle)
{
  if (verbose()) {
    if (!simple||!detailed||!handle) {
      message("registerAscObj ERROR: Received null pointer!");
      return;
    }
    std::map<SoSeparator*,AssociatedObjectHandleBase*>::iterator it, itE;
    itE = m_d->ascobjs_simpleToHandle.end();
    for (it=m_d->ascobjs_simpleToHandle.begin();it!=itE;++it) {
      if (it->first==simple||it->second==handle)
	message("registerAscObj ERROR: Simple separator/handle already registered!");
    }
    itE = m_d->ascobjs_detailedToHandle.end();
    for (it=m_d->ascobjs_detailedToHandle.begin();it!=itE;++it) {
      if (it->first==detailed||it->second==handle)
	message("registerAscObj ERROR: Detailed separator/handle already registered!");
    }
  }
  m_d->ascobjs_simpleToHandle[simple]=handle;
  m_d->ascobjs_detailedToHandle[detailed]=handle;
}

//____________________________________________________________________
void AscObjSelectionManager::unregisterAscObj(SoSeparator*simple,SoSeparator *detailed)
{
  if (verbose()) {
    if (!simple||!detailed) {
      message("unregisterAscObj ERROR: Received null pointer!");
      return;
    }
  }
  std::map<SoSeparator*,AssociatedObjectHandleBase*>::iterator itSimple = m_d->ascobjs_simpleToHandle.find(simple);
  if (itSimple==m_d->ascobjs_simpleToHandle.end()) {
    message("unregisterAscObj ERROR: Not previously registered simple sep!");
  } else {
    m_d->ascobjs_simpleToHandle.erase(itSimple);
  }
  std::map<SoSeparator*,AssociatedObjectHandleBase*>::iterator itDetailed = m_d->ascobjs_detailedToHandle.find(detailed);
  if (itDetailed==m_d->ascobjs_detailedToHandle.end()) {
    message("unregisterAscObj ERROR: Not previously registered detailed sep!");
  } else {
    m_d->ascobjs_detailedToHandle.erase(itDetailed);
  }
}

//____________________________________________________________________
AssociatedObjectHandleBase* AscObjSelectionManager::Imp::ascObjHandle(const SoPath*path)
{
  const int n(path?path->getLength():0);
  AssociatedObjectHandleBase * handle(0);
  for (int i = 0; i < n; ++i) {
    if (path->getNodeFromTail(i)->getTypeId()==SoSeparator::getClassTypeId()) {
      handle = ascObjHandle(static_cast<SoSeparator*>(path->getNodeFromTail(i)));
      if (handle)
	return handle;
    }
  }
  return 0;
}

//____________________________________________________________________
AssociatedObjectHandleBase* AscObjSelectionManager::Imp::ascObjHandle(const SoSeparator*s)
{
  std::map<SoSeparator*,AssociatedObjectHandleBase*>::const_iterator
    it(ascobjs_simpleToHandle.find(const_cast<SoSeparator*>(s)));
  if (it!=ascobjs_simpleToHandle.end())
    return it->second;
  it = ascobjs_detailedToHandle.find(const_cast<SoSeparator*>(s));
  return it==ascobjs_detailedToHandle.end() ? 0 : it->second;
}

//____________________________________________________________________
void AscObjSelectionManager::Imp::updateSelectionVisuals()
{
  const bool isSimpleMode = controller->assocObjDetailLevel()==TrackCommonFlags::SIMPLE;
  sel_assocobjs->deselectAll();
  foreach (AssociatedObjectHandleBase* handle,selAscObjHandles) {
    SoPath * path = new SoPath(sel_assocobjs);
    path->ref();
    if (!VP1QtInventorUtils::changePathTail(path,sel_assocobjs,
					    (isSimpleMode?handle->shapeSimple():handle->shapeDetailed()))) {
      theclass->message("updateSelectionVisuals ERROR: Failed to relocate picked node.");
      path->unref();
      continue;
    }
    sel_assocobjs->select(path);
    path->unref();
  }
}


//____________________________________________________________________
bool AscObjSelectionManager::handleUserSelectedSingleNode( SoCooperativeSelection* sel, SoNode*, SoPath* pickedPath, AssociatedObjectHandleBase*& pickedHandle )
{
  messageVerbose("handleUserSelectedSingleNode");
  pickedHandle = 0;
  if (sel==m_d->sel_assocobjs) {
    messageVerbose("  => ignore selections for m_d->sel_assocobjs");
    return true;//We simply ignore those
  }
  if (sel!=m_d->sel_assocobjs_click) {
    messageVerbose("  => We don't handle this selection.");
    return false;
  }

  AssociatedObjectHandleBase* handle = m_d->ascObjHandle(pickedPath);
  m_d->sel_assocobjs_click->deselectAll();
  if (!handle) {
    message("ERROR: Unknown associated object.");
    return true;
  }
  messageVerbose("  => Found handle. Mode is "+toString(m_d->mode)+", and number of previously selected handles is "+str(m_d->selAscObjHandles.count()));
  pickedHandle = handle;
  pretendUserClicked(handle);//we are not really pretending in this case of course...
  return true;
}

//____________________________________________________________________
void AscObjSelectionManager::pretendUserClicked(AssociatedObjectHandleBase*handle)
{
  assert(handle);
  if (!handle)
    return;
  const bool alreadyselected = m_d->selAscObjHandles.contains(handle);
  qSort(m_d->selAscObjHandles);
  QList<AssociatedObjectHandleBase*> selHandlesBefore = m_d->selAscObjHandles;

  if (m_d->mode==SINGLE) {
    if (m_d->selAscObjHandles.isEmpty()) {
      //Add handle to selection.
      m_d->selAscObjHandles << handle;
      if (m_d->controller->printInfoOnSingleSelection())
      m_d->system->message(handle->clicked());
    } else {
      //Clear selection.
      m_d->selAscObjHandles.clear();
      //Add if not already selected:
      if (!alreadyselected) {
	m_d->selAscObjHandles << handle;
	if (m_d->controller->printInfoOnSingleSelection())
	  m_d->system->message(handle->clicked());
      }
    }
  } else if (m_d->mode==SHIFT) {
    if (Imp::shiftIsDown()) {
      if (alreadyselected)
	m_d->selAscObjHandles.removeAll(handle);
      else
	m_d->selAscObjHandles << handle;
    } else {
      m_d->selAscObjHandles.clear();
      if (!alreadyselected)
	m_d->selAscObjHandles << handle;
    }
  } else if (m_d->mode==TOGGLE) {
    if (alreadyselected) {
      m_d->selAscObjHandles.removeAll(handle);
    } else {
      m_d->selAscObjHandles << handle;
    }
  } else {
    message("ERROR: Should not happen!");
    deselectAll();
    return;
  }
  qSort(m_d->selAscObjHandles);

  if (selHandlesBefore!=m_d->selAscObjHandles) {
    m_d->updateSelectionVisuals();
    currentSelectionChanged(m_d->selAscObjHandles);
  }
}


//____________________________________________________________________
void AscObjSelectionManager::userClickedOnBgd()
{
  if (m_d->mode==TOGGLE)
    return;
  if (m_d->mode==SHIFT&&Imp::shiftIsDown())
    return;
  deselectAll();
}

//____________________________________________________________________
void AscObjSelectionManager::deselectAll()
{
  if (m_d->selAscObjHandles.isEmpty())
    return;
  m_d->selAscObjHandles.clear();
  currentSelectionChanged(m_d->selAscObjHandles);
  m_d->updateSelectionVisuals();
}

//____________________________________________________________________
void AscObjSelectionManager::ensureDeselected(const QList<AssociatedObjectHandleBase*>& handles)
{
  if (handles.isEmpty())
    return;
  QList<AssociatedObjectHandleBase*> selHandlesBefore = m_d->selAscObjHandles;
  foreach (AssociatedObjectHandleBase*handle,handles)
    m_d->selAscObjHandles.removeAll(handle);
  if (selHandlesBefore!=m_d->selAscObjHandles) {
    m_d->updateSelectionVisuals();
    currentSelectionChanged(m_d->selAscObjHandles);
  }
}

//____________________________________________________________________
void AscObjSelectionManager::ensureSelected(const QList<AssociatedObjectHandleBase*>& handles)
{
  if (handles.isEmpty())
    return;
  if (m_d->mode==SINGLE) {
    //Single selections
    if (handles.count()>1)
      message("WARNING: ensureSelected called with more than one handle in SINGLE mode. Ignoring all but the first.");
    if (m_d->selAscObjHandles.contains(handles.at(0)))
      return;
    m_d->selAscObjHandles.clear();
    m_d->selAscObjHandles << handles.at(0);
    currentSelectionChanged(m_d->selAscObjHandles);
    m_d->updateSelectionVisuals();
  } else {
    //Multi selections allowed
    QList<AssociatedObjectHandleBase*> selHandlesBefore = m_d->selAscObjHandles;
    foreach (AssociatedObjectHandleBase*handle,handles) {
      if (!m_d->selAscObjHandles.contains(handle))
	m_d->selAscObjHandles << handle;
    }
    qSort(m_d->selAscObjHandles);
    if (selHandlesBefore!=m_d->selAscObjHandles) {
      m_d->updateSelectionVisuals();
      currentSelectionChanged(m_d->selAscObjHandles);
    }
  }

}

//____________________________________________________________________
void AscObjSelectionManager::ascObjDetailLevelChanged()
{
  messageVerbose("Signal received in ascObjDetailLevelChanged slot");
  if (m_d->selAscObjHandles.isEmpty()) {
    m_d->sel_assocobjs->deselectAll();
    return;
  }
  const SoPathList * pathlist = m_d->sel_assocobjs->getList();
  if (!pathlist||pathlist->getLength()!=m_d->selAscObjHandles.count())
    return;

  const bool isSimpleMode = (m_d->controller->assocObjDetailLevel()==TrackCommonFlags::SIMPLE);
  int i(0);
  foreach (AssociatedObjectHandleBase* handle,m_d->selAscObjHandles) {
    SoPath * path = (*pathlist)[i++];
    if (!path)
      continue;
    if (!VP1QtInventorUtils::changePathTail(path,m_d->sel_assocobjs,
					    (isSimpleMode?handle->shapeSimple():handle->shapeDetailed()))) {
      message("Warning: Failed to relocate picked node.");
      deselectAll();
      return;
    }
  }
}

//____________________________________________________________________
QList<AssociatedObjectHandleBase*> AscObjSelectionManager::currentSelection() const
{
  return m_d->selAscObjHandles;
}

SoSeparator* AscObjSelectionManager::eventRoot()
{
  return m_d->eventRoot;
}
