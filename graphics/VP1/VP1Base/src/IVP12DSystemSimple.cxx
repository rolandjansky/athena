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
 : IVP12DSystem(name,information,contact_info), m_d(new Imp)
{
  m_d->itemcollectionPermanent = new VP1GraphicsItemCollection(this);
  m_d->itemcollectionEvent = new VP1GraphicsItemCollection(this);

  registerItemCollection(m_d->itemcollectionPermanent);
  registerItemCollection(m_d->itemcollectionEvent);

  m_d->wasrefreshed=false;
  m_d->wascreated=false;
}

//____________________________________________________________________
IVP12DSystemSimple::~IVP12DSystemSimple()
{
  delete m_d; m_d=0;
}

//____________________________________________________________________
void IVP12DSystemSimple::create(StoreGateSvc* detstore)
{
  assert(!m_d->wascreated);
  systemcreate(detstore);

  QWidget * controller = buildController();
  if (controller)
    registerController(controller);

  bool hasview = m_d->itemcollectionPermanent->isAttachedToView();
  if (hasview)
    m_d->itemcollectionPermanent->detachFromView();
  buildPermanentItemCollection(detstore,m_d->itemcollectionPermanent);
  if (hasview)
    m_d->itemcollectionPermanent->reattachToView();
  m_d->wascreated=true;
}

//____________________________________________________________________
void IVP12DSystemSimple::refresh(StoreGateSvc* sg)
{
  assert(m_d->wascreated);
  assert(!m_d->wasrefreshed);

  bool hasview = m_d->itemcollectionEvent->isAttachedToView();
  if (hasview)
    m_d->itemcollectionEvent->detachFromView();
  updateGUI();
  buildEventItemCollection(sg, m_d->itemcollectionEvent);
  updateGUI();
  if (hasview)
    m_d->itemcollectionEvent->reattachToView();
  updateGUI();

  m_d->wasrefreshed=true;
}

//____________________________________________________________________
void IVP12DSystemSimple::erase()
{
  assert(m_d->wascreated);
  assert(m_d->wasrefreshed);
  systemerase();
  m_d->itemcollectionEvent->clear();
  m_d->wasrefreshed=false;
}

//____________________________________________________________________
void IVP12DSystemSimple::uncreate()
{
	messageDebug("uncreate()...");

  assert(m_d->wascreated);
  assert(!m_d->wasrefreshed);
  if (m_d->itemcollectionEvent->isAttachedToView())
    m_d->itemcollectionEvent->detachFromView();
  if (m_d->itemcollectionPermanent->isAttachedToView())
    m_d->itemcollectionPermanent->detachFromView();
  systemuncreate();
  m_d->itemcollectionPermanent->clear();
  m_d->wascreated=false;
}
