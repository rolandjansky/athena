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
 : QObject(parent), d(new Imp)
{
  d->view=0;
  d->lastview=0;
  d->scene=0;
  d->ignoreall_int=false;
  d->ignoreall_move=false;
  d->interactionmode=INERT;
  d->nactiveitems=0;
}

//____________________________________________________________________
VP1GraphicsItemCollection::~VP1GraphicsItemCollection()
{
  if (d->view)
    detachFromView();
  clear();
  delete d;
}

//____________________________________________________________________
void VP1GraphicsItemCollection::setInteractionMode( const VP1GraphicsItemCollection::INTERACTIONMODE& im )
{
  assert(!d->scene&&"VP1GraphicsItemCollection::setInteractionMode can not be called while a scene is attached.");
  if (d->scene) {
    std::cout<<"VP1GraphicsItemCollection::setInteractionMode can not be called while a scene is attached."<<std::endl;
    return;
  }
  d->interactionmode = im;
}

//____________________________________________________________________
VP1GraphicsItemCollection::INTERACTIONMODE VP1GraphicsItemCollection::interactionMode() const
{
  return d->interactionmode;
}

//____________________________________________________________________
void VP1GraphicsItemCollection::addItem(QGraphicsItem*item, const bool& active,const bool&movable)
{
  assert(!d->items2active.contains(item)&&"Please do not add the same item twice");
  assert(!item->flags()&&"Please do not add any flags to your items!!");

  if (d->scene) {
    //The following must postponed if there is no scene:
    d->scene->addItem(item);
    item->setFlag(QGraphicsItem::ItemIsMovable,(movable&&(!d->ignoreall_move)));
  }
  assert(!d->items2active.contains(item));
  d->items2active.insert(item,active);
  if (active)
    ++d->nactiveitems;
  if (movable)
    d->movableitems<<item;
}

//Fixme: Make VP1GraphicsView setScene() private so it is the same throughout a GV lifetime.

//____________________________________________________________________
bool VP1GraphicsItemCollection::removeItem(QGraphicsItem*item)
{
  Imp::ItemDataMapItr it = d->items2active.find( item );
  if (it==d->items2active.end())
    return false;
  if (it.value())
    --d->nactiveitems;
  if (d->scene) {
    d->scene->removeItem(item);
  }
  d->items2active.erase(it);
  assert(!d->items2active.contains(item));

  if (!d->movableitems.empty()) {
    QSet<QGraphicsItem*>::iterator it2 = d->movableitems.find(item);
    if (it2!=d->movableitems.end()) {
      assert(d->movableitems.contains(item));
      d->movableitems.erase(it2);
    }
  }
  assert(!d->movableitems.contains(item));
  return true;
}

//____________________________________________________________________
bool VP1GraphicsItemCollection::setMovable(QGraphicsItem* item, const bool& movable)
{
  Imp::ItemDataMapItr it = d->items2active.find( item );
  if (it==d->items2active.end())
    return false;
  if (movable==d->movableitems.contains(item))//Nothing needs to be done:
    return true;
  if (movable) {
    assert(!d->movableitems.contains(item));
    d->movableitems<<item;
  } else {
    assert(d->movableitems.contains(item));
    d->movableitems.remove(item);
    assert(!d->movableitems.contains(item));
  }
  if (!d->ignoreall_move)
    item->setFlag(QGraphicsItem::ItemIsMovable,movable);
  return true;
}

//____________________________________________________________________
bool VP1GraphicsItemCollection::setActive(QGraphicsItem* item, const bool& active)
{
  Imp::ItemDataMapItr it = d->items2active.find( item );
  if (it==d->items2active.end())
    return false;
  it.value()=active;
  if (active)
    ++d->nactiveitems;
  else
    --d->nactiveitems;
  return true;
}

