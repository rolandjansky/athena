/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class IVP12DSystemSimple             //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/IVP12DSystemSimple.h"
#include "VP1Base/VP1GraphicsItemCollection.h"
#include <cassert>

void IVP12DSystemSimple::systemcreate(StoreGateSvc*) {}
QWidget * IVP12DSystemSimple::buildController() { return 0; }
void IVP12DSystemSimple::buildPermanentItemCollection(StoreGateSvc*,VP1GraphicsItemCollection*){}
void IVP12DSystemSimple::buildEventItemCollection(StoreGateSvc *, VP1GraphicsItemCollection*) {}
void IVP12DSystemSimple::systemuncreate() {}
void IVP12DSystemSimple::systemerase() {}

//____________________________________________________________________
class IVP12DSystemSimple::Imp {
public:
  VP1GraphicsItemCollection* itemcollectionPermanent;
  VP1GraphicsItemCollection* itemcollectionEvent;
  bool wasrefreshed;
  bool wascreated;
};

//____________________________________________________________________
IVP12DSystemSimple::IVP12DSystemSimple(const QString & name, const QString & information, const QString & contact_info)
 : IVP12DSystem(name,information,contact_info), d(new Imp)
{
  d->itemcollectionPermanent = new VP1GraphicsItemCollection(this);
  d->itemcollectionEvent = new VP1GraphicsItemCollection(this);

  registerItemCollection(d->itemcollectionPermanent);
  registerItemCollection(d->itemcollectionEvent);

  d->wasrefreshed=false;
  d->wascreated=false;
}

//____________________________________________________________________
IVP12DSystemSimple::~IVP12DSystemSimple()
{
  delete d; d=0;
}

//____________________________________________________________________
void IVP12DSystemSimple::create(StoreGateSvc* detstore)
{
  assert(!d->wascreated);
  systemcreate(detstore);

  QWidget * controller = buildController();
  if (controller)
    registerController(controller);

  bool hasview = d->itemcollectionPermanent->isAttachedToView();
  if (hasview)
    d->itemcollectionPermanent->detachFromView();
  buildPermanentItemCollection(detstore,d->itemcollectionPermanent);
  if (hasview)
    d->itemcollectionPermanent->reattachToView();
  d->wascreated=true;
}

//____________________________________________________________________
void IVP12DSystemSimple::refresh(StoreGateSvc* sg)
{
  assert(d->wascreated);
  assert(!d->wasrefreshed);

  bool hasview = d->itemcollectionEvent->isAttachedToView();
  if (hasview)
    d->itemcollectionEvent->detachFromView();
  updateGUI();
  buildEventItemCollection(sg, d->itemcollectionEvent);
  updateGUI();
  if (hasview)
    d->itemcollectionEvent->reattachToView();
  updateGUI();

  d->wasrefreshed=true;
}

//____________________________________________________________________
void IVP12DSystemSimple::erase()
{
  assert(d->wascreated);
  assert(d->wasrefreshed);
  systemerase();
  d->itemcollectionEvent->clear();
  d->wasrefreshed=false;
}

//____________________________________________________________________
void IVP12DSystemSimple::uncreate()
{
	messageDebug("uncreate()...");

  assert(d->wascreated);
  assert(!d->wasrefreshed);
  if (d->itemcollectionEvent->isAttachedToView())
    d->itemcollectionEvent->detachFromView();
  if (d->itemcollectionPermanent->isAttachedToView())
    d->itemcollectionPermanent->detachFromView();
  systemuncreate();
  d->itemcollectionPermanent->clear();
  d->wascreated=false;
}
