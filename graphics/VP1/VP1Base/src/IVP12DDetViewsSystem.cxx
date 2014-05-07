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
 : IVP12DSystem(name,information,contact_info), d(new Imp)
{
  d->itemcollectionPermanent_xy = new VP1GraphicsItemCollection(this);
  d->itemcollectionPermanent_rz = new VP1GraphicsItemCollection(this);
  d->itemcollectionEvent_xy = new VP1GraphicsItemCollection(this);
  d->itemcollectionEvent_rz = new VP1GraphicsItemCollection(this);

  registerItemCollection(d->itemcollectionPermanent_xy);
  registerItemCollection(d->itemcollectionPermanent_rz);
  registerItemCollection(d->itemcollectionEvent_xy);
  registerItemCollection(d->itemcollectionEvent_rz);

  d->wasrefreshed=false;
  d->wascreated=false;
}

//____________________________________________________________________
IVP12DDetViewsSystem::~IVP12DDetViewsSystem()
{
  delete d; d=0;
}

//____________________________________________________________________
void IVP12DDetViewsSystem::create(StoreGateSvc* detstore)
{
  assert(!d->wascreated);
  systemcreate(detstore);

  QWidget * controller = buildController();
  if (controller)
    registerController(controller);

  bool hasview_xy = d->itemcollectionPermanent_xy->isAttachedToView();
  if (hasview_xy)
    d->itemcollectionPermanent_xy->detachFromView();
  bool hasview_rz = d->itemcollectionPermanent_rz->isAttachedToView();
  if (hasview_rz)
    d->itemcollectionPermanent_rz->detachFromView();

  buildPermanentItemCollections(detstore,
				d->itemcollectionPermanent_xy,
				d->itemcollectionPermanent_rz );

  if (hasview_xy)
    d->itemcollectionPermanent_xy->reattachToView();
  if (hasview_rz)
    d->itemcollectionPermanent_rz->reattachToView();

  d->wascreated=true;
}

//____________________________________________________________________
void IVP12DDetViewsSystem::refresh(StoreGateSvc* sg)
{
  assert(d->wascreated);
  assert(!d->wasrefreshed);

  bool hasview_xy = d->itemcollectionEvent_xy->isAttachedToView();
  if (hasview_xy)
    d->itemcollectionEvent_xy->detachFromView();
  updateGUI();
  bool hasview_rz = d->itemcollectionEvent_rz->isAttachedToView();
  if (hasview_rz)
    d->itemcollectionEvent_rz->detachFromView();
  updateGUI();

  buildEventItemCollections( sg,
			     d->itemcollectionEvent_xy,
			     d->itemcollectionEvent_rz );

  updateGUI();
  if (hasview_xy)
    d->itemcollectionEvent_xy->reattachToView();
  updateGUI();
  if (hasview_rz)
    d->itemcollectionEvent_rz->reattachToView();

  d->wasrefreshed=true;
}

//____________________________________________________________________
void IVP12DDetViewsSystem::erase()
{
  assert(d->wascreated);
  assert(d->wasrefreshed);
  systemerase();
  d->itemcollectionEvent_xy->clear();
  d->itemcollectionEvent_rz->clear();
  d->wasrefreshed=false;
}

//____________________________________________________________________
void IVP12DDetViewsSystem::uncreate()
{
	messageDebug("uncreate()...");

  assert(d->wascreated);
  assert(!d->wasrefreshed);

  if (d->itemcollectionEvent_xy->isAttachedToView())
    d->itemcollectionEvent_xy->detachFromView();
  if (d->itemcollectionEvent_rz->isAttachedToView())
    d->itemcollectionEvent_rz->detachFromView();

  if (d->itemcollectionPermanent_xy->isAttachedToView())
    d->itemcollectionPermanent_xy->detachFromView();
  if (d->itemcollectionPermanent_rz->isAttachedToView())
    d->itemcollectionPermanent_rz->detachFromView();

  systemuncreate();
  d->itemcollectionPermanent_xy->clear();
  d->itemcollectionPermanent_rz->clear();
  d->wascreated=false;
}

//____________________________________________________________________
QSet<VP1GraphicsItemCollection*> IVP12DDetViewsSystem::getItemCollections_XY() const
{
  QSet<VP1GraphicsItemCollection*> l;
  l << d->itemcollectionPermanent_xy;
  l << d->itemcollectionEvent_xy;
  return l;
}

//____________________________________________________________________
QSet<VP1GraphicsItemCollection*> IVP12DDetViewsSystem::getItemCollections_RZ() const
{
  QSet<VP1GraphicsItemCollection*> l;
  l << d->itemcollectionPermanent_rz;
  l << d->itemcollectionEvent_rz;
  return l;
}
