/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class AssociatedObjectHandleBase        //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/AssociatedObjectHandleBase.h"
#include "VP1TrackSystems/TrackHandleBase.h"
#include "VP1TrackSystems/TrackCollHandleBase.h"
#include "VP1TrackSystems/TrackSysCommonData.h"
#include "VP1TrackSystems/AscObjSelectionManager.h"
#include "VP1TrackSystems/VP1TrackSystem.h"
#include "VP1Base/VP1Msg.h"

#include <Inventor/nodes/SoSeparator.h>

//____________________________________________________________________
class AssociatedObjectHandleBase::Imp {
public:
  static int nascobjs;
  Imp() : sep_simple(0),
	  sep_detailed(0),
	  attachhandle(0)
	  //,transform(0) 
	  {}
  SoSeparator * sep_simple;
  SoSeparator * sep_detailed;
  AssocObjAttachmentHandle * attachhandle;

  //SoTransform * transform;
  void detach();
  void attach(AssociatedObjectHandleBase * theclass );
  void ensureShapesBuild(AssociatedObjectHandleBase * theclass );
  void ensureShapesErased(AssociatedObjectHandleBase * theclass );
};

int AssociatedObjectHandleBase::Imp::nascobjs = 0;

//____________________________________________________________________
void AssociatedObjectHandleBase::Imp::ensureShapesBuild( AssociatedObjectHandleBase * theclass )
{
  if (sep_simple&&sep_detailed)
    return;
  if (!sep_simple&&!sep_detailed) {
    theclass->buildShapes(sep_simple, sep_detailed);
    if (!sep_simple&&!sep_detailed) {
      VP1Msg::messageDebug("AssociatedObjectHandleBase WARNING: Got null shapes! Replacing with simple points.");
      sep_simple = new SoSeparator;//Fixme: Shared point!!!
    }
    if (sep_simple)
      sep_simple->ref();
    if (sep_detailed)
      sep_detailed->ref();
  }
  if (!sep_detailed) {
    sep_detailed = sep_simple;
    sep_detailed->ref();
  }
  if (!sep_simple) {
    sep_simple = sep_detailed;
    sep_simple->ref();
  }

  theclass->registerShapes(sep_simple,sep_detailed);
}

//____________________________________________________________________
void AssociatedObjectHandleBase::Imp::ensureShapesErased(AssociatedObjectHandleBase * theclass )
{
  if (sep_simple||sep_detailed) {
    theclass->unregisterShapes(sep_simple,sep_detailed);
    if (sep_simple) {
      sep_simple->unref();
      sep_simple = 0;
    }
    if (sep_detailed) {
      sep_detailed->unref();
      sep_detailed = 0;
    }
  }
}

//____________________________________________________________________
void AssociatedObjectHandleBase::registerShapes(SoSeparator*simple,SoSeparator*detailed)
{
  m_trackHandle->collHandle()->common()->ascObjSelectionManager()->registerAscObj(simple,detailed,this);
}

//____________________________________________________________________
void AssociatedObjectHandleBase::unregisterShapes(SoSeparator*simple,SoSeparator*detailed)
{
  m_trackHandle->collHandle()->common()->ascObjSelectionManager()->unregisterAscObj(simple,detailed);
}

//____________________________________________________________________
TrackSysCommonData * AssociatedObjectHandleBase::common() const
{
  return m_trackHandle->common();
}

//____________________________________________________________________
AssociatedObjectHandleBase::AssociatedObjectHandleBase(TrackHandleBase*th)
  : m_d(new Imp),m_trackHandle(th), m_visible(false), m_pickStyle(UNPICKABLE/*better default?*/)
{
  ++Imp::nascobjs;
}

//____________________________________________________________________
AssociatedObjectHandleBase::~AssociatedObjectHandleBase()
{
  if (m_d->attachhandle&&m_visible&&m_d->sep_simple&&m_d->sep_detailed) {
    m_d->attachhandle->detachNodes(m_d->sep_simple,m_d->sep_detailed);
  }
  m_d->ensureShapesErased(this );
  delete m_d;
  --Imp::nascobjs;

}

//____________________________________________________________________
int AssociatedObjectHandleBase::numberOfInstances()
{
  return Imp::nascobjs;
}

//____________________________________________________________________
void AssociatedObjectHandleBase::setPickable( PICKSTYLE ps )
{
  if (m_pickStyle==ps)
    return;
  m_pickStyle=ps;

  //Fixme...

}

//____________________________________________________________________
void AssociatedObjectHandleBase::Imp::detach()
{
  if (!attachhandle)
    return;//Can't have been attached.
  attachhandle->detachNodes(sep_simple,sep_detailed);
}

//____________________________________________________________________
AssocObjAttachmentHandle * AssociatedObjectHandleBase::getAttachmentHandle()
{
  return m_trackHandle->getAttachmentHandle(regionIndex(), lodCrossOverValue());
}


//____________________________________________________________________
void AssociatedObjectHandleBase::Imp::attach(AssociatedObjectHandleBase * theclass )
{
  if (!attachhandle)
    attachhandle = theclass->getAttachmentHandle();
  attachhandle->attachNodes(sep_simple,sep_detailed,(theclass->pickStyle()!=UNPICKABLE));
}


//____________________________________________________________________
void AssociatedObjectHandleBase::setVisible( bool b )
{
  if (m_visible==b)
    return;
  m_visible=b;
  if (b) {
    m_d->ensureShapesBuild(this);
    m_d->attach(this);
  } else {
    m_d->detach();
  }
}

//____________________________________________________________________
void AssociatedObjectHandleBase::update3DObjects()
{
  VP1Msg::messageVerbose("AssociatedObjectHandleBase update3DObjects.");
  
  if (m_visible) {
    m_d->detach();
    m_d->ensureShapesErased(this);
    m_d->ensureShapesBuild(this);
    m_d->attach(this);
  } else {
    m_d->ensureShapesErased(this);
  }
}

//____________________________________________________________________
SoSeparator* AssociatedObjectHandleBase::shapeSimple() const
{
  return m_d->sep_simple;
}

//____________________________________________________________________
SoSeparator* AssociatedObjectHandleBase::shapeDetailed() const
{
  return m_d->sep_detailed;
}
