/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class IVP12DSystem                   //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/IVP12DSystem.h"
#include "VP1Base/VP1GraphicsItemCollection.h"
#include <cassert>

//Default implementations that do not do anything:
void IVP12DSystem::itemPicked(VP1GraphicsItemCollection*,QGraphicsItem*) {}
void IVP12DSystem::itemGotEvent(VP1GraphicsItemCollection*,QGraphicsItem*,QEvent*) {}
void IVP12DSystem::selectionChanged(VP1GraphicsItemCollection*,QList<QGraphicsItem*>) {}

//____________________________________________________________________
class IVP12DSystem::Imp {
public:
  QSet<VP1GraphicsItemCollection*> collections;
};


//____________________________________________________________________
IVP12DSystem::IVP12DSystem(const QString & name, const QString & information, const QString & contact_info)
  : IVP1System(name,information,contact_info), m_d(new Imp)
{
}

//____________________________________________________________________
IVP12DSystem::~IVP12DSystem()
{
  delete m_d; m_d=0;
}

//____________________________________________________________________
void IVP12DSystem::registerItemCollection(VP1GraphicsItemCollection*ic)
{
  assert(!m_d->collections.contains(ic)
	 &&"IVP12DSystem::registerItemCollection Error: Item collection registered twice");
  m_d->collections<<ic;
  connect(ic,SIGNAL(itemPicked(QGraphicsItem*)),this,SLOT(itemPickedPrivate(QGraphicsItem*)));
  connect(ic,SIGNAL(itemGotEvent(QGraphicsItem*,QEvent*)),this,SLOT(itemGotEventPrivate(QGraphicsItem*,QEvent*)));
  connect(ic,SIGNAL(selectionChanged(QList<QGraphicsItem*>)),this,SLOT(selectionChangedPrivate(QList<QGraphicsItem*>)));
}

//____________________________________________________________________
QSet<VP1GraphicsItemCollection*> IVP12DSystem::getItemCollections()
{
  return m_d->collections;
}

//____________________________________________________________________
void IVP12DSystem::itemPickedPrivate(QGraphicsItem*item)
{
   VP1GraphicsItemCollection * ic = static_cast<VP1GraphicsItemCollection*>(sender());
   assert(ic&&ic->interactionMode()==VP1GraphicsItemCollection::SINGLEPICKS);
   itemPicked(ic,item);
   itemFromSystemSelected();
}

//____________________________________________________________________
void IVP12DSystem::itemGotEventPrivate(QGraphicsItem*item,QEvent*event)
{
   VP1GraphicsItemCollection * ic = static_cast<VP1GraphicsItemCollection*>(sender());
   assert(ic&&ic->interactionMode()==VP1GraphicsItemCollection::EVENTS);
   itemGotEvent(ic,item,event);
   itemFromSystemSelected();
}

//____________________________________________________________________
void IVP12DSystem::selectionChangedPrivate(QList<QGraphicsItem*> items)
{
   VP1GraphicsItemCollection * ic = static_cast<VP1GraphicsItemCollection*>(sender());
   assert(ic&&ic->interactionMode()==VP1GraphicsItemCollection::SELECTIONS);
   selectionChanged(ic,items);
   itemFromSystemSelected();
}
