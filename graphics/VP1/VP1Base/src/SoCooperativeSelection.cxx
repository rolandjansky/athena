/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Class: SoCooperativeSelection.
// Author: Thomas.Kittelmann@cern.ch
// Initial version: October 2007

#include "VP1Base/SoCooperativeSelection.h"

#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/lists/SoCallbackList.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/events/SoMouseButtonEvent.h>


SO_NODE_SOURCE(SoCooperativeSelection)

//____________________________________________________________________
void SoCooperativeSelection::initClass()
{
  SO_NODE_INIT_CLASS(SoCooperativeSelection,SoSelection,"CooperativeSelection");
}

bool SoCooperativeSelection::s_needsinit = true;

//____________________________________________________________________
void SoCooperativeSelection::ensureInitClass()
{
  if (SoCooperativeSelection::s_needsinit) {
    SoCooperativeSelection::s_needsinit = false;
    initClass();
  }
}

//____________________________________________________________________
SoCooperativeSelection::SoCooperativeSelection()
  : SoSelection()
{
  init();
}

//____________________________________________________________________
SoCooperativeSelection::SoCooperativeSelection(const int nChildren)
  : SoSelection(nChildren)
{
  init();
}

//____________________________________________________________________
void SoCooperativeSelection::init()
{
  SO_NODE_CONSTRUCTOR(SoCooperativeSelection);

  SO_NODE_ADD_FIELD(activePolicy, (SoCooperativeSelection::ACTIVE));
  SO_NODE_DEFINE_ENUM_VALUE(ActivePolicy, INERT);
  SO_NODE_DEFINE_ENUM_VALUE(ActivePolicy, ACTIVE);
  SO_NODE_SET_SF_ENUM_TYPE(activePolicy, ActivePolicy);

  setNodeType(EXTENSION);
  m_clickoutsideCBList = new SoCallbackList;

}

//____________________________________________________________________
SoCooperativeSelection::~SoCooperativeSelection()
{
  if (this->mouseDownPickPath)
    this->mouseDownPickPath->unref();
  delete m_clickoutsideCBList;
}

//____________________________________________________________________
SoCooperativeSelection * SoCooperativeSelection::getLastActiveSoSelectionFromPath(SoPath*path) const
{
  for (int i = 0; i < path->getLength(); ++i) {
    if (path->getNodeFromTail(i)->getTypeId().isDerivedFrom(SoCooperativeSelection::getClassTypeId())) {
      SoCooperativeSelection* sel = static_cast<SoCooperativeSelection*>(path->getNodeFromTail(i));
      if (sel->activePolicy.getValue() == ACTIVE)
	return sel;
    }
  }
  return 0;
}

//____________________________________________________________________
void SoCooperativeSelection::addClickOutsideCallback(SoCooperativeSelectionClickOutsideCB * f, void * userData)
{
  m_clickoutsideCBList->addCallback((SoCallbackListCB *)f, userData);
}

//____________________________________________________________________
void SoCooperativeSelection::removeClickOutsideCallback(SoCooperativeSelectionClickOutsideCB * f, void * userData)
{
  m_clickoutsideCBList->removeCallback((SoCallbackListCB *)f, userData);
}

