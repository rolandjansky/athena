/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1GraphicsItemCollection      //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/VP1GraphicsItemCollection.h"
#include "VP1Base/VP1GraphicsView.h"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QHash>
#include <QSet>

#include <iostream>
#include <cassert>

//____________________________________________________________________
class VP1GraphicsItemCollection::Imp {
public:

  QGraphicsScene *scene;
  VP1GraphicsView *lastview;
  VP1GraphicsView *view;
  bool ignoreall_int;
  bool ignoreall_move;
  int nactiveitems;

  void setEnabledMovableItems(const bool& enabled);
  bool ignoresInteractions() const;


  QHash<QGraphicsItem*,bool> items2active;
  typedef QHash<QGraphicsItem*,bool>::iterator ItemDataMapItr;//fixme: optimise flag.

  //For performance reasons, we keep a separate list of the items that
  //are movable (anyone making a collection with 100000 movable items
  //will be the first against the wall when the revolution comes):
  QSet<QGraphicsItem*> movableitems;

  VP1GraphicsItemCollection::INTERACTIONMODE interactionmode;

};


//____________________________________________________________________
VP1GraphicsItemCollection::VP1GraphicsItemCollection( QObject * parent)
 : QObject(parent), m_d(new Imp)
{
  m_d->view=0;
  m_d->lastview=0;
  m_d->scene=0;
  m_d->ignoreall_int=false;
  m_d->ignoreall_move=false;
  m_d->interactionmode=INERT;
  m_d->nactiveitems=0;
}

//____________________________________________________________________
VP1GraphicsItemCollection::~VP1GraphicsItemCollection()
{
  if (m_d->view)
    detachFromView();
  clear();
  delete m_d;
}

//____________________________________________________________________
void VP1GraphicsItemCollection::setInteractionMode( const VP1GraphicsItemCollection::INTERACTIONMODE& im )
{
  assert(!m_d->scene&&"VP1GraphicsItemCollection::setInteractionMode can not be called while a scene is attached.");
  if (m_d->scene) {
    std::cout<<"VP1GraphicsItemCollection::setInteractionMode can not be called while a scene is attached."<<std::endl;
    return;
  }
  m_d->interactionmode = im;
}

//____________________________________________________________________
VP1GraphicsItemCollection::INTERACTIONMODE VP1GraphicsItemCollection::interactionMode() const
{
  return m_d->interactionmode;
}

//____________________________________________________________________
void VP1GraphicsItemCollection::addItem(QGraphicsItem*item, const bool& active,const bool&movable)
{
  assert(!m_d->items2active.contains(item)&&"Please do not add the same item twice");
  assert(!item->flags()&&"Please do not add any flags to your items!!");

  if (m_d->scene) {
    //The following must postponed if there is no scene:
    m_d->scene->addItem(item);
    item->setFlag(QGraphicsItem::ItemIsMovable,(movable&&(!m_d->ignoreall_move)));
  }
  assert(!m_d->items2active.contains(item));
  m_d->items2active.insert(item,active);
  if (active)
    ++m_d->nactiveitems;
  if (movable)
    m_d->movableitems<<item;
}

//Fixme: Make VP1GraphicsView setScene() private so it is the same throughout a GV lifetime.

//____________________________________________________________________
bool VP1GraphicsItemCollection::removeItem(QGraphicsItem*item)
{
  Imp::ItemDataMapItr it = m_d->items2active.find( item );
  if (it==m_d->items2active.end())
    return false;
  if (it.value())
    --m_d->nactiveitems;
  if (m_d->scene) {
    m_d->scene->removeItem(item);
  }
  m_d->items2active.erase(it);
  assert(!m_d->items2active.contains(item));

  if (!m_d->movableitems.empty()) {
    QSet<QGraphicsItem*>::iterator it2 = m_d->movableitems.find(item);
    if (it2!=m_d->movableitems.end()) {
      assert(m_d->movableitems.contains(item));
      m_d->movableitems.erase(it2);
    }
  }
  assert(!m_d->movableitems.contains(item));
  return true;
}

//____________________________________________________________________
bool VP1GraphicsItemCollection::setMovable(QGraphicsItem* item, const bool& movable)
{
  Imp::ItemDataMapItr it = m_d->items2active.find( item );
  if (it==m_d->items2active.end())
    return false;
  if (movable==m_d->movableitems.contains(item))//Nothing needs to be done:
    return true;
  if (movable) {
    assert(!m_d->movableitems.contains(item));
    m_d->movableitems<<item;
  } else {
    assert(m_d->movableitems.contains(item));
    m_d->movableitems.remove(item);
    assert(!m_d->movableitems.contains(item));
  }
  if (!m_d->ignoreall_move)
    item->setFlag(QGraphicsItem::ItemIsMovable,movable);
  return true;
}

//____________________________________________________________________
bool VP1GraphicsItemCollection::setActive(QGraphicsItem* item, const bool& active)
{
  Imp::ItemDataMapItr it = m_d->items2active.find( item );
  if (it==m_d->items2active.end())
    return false;
  it.value()=active;
  if (active)
    ++m_d->nactiveitems;
  else
    --m_d->nactiveitems;
  return true;
}