//____________________________________________________________________
void VP1GraphicsItemCollection::clear(const bool& deleteitems)
{
  //Clear selections before deleting items in order to only get one selectionchanged signal.
  if (d->view)
    d->view->clearSelections();

  Imp::ItemDataMapItr it,itE=d->items2active.end();
  if (d->scene) {
    //Remove item from scene and possible also remove event filter.
    for (it=d->items2active.begin();it!=itE;++it)
      d->scene->removeItem(it.key());
  }
  if (deleteitems) {
    //Delete the items:
    for (it=d->items2active.begin();it!=itE;++it) {
      delete it.key();
    }
  }
  d->items2active.clear();
  d->movableitems.clear();
  d->nactiveitems=0;
}

//____________________________________________________________________
bool VP1GraphicsItemCollection::hasItem(QGraphicsItem* item) const
{
  return d->items2active.contains(item);
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
  if (d->ignoreall_int==b)
    return;
  d->ignoreall_int=b;
}

//____________________________________________________________________
void VP1GraphicsItemCollection::setTemporaryIgnoreMovable( const bool& b )
{
  if (d->ignoreall_move==b)
    return;
  d->ignoreall_move=b;
  if (d->scene)
    d->setEnabledMovableItems(!b);
}

//____________________________________________________________________
bool VP1GraphicsItemCollection::Imp::ignoresInteractions() const
{
  return ( interactionmode==INERT || ignoreall_int );
}

//____________________________________________________________________
void VP1GraphicsItemCollection::attachToView(VP1GraphicsView*view)
{
  assert(!d->scene);
  assert(!d->view);
  d->scene=view->scene();
  //  d->lastscene=d->scene;
  d->view=view;
  d->lastview=d->view;
  Imp::ItemDataMapItr it,itE=d->items2active.end();
  for (it=d->items2active.begin();it!=itE;++it) {
    d->scene->addItem(it.key());
  }
  //Update this, since we didnt do any updates while there was no scene:
  d->setEnabledMovableItems(!d->ignoreall_move);
}

//____________________________________________________________________
void VP1GraphicsItemCollection::detachFromView()
{
  if (!d->view)
    return;
  d->view->removeItemCollection(this);
  //NB. The call to removeItemCollection ends up by calling
  //VP1GraphicsItemCollection::real_detachFromView().
}

//____________________________________________________________________
void VP1GraphicsItemCollection::real_detachFromView()
{
  //This method is called after the collection has been removed from the the view.
  if (!d->view)
    return;
  //Remove item from scene:
  Imp::ItemDataMapItr it,itE=d->items2active.end();
  for (it=d->items2active.begin();it!=itE;++it) {
    d->scene->removeItem(it.key());
  }
  d->view=0;
  d->scene=0;
}

//____________________________________________________________________
void VP1GraphicsItemCollection::reattachToView()
{
  if (d->view)
    return;
  assert(d->lastview);
  d->lastview->addItemCollection(this);
  //NB. The call to addItemCollection ends up by calling
  //VP1GraphicsItemCollection::attachToView().
}

//____________________________________________________________________
bool VP1GraphicsItemCollection::isAttachedToView()
{
  return d->view;
}


//____________________________________________________________________
int VP1GraphicsItemCollection::nItems() const
{
  return d->items2active.count();
}

//____________________________________________________________________
int VP1GraphicsItemCollection::nActiveItems() const
{
  return d->nactiveitems;
}

//____________________________________________________________________
int VP1GraphicsItemCollection::nMovableItems() const
{
  return d->movableitems.count();
}

//____________________________________________________________________
int VP1GraphicsItemCollection::nPresentlyActiveItems() const
{
  return d->ignoresInteractions() ? 0 : d->nactiveitems;
}

//____________________________________________________________________
int VP1GraphicsItemCollection::nPresentlyMovableItems() const
{
  return d->ignoresInteractions() ? 0 : d->movableitems.count();
}

//____________________________________________________________________
bool VP1GraphicsItemCollection::itemBelongsAndIsPresentlyActive(QGraphicsItem*item) const
{
  if (!nPresentlyActiveItems())
    return false;
  QHash<QGraphicsItem*,bool>::const_iterator it = d->items2active.find(item);


  if (it==d->items2active.constEnd())
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