//____________________________________________________________________
void SoCooperativeSelection::handleEvent(SoHandleEventAction * action)
{
  // Overridden to do selection picking.
  SoSeparator::handleEvent(action);//Skip the SoSelection impl and go directly to the SoSeparator action!!

  if (activePolicy.getValue()==INERT)
    return;

  const SoEvent * event = action->getEvent();

  SbBool haltaction = FALSE;
  if (SO_MOUSE_PRESS_EVENT(event, BUTTON1)) {
    if (this->mouseDownPickPath) {
      this->mouseDownPickPath->unref();
      this->mouseDownPickPath = NULL;
    }
    const SoPickedPoint * pp = action->getPickedPoint();
    if (!pp)
      m_clickoutsideCBList->invokeCallbacks(this);
    if (pp) {
      SoPath * selectionpath = pp->getPath();
      if (!selectionpath)
	return;
      // call pick filter callback also for mouse down events
      if (this->pickCBFunc && (!this->callPickCBOnlyIfSelectable ||
                               selectionpath->findNode(this) >= 0)) {
	selectionpath = this->pickCBFunc(this->pickCBData, pp);
      }

      SoCooperativeSelection * lastsoselectionfrompath = getLastActiveSoSelectionFromPath(selectionpath);

      if (selectionpath&&lastsoselectionfrompath==this) {
	//We are the selection node furthest down the path => handle the event.
	this->mouseDownPickPath = selectionpath;
	this->mouseDownPickPath->ref();
	action->setHandled();
      }
    }
  }
  else if (SO_MOUSE_RELEASE_EVENT(event, BUTTON1)) {

    SbBool ignorepick = FALSE;
    // call pick filter callback (called from getSelectionPath()) even
    // if the event was handled by a child node.
    SoPath * selpath = this->getSelectionPath(action, ignorepick, haltaction);

    if (action->isHandled()) {
      // if the event was handled by a child node we should not invoke
      // the selection policy
      if (selpath) {
        selpath->ref();
        selpath->unref();
      }
    }
    else {
      if (haltaction) action->setHandled();

      if (!ignorepick) {
        if (selpath) selpath->ref();
        this->startCBList->invokeCallbacks(this);
        this->invokeSelectionPolicy(selpath, event->wasShiftDown());
        this->finishCBList->invokeCallbacks(this);
        if (selpath) selpath->unref();
      }
    }
    if (this->mouseDownPickPath) {
      this->mouseDownPickPath->unref();
      this->mouseDownPickPath = NULL;
    }
  }
}


//Implementation of getSelectionPath is copied verbatim from
//SoSelection.cpp. If only it was protected rather than private...

SoPath *
SoCooperativeSelection::getSelectionPath(SoHandleEventAction * action, SbBool & ignorepick,
					 SbBool & haltaction)
{
  //
  // handled like described in the man-pages for SoSelection
  //

  haltaction = FALSE;
  ignorepick = FALSE;
  if (this->pickMatching && this->mouseDownPickPath == NULL) {
    return NULL;
  }
  const SoPickedPoint * pp = action->getPickedPoint();
  SoPath * selectionpath = NULL;
  if (pp) {
    selectionpath = pp->getPath();
    // if there's no pickCBFunc we can just test against
    // mouseDownPickPath and (possibly) return here.
    if (this->pickMatching && !this->pickCBFunc) {
      if (*(this->mouseDownPickPath) != *selectionpath) {
        ignorepick = TRUE;
        return NULL;
      }
    }
    // if we have a pickCBFunc we have to get the pick filter path
    // before comparing the mouse press and mouse release paths
    if (this->pickCBFunc && (!this->callPickCBOnlyIfSelectable ||
                             selectionpath->findNode(this) >= 0)) {
      selectionpath = this->pickCBFunc(this->pickCBData, pp);

      // From the SoSelection man-pages:
      // Possible return values from pickCBFunc:
      // 1) NULL - behave as if nothing was picked, halt action
      // 2) path through the selection node - select/deselect path
      // 3) path containing only the selection node - as 1, but do not halt action
      // 4) path not through the selection node - ignore event
      if (selectionpath) {
        if (selectionpath->getLength() == 1 &&
            selectionpath->getNode(0) == this) {
          selectionpath->ref();
          selectionpath->unref();
          selectionpath = NULL;
        }
        else if (selectionpath->findNode(this) >= 0) {
          if (*(this->mouseDownPickPath) == *selectionpath) {
            // pick matched
            haltaction = TRUE;
          }
          else {
            // mouse release didn't match mouse down
            ignorepick = TRUE;
            selectionpath->ref();
            selectionpath->unref();
            ignorepick = TRUE;
          }
        }
        else { // path with this not in the path (most probably an empty path)
          selectionpath->ref();
          selectionpath->unref();
          selectionpath = NULL;
          ignorepick = TRUE;
        }
      }
      else { // pickCBFunc returned NULL
        haltaction = TRUE;
      }
    }
    else { // no pickCBFunc or not a valid path
      haltaction = FALSE;
    }
  }
  else if (this->mouseDownPickPath) {
    ignorepick = TRUE;
  }
  return selectionpath;
}
