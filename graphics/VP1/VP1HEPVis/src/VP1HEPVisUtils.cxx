/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1HEPVisUtils                    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: November 2008                            //
//  Update: Giorgi Gvaberidze (ggvaberi@cern.ch)              //
//  Update  version: November 2010                            //  
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1HEPVis/VP1HEPVisUtils.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
//---
#include <Inventor/nodes/SoSphere.h>

#include "VP1HEPVis/nodes/SoCons.h"
#include "VP1HEPVis/nodes/SoGenericBox.h"
#include "VP1HEPVis/nodes/SoLAr.h"
#include "VP1HEPVis/nodes/SoPcons.h"
#include "VP1HEPVis/nodes/SoPolyhedron.h"
#include "VP1HEPVis/nodes/SoTransparency.h"
#include "VP1HEPVis/nodes/SoTubs.h"
#include "VP1HEPVis/nodes/SoTessellated.h"

#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <VP1HEPVis/SbPolyhedron.h>

#include <iostream>//fixme

//____________________________________________________________________
class VP1HEPVisUtils::Imp {
public:
  template <class T>
  static SoNode * convertToAlternateRep(SoNode *);

  template <class T>
  static void clearAlternateRep(SoNode *);

  template <class T>
  static void updateAlternateRepIfNull(SoNode *);

  static bool isGroup(SoNode* n);
  static bool isNonCustomTree(SoGroup* g);
  static SoGroup * convertToStandardGroupNode(SoGroup* g,bool transferChildren);
  static SoGroup * convertToStandardScene(SoGroup* g);
  static void updateAllNullAlternativeReps(SoGroup*g);
};

//____________________________________________________________________
bool VP1HEPVisUtils::Imp::isNonCustomTree(SoGroup* g)
{
  if (!g)
    return true;//well...

  if (isCustomNode(g))
    return false;

  for (int i = 0; i<g->getNumChildren();++i) {
    SoNode * n = g->getChild(i);
    if ( Imp::isGroup(n) ) {
      if (!isNonCustomTree(static_cast<SoGroup*>(n)))
	return false;
    } else {
      if (isCustomNode(n))
	return false;
    }
  }
  return true;
}

//____________________________________________________________________
SoGroup * VP1HEPVisUtils::Imp::convertToStandardGroupNode(SoGroup* g,bool transferChildren)
{
  //Create new non-custom node and transfer relevant fields:
  SoGroup * newgroup(0);
  if ( g->getTypeId().isDerivedFrom(SoSeparator::getClassTypeId()) &&
       ! g->getTypeId().isDerivedFrom(SoSelection::getClassTypeId()) ) {
    SoSeparator * sep = new SoSeparator;
    SoSeparator * origsep = static_cast<SoSeparator*>(g);
    sep->renderCaching.setValue(origsep->renderCaching.getValue());
    sep->boundingBoxCaching.setValue(origsep->boundingBoxCaching.getValue());
    sep->renderCulling.setValue(origsep->renderCulling.getValue());
    sep->pickCulling.setValue(origsep->pickCulling.getValue());
    newgroup = sep;
  } else if ( g->getTypeId().isDerivedFrom(SoSelection::getClassTypeId()) ) {
    SoSelection * sel = new SoSelection;
    SoSelection * origsel = static_cast<SoSelection*>(g);
    sel->renderCaching.setValue(origsel->renderCaching.getValue());
    sel->boundingBoxCaching.setValue(origsel->boundingBoxCaching.getValue());
    sel->renderCulling.setValue(origsel->renderCulling.getValue());
    sel->pickCulling.setValue(origsel->pickCulling.getValue());
    sel->policy.setValue(origsel->policy.getValue());
    newgroup = sel;
  } else if ( g->getTypeId().isDerivedFrom(SoSwitch::getClassTypeId()) ) {
    SoSwitch *sw = new SoSwitch;
    sw->whichChild.setValue(static_cast<SoSwitch*>(g)->whichChild.getValue());
    newgroup = sw;
  } else {
    //Everything else is just treated like a group (we could add more specialisations):
    newgroup = new SoGroup;
  }

  //Transfer name:
  newgroup->setName(g->getName());

  //Transfer children:
  if (transferChildren)
    for (int i = 0; i<g->getNumChildren();++i)
      newgroup->addChild(g->getChild(i));

  return newgroup;
}