//____________________________________________________________________
void VP1GraphicsItemCollection::clear(const bool& deleteitems)
{
  //Clear selections before deleting items in order to only get one selectionchanged signal.
  if (m_d->view)
    m_d->view->clearSelections();

  Imp::ItemDataMapItr it,itE=m_d->items2active.end();
  if (m_d->scene) {
    //Remove item from scene and possible also remove event filter.
    for (it=m_d->items2active.begin();it!=itE;++it)
      m_d->scene->removeItem(it.key());
  }
  if (deleteitems) {
    //Delete the items:
    for (it=m_d->items2active.begin();it!=itE;++it) {
      delete it.key();
    }
  }
  m_d->items2active.clear();
  m_d->movableitems.clear();
  m_d->nactiveitems=0;
}

//____________________________________________________________________
bool VP1GraphicsItemCollection::hasItem(QGraphicsItem* item) const
{
  return m_d->items2active.contains(item);
}

//____________________________________________________________________
void VP1GraphicsItemCollection::Imp::setEnabledMovableItems(const bool& enabled) {
  if (movableitems.empty())
    return;
  QSet<QGraphicsItem*>::iterator it,itE=movableitems.end();
  for (it=movableitems.begin();it!=itE;++it) {
    (*it)->setFlag(QGraphicsItem::ItemIsMovable,enabled);
  }

}

//____________________________________________________________________
void VP1GraphicsItemCollection::setTemporaryIgnoreInteractions( const bool& b )
{
  if (m_d->ignoreall_int==b)
    return;
  m_d->ignoreall_int=b;
}

//____________________________________________________________________
void VP1GraphicsItemCollection::setTemporaryIgnoreMovable( const bool& b )
{
  if (m_d->ignoreall_move==b)
    return;
  m_d->ignoreall_move=b;
  if (m_d->scene)
    m_d->setEnabledMovableItems(!b);
}

//____________________________________________________________________
bool VP1GraphicsItemCollection::Imp::ignoresInteractions() const
{
  return ( interactionmode==INERT || ignoreall_int );
}

//____________________________________________________________________
void VP1GraphicsItemCollection::attachToView(VP1GraphicsView*view)
{
  assert(!m_d->scene);
  assert(!m_d->view);
  m_d->scene=view->scene();
  //  m_d->lastscene=m_d->scene;
  m_d->view=view;
  m_d->lastview=m_d->view;
  Imp::ItemDataMapItr it,itE=m_d->items2active.end();
  for (it=m_d->items2active.begin();it!=itE;++it) {
    m_d->scene->addItem(it.key());
  }
  //Update this, since we didnt do any updates while there was no scene:
  m_d->setEnabledMovableItems(!m_d->ignoreall_move);
}

//____________________________________________________________________
void VP1GraphicsItemCollection::detachFromView()
{
  if (!m_d->view)
    return;
  m_d->view->removeItemCollection(this);
  //NB. The call to removeItemCollection ends up by calling
  //VP1GraphicsItemCollection::real_detachFromView().
}

//____________________________________________________________________
void VP1GraphicsItemCollection::real_detachFromView()
{
  //This method is called after the collection has been removed from the the view.
  if (!m_d->view)
    return;
  //Remove item from scene:
  Imp::ItemDataMapItr it,itE=m_d->items2active.end();
  for (it=m_d->items2active.begin();it!=itE;++it) {
    m_d->scene->removeItem(it.key());
  }
  m_d->view=0;
  m_d->scene=0;
}

//____________________________________________________________________
void VP1GraphicsItemCollection::reattachToView()
{
  if (m_d->view)
    return;
  assert(m_d->lastview);
  m_d->lastview->addItemCollection(this);
  //NB. The call to addItemCollection ends up by calling
  //VP1GraphicsItemCollection::attachToView().
}

//____________________________________________________________________
bool VP1GraphicsItemCollection::isAttachedToView()
{
  return m_d->view;
}


//____________________________________________________________________
int VP1GraphicsItemCollection::nItems() const
{
  return m_d->items2active.count();
}

//____________________________________________________________________
int VP1GraphicsItemCollection::nActiveItems() const
{
  return m_d->nactiveitems;
}

//____________________________________________________________________
int VP1GraphicsItemCollection::nMovableItems() const
{
  return m_d->movableitems.count();
}

//____________________________________________________________________
int VP1GraphicsItemCollection::nPresentlyActiveItems() const
{
  return m_d->ignoresInteractions() ? 0 : m_d->nactiveitems;
}

//____________________________________________________________________
int VP1GraphicsItemCollection::nPresentlyMovableItems() const
{
  return m_d->ignoresInteractions() ? 0 : m_d->movableitems.count();
}

//____________________________________________________________________
bool VP1GraphicsItemCollection::itemBelongsAndIsPresentlyActive(QGraphicsItem*item) const
{
  if (!nPresentlyActiveItems())
    return false;
  QHash<QGraphicsItem*,bool>::const_iterator it = m_d->items2active.find(item);


  if (it==m_d->items2active.constEnd())
    return false;
  else
    return it.value();
}

//____________________________________________________________________
void VP1GraphicsItemCollection::itemPickedPrivate(QGraphicsItem* item) const
{
  itemPicked(item);
}

//____________________________________________________________________
void VP1GraphicsItemCollection::itemGotEventPrivate(QGraphicsItem*item,QEvent*event) const
{
  itemGotEvent(item,event);
}

//____________________________________________________________________
void VP1GraphicsItemCollection::selectionChangedPrivate(QList<QGraphicsItem*> items) const
{
  selectionChanged(items);
}
