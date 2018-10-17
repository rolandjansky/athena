/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class IVP12DDetViewsSystem           //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: July 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/IVP12DDetViewsSystem.h"
#include "VP1Base/VP1GraphicsItemCollection.h"
#include <cassert>

void IVP12DDetViewsSystem::systemcreate(StoreGateSvc*) {}
QWidget * IVP12DDetViewsSystem::buildController() { return 0; }
void IVP12DDetViewsSystem::buildPermanentItemCollections(StoreGateSvc*,VP1GraphicsItemCollection*,
							 VP1GraphicsItemCollection*){}
void IVP12DDetViewsSystem::buildEventItemCollections(StoreGateSvc*,VP1GraphicsItemCollection*,
						     VP1GraphicsItemCollection*){}
void IVP12DDetViewsSystem::systemuncreate() {}
void IVP12DDetViewsSystem::systemerase() {}

//____________________________________________________________________
class IVP12DDetViewsSystem::Imp {
public:
  VP1GraphicsItemCollection* itemcollectionPermanent_xy;
  VP1GraphicsItemCollection* itemcollectionPermanent_rz;
  VP1GraphicsItemCollection* itemcollectionEvent_xy;
  VP1GraphicsItemCollection* itemcollectionEvent_rz;
  bool wasrefreshed;
  bool wascreated;
};

//____________________________________________________________________
IVP12DDetViewsSystem::IVP12DDetViewsSystem(const QString & name, const QString & information, const QString & contact_info)
 : IVP12DSystem(name,information,contact_info), m_d(new Imp)
{
  m_d->itemcollectionPermanent_xy = new VP1GraphicsItemCollection(this);
  m_d->itemcollectionPermanent_rz = new VP1GraphicsItemCollection(this);
  m_d->itemcollectionEvent_xy = new VP1GraphicsItemCollection(this);
  m_d->itemcollectionEvent_rz = new VP1GraphicsItemCollection(this);

  registerItemCollection(m_d->itemcollectionPermanent_xy);
  registerItemCollection(m_d->itemcollectionPermanent_rz);
  registerItemCollection(m_d->itemcollectionEvent_xy);
  registerItemCollection(m_d->itemcollectionEvent_rz);

  m_d->wasrefreshed=false;
  m_d->wascreated=false;
}

//____________________________________________________________________
IVP12DDetViewsSystem::~IVP12DDetViewsSystem()
{
  delete m_d; m_d=0;
}

//____________________________________________________________________
void IVP12DDetViewsSystem::create(StoreGateSvc* detstore)
{
  assert(!m_d->wascreated);
  systemcreate(detstore);

  QWidget * controller = buildController();
  if (controller)
    registerController(controller);

  bool hasview_xy = m_d->itemcollectionPermanent_xy->isAttachedToView();
  if (hasview_xy)
    m_d->itemcollectionPermanent_xy->detachFromView();
  bool hasview_rz = m_d->itemcollectionPermanent_rz->isAttachedToView();
  if (hasview_rz)
    m_d->itemcollectionPermanent_rz->detachFromView();

  buildPermanentItemCollections(detstore,
				m_d->itemcollectionPermanent_xy,
				m_d->itemcollectionPermanent_rz );

  if (hasview_xy)
    m_d->itemcollectionPermanent_xy->reattachToView();
  if (hasview_rz)
    m_d->itemcollectionPermanent_rz->reattachToView();

  m_d->wascreated=true;
}

//____________________________________________________________________
void IVP12DDetViewsSystem::refresh(StoreGateSvc* sg)
{
  assert(m_d->wascreated);
  assert(!m_d->wasrefreshed);

  bool hasview_xy = m_d->itemcollectionEvent_xy->isAttachedToView();
  if (hasview_xy)
    m_d->itemcollectionEvent_xy->detachFromView();
  updateGUI();
  bool hasview_rz = m_d->itemcollectionEvent_rz->isAttachedToView();
  if (hasview_rz)
    m_d->itemcollectionEvent_rz->detachFromView();
  updateGUI();

  buildEventItemCollections( sg,
			     m_d->itemcollectionEvent_xy,
			     m_d->itemcollectionEvent_rz );

  updateGUI();
  if (hasview_xy)
    m_d->itemcollectionEvent_xy->reattachToView();
  updateGUI();
  if (hasview_rz)
    m_d->itemcollectionEvent_rz->reattachToView();

  m_d->wasrefreshed=true;
}

//____________________________________________________________________
void IVP12DDetViewsSystem::erase()
{
  assert(m_d->wascreated);
  assert(m_d->wasrefreshed);
  systemerase();
  m_d->itemcollectionEvent_xy->clear();
  m_d->itemcollectionEvent_rz->clear();
  m_d->wasrefreshed=false;
}

//____________________________________________________________________
void IVP12DDetViewsSystem::uncreate()
{
	messageDebug("uncreate()...");

  assert(m_d->wascreated);
  assert(!m_d->wasrefreshed);

  if (m_d->itemcollectionEvent_xy->isAttachedToView())
    m_d->itemcollectionEvent_xy->detachFromView();
  if (m_d->itemcollectionEvent_rz->isAttachedToView())
    m_d->itemcollectionEvent_rz->detachFromView();

  if (m_d->itemcollectionPermanent_xy->isAttachedToView())
    m_d->itemcollectionPermanent_xy->detachFromView();
  if (m_d->itemcollectionPermanent_rz->isAttachedToView())
    m_d->itemcollectionPermanent_rz->detachFromView();

  systemuncreate();
  m_d->itemcollectionPermanent_xy->clear();
  m_d->itemcollectionPermanent_rz->clear();
  m_d->wascreated=false;
}

//____________________________________________________________________
QSet<VP1GraphicsItemCollection*> IVP12DDetViewsSystem::getItemCollections_XY() const
{
  QSet<VP1GraphicsItemCollection*> l;
  l << m_d->itemcollectionPermanent_xy;
  l << m_d->itemcollectionEvent_xy;
  return l;
}

//____________________________________________________________________
QSet<VP1GraphicsItemCollection*> IVP12DDetViewsSystem::getItemCollections_RZ() const
{
  QSet<VP1GraphicsItemCollection*> l;
  l << m_d->itemcollectionPermanent_rz;
  l << m_d->itemcollectionEvent_rz;
  return l;
}
