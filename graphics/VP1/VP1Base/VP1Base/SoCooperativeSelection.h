/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SOCOOPERATIVESELECTION_H
#define SOCOOPERATIVESELECTION_H

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/fields/SoSFNode.h>

// Class: SoCooperativeSelection.
// Author: Thomas.Kittelmann@cern.ch
// Initial version: October 2007
//
//  "Cooperative" extension of SoSelection which does NOT mark events
//  as handled when the picked point wasn't actually on any of its
//  children (but it still deselects if appropriate).
//
//  Furthermore, it ignores events if it is not the
//  SoSelection/SoCooperativeSelection node furthest down on the path (thus
//  selection behaviour can be easily changed for a given subgraph by
//  the insertion of a new selection node)

class SoCooperativeSelection;
typedef void SoCooperativeSelectionClickOutsideCB(void * data,SoCooperativeSelection * sel);

class SoCooperativeSelection : public SoSelection {

  SO_NODE_HEADER(SoCooperativeSelection);

public:
  static void initClass();
  SoCooperativeSelection();
  SoCooperativeSelection(const int nChildren);

  static void ensureInitClass();//Use this instead of initClass() (multiple invocations are harmless).

  enum ActivePolicy {
    INERT, ACTIVE
  };

  //INERT: The node simply behaves as an SoSeparator, allowing
  //       SoCooperativeSelection nodes further up the tree to handle the event.
  //ACTIVE: Active (default). SoCooperativeSelection Nodes further up in the
  //        tree will never handle the event.

  SoSFEnum activePolicy;

  void addClickOutsideCallback(SoCooperativeSelectionClickOutsideCB * f, void * userData = 0);
  void removeClickOutsideCallback(SoCooperativeSelectionClickOutsideCB * f, void * userData = 0);

protected:
  virtual ~SoCooperativeSelection();
  virtual void handleEvent(SoHandleEventAction * action);

private:
  //The implementation of this method is copied verbatim from the base
  //class (since it is private rather than protected there).
  SoPath *getSelectionPath( SoHandleEventAction *action,
			    SbBool &ignorepick, SbBool &haltaction );

  //Helper method:
  SoCooperativeSelection * getLastActiveSoSelectionFromPath(SoPath*) const;

  SoCallbackList *clickoutsideCBList;

  //Common code for both constructors:
  void init();
  static bool needsinit;
};

#endif