//____________________________________________________________________
SoGroup * VP1HEPVisUtils::Imp::convertToStandardScene(SoGroup* g)
{
  if (Imp::isNonCustomTree(g))
    return g;

  //We have to change something underneath, so we must get a new (and non-custom) group node:
  SoGroup * thegroup = convertToStandardGroupNode(g,false/*don't transfer children*/);
  if (!thegroup)
    std::cout <<"BAAAAAAAAAD!"<<std::endl;
  for (int i = 0; i<g->getNumChildren();++i) {
    SoNode * n = g->getChild(i);
    if (isGroup(n)) {
      SoGroup * standard = convertToStandardScene(static_cast<SoGroup*>(n));
      if (standard)
	thegroup->addChild(standard);
      else
	std::cout <<"BAD 1"<<std::endl;
    } else {
      SoNode * ntoadd = isCustomNode(n)?convertCustomNodeToAlternateRep(n):n;
      if (ntoadd)
	thegroup->addChild(isCustomNode(n)?convertCustomNodeToAlternateRep(n):n);
      else
	std::cout <<"BAD 2"<<std::endl;
    }
  }

  return thegroup;
}


//____________________________________________________________________
SoGroup * VP1HEPVisUtils::convertToStandardScene(SoGroup* g)
{
  if (!g)
    return 0;

  initAllCustomClasses();

  if (Imp::isNonCustomTree(g))
    return g;


  g->ref();

  updateAllAlternativeReps(g);

  SoGroup* thegroup = Imp::convertToStandardScene(g);
  thegroup->ref();

  clearAllAlternativeReps(g);

  g->unrefNoDelete();
  thegroup->unrefNoDelete();
  return thegroup;
}

//____________________________________________________________________
bool VP1HEPVisUtils::isCustomNode(SoNode* n)
{
  if (!n)
    return false;

  return n->getNodeType() & SoNode::EXTENSION;
}

//____________________________________________________________________
template <class T>
SoNode * VP1HEPVisUtils::Imp::convertToAlternateRep(SoNode * n)
{
  if (!n)
    return 0;
  T * t = static_cast<T*>(n);
  SoNode * n_alt = t->alternateRep.getValue();
  if (n_alt) {
    //Don't clear the alternateRep, since it was there before.
  } else {
    t->generateAlternateRep();
    n_alt = t->alternateRep.getValue();
    if (n_alt) {
      //Clear the alternateRep, since it was not there before.
      n_alt->ref();
      t->clearAlternateRep();
      n_alt->unrefNoDelete();
    } else {
      return 0;
    }
  }
  n_alt->setName(n->getName());
  return n_alt;
}

//____________________________________________________________________
template <class T>
void VP1HEPVisUtils::Imp::clearAlternateRep(SoNode * n)
{
  T * t = static_cast<T*>(n);
  if (t&&t->alternateRep.getValue())
    t->clearAlternateRep();
}

//____________________________________________________________________
template <class T>
void VP1HEPVisUtils::Imp::updateAlternateRepIfNull(SoNode * n)
{
  T * t = static_cast<T*>(n);
  if (t&&!t->alternateRep.getValue()) {
    t->generateAlternateRep();
    t->clearAlternateRep();
  }
}

//____________________________________________________________________
SoNode * VP1HEPVisUtils::convertCustomNodeToAlternateRep(SoNode*n)
{
  if (!n)
    return 0;

  initAllCustomClasses();

  if ( n->getTypeId().isDerivedFrom(SoCons::getClassTypeId()) ) return Imp::convertToAlternateRep<SoCons>(n);
  if ( n->getTypeId().isDerivedFrom(SoLAr::getClassTypeId()) ) return Imp::convertToAlternateRep<SoLAr>(n);
//NOT IMPLEMENTED   if ( n->getTypeId().isDerivedFrom(SoPcons::getClassTypeId()) ) return Imp::convertToAlternateRep<SoPcons>(n);
  if ( n->getTypeId().isDerivedFrom(SoPcons::getClassTypeId()) ) return Imp::convertToAlternateRep<SoPcons>(n);
  if ( n->getTypeId().isDerivedFrom(SoGenericBox::getClassTypeId()) ) return Imp::convertToAlternateRep<SoGenericBox>(n);
  if ( n->getTypeId().isDerivedFrom(SoTubs::getClassTypeId()) ) return Imp::convertToAlternateRep<SoTubs>(n);
  if ( n->getTypeId().isDerivedFrom(SoPolyhedron::getClassTypeId()) ) return Imp::convertToAlternateRep<SoPolyhedron>(n);
  if ( n->getTypeId().isDerivedFrom(SoTessellated::getClassTypeId()) ) return Imp::convertToAlternateRep<SoTessellated>(n);

  //A bit special. Probably should not get called here.
  if ( Imp::isGroup(n) ) return Imp::convertToStandardGroupNode(static_cast<SoGroup*>(n),false /*transferChildren*/);
  SoGroup * g = new SoGroup;
  return g;//fixme: share this
}

//____________________________________________________________________
bool VP1HEPVisUtils::Imp::isGroup(SoNode* n)
{
  return n->getTypeId().isDerivedFrom(SoGroup::getClassTypeId());
}

//____________________________________________________________________
void VP1HEPVisUtils::Imp::updateAllNullAlternativeReps(SoGroup*g)
{
  for (int i = 0; i<g->getNumChildren();++i) {
    SoNode * n = g->getChild(i);
    if ( Imp::isGroup(n) ) {
      updateAllNullAlternativeReps(static_cast<SoGroup*>(n));
    } else if (VP1HEPVisUtils::isCustomNode(n)) {
      if ( n->getTypeId().isDerivedFrom(SoCons::getClassTypeId()) ) return Imp::updateAlternateRepIfNull<SoCons>(n);
      if ( n->getTypeId().isDerivedFrom(SoLAr::getClassTypeId()) ) return Imp::updateAlternateRepIfNull<SoLAr>(n);
      //NOT IMPLEMENTED if ( n->getTypeId().isDerivedFrom(SoPcons::getClassTypeId()) ) return Imp::updateAlternateRepIfNull<SoPcons>(n);
      if ( n->getTypeId().isDerivedFrom(SoPcons::getClassTypeId()) ) return Imp::updateAlternateRepIfNull<SoPcons>(n);
      if ( n->getTypeId().isDerivedFrom(SoGenericBox::getClassTypeId()) ) return Imp::updateAlternateRepIfNull<SoGenericBox>(n);
      if ( n->getTypeId().isDerivedFrom(SoTubs::getClassTypeId()) ) return Imp::updateAlternateRepIfNull<SoTubs>(n);
      if ( n->getTypeId().isDerivedFrom(SoPolyhedron::getClassTypeId()) ) return Imp::updateAlternateRepIfNull<SoPolyhedron>(n);
      if ( n->getTypeId().isDerivedFrom(SoTessellated::getClassTypeId()) ) return Imp::updateAlternateRepIfNull<SoTessellated>(n);
    }
  }
}

//____________________________________________________________________
void VP1HEPVisUtils::updateAllAlternativeReps(SoGroup*g)
{
  clearAllAlternativeReps(g);//To make sure all altReps. are NULL
			     //(thus, we only update those with null,
			     //avoiding repeated updates on shared
			     //instances)
  Imp::updateAllNullAlternativeReps(g);
}

//____________________________________________________________________
void VP1HEPVisUtils::clearAllAlternativeReps(SoGroup*g)
{
  for (int i = 0; i<g->getNumChildren();++i) {
    SoNode * n = g->getChild(i);
    if ( Imp::isGroup(n) ) {
      clearAllAlternativeReps(static_cast<SoGroup*>(n));
    } else if (isCustomNode(n)) {
      if ( n->getTypeId().isDerivedFrom(SoCons::getClassTypeId()) ) return Imp::clearAlternateRep<SoCons>(n);
      if ( n->getTypeId().isDerivedFrom(SoLAr::getClassTypeId()) ) return Imp::clearAlternateRep<SoLAr>(n);
      //NOT IMPLEMENTED if ( n->getTypeId().isDerivedFrom(SoPcons::getClassTypeId()) ) return Imp::clearAlternateRep<SoPcons>(n);
      if ( n->getTypeId().isDerivedFrom(SoPcons::getClassTypeId()) ) return Imp::clearAlternateRep<SoPcons>(n);
      if ( n->getTypeId().isDerivedFrom(SoGenericBox::getClassTypeId()) ) return Imp::clearAlternateRep<SoGenericBox>(n);
      if ( n->getTypeId().isDerivedFrom(SoTubs::getClassTypeId()) ) return Imp::clearAlternateRep<SoTubs>(n);
      if ( n->getTypeId().isDerivedFrom(SoPolyhedron::getClassTypeId()) ) return Imp::clearAlternateRep<SoPolyhedron>(n);
      if ( n->getTypeId().isDerivedFrom(SoTessellated::getClassTypeId()) ) return Imp::clearAlternateRep<SoTessellated>(n);
    }
  }
}

//____________________________________________________________________
void VP1HEPVisUtils::initAllCustomClasses()
{
  SoCons::initClass();
  SoGenericBox::initClass();
  SoLAr::initClass();
  SoPcons::initClass();
  SoPolyhedron::initClass();
  SoTransparency::initClass();
  SoTubs::initClass();
  SoTessellated::initClass();
}
