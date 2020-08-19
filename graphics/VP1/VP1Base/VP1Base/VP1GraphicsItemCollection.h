/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1GraphicsItemCollection        //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1GRAPHICSITEMCOLLECTION_H
#define VP1GRAPHICSITEMCOLLECTION_H

//Policy: It is forbidden for systems/channels to install
//        eventhandlers or to set flags on QGraphicsItems!!!!!
//
//  Each item is marked as either ACTIVE or PASSIVE [+movable]
//  (internally an ic knows whether it has any active items at all of
//  course). An IC can be in exactly one of the following modes,
//  governing how its active items respond (this state is set at
//  initialisation by the system and is not changeable later - if so,
//  warn if INACTIVE system has active elements and vice versa?):
//
//  INERT, SINGLEPICKS, SINGLEEVENTS, SELECTIONS (+UNSET)
//
//  It must be set BEFORE a scene is attached for the first time.
//
//  Furthermore, each IC has a temporary ignore state, which makes it
//  temporarily work (i.e. respond to questions such as "is this item
//  presently active") just as if it was inert
//
//
//  In graphicsview: Any click or event gets passed to the topmost
//                   active item in a non-inert system.
//
//  If there was a previous selection:
//      If the new item is not in the same IC as the previous selection: clear selection.
//      Else: If shift was down: Add to selection. Else: Set selection to the new item.
//
//  Future: Special selection tool which lets you draw a path and select anything inside it (needs to know which system it is selecting from)
//
// Any change of selection or occurrence of picks/events makes the GV ask the appropriate IC to emit a signal.

// FIXME: need to include this before any QT header, because QT defines
// a macro `foreach' that collides with an identifier in AthenaKernel.
#include "xAODCore/BaseInfo.h"

#include <QObject>

class VP1GraphicsView;
class QGraphicsItem;

class VP1GraphicsItemCollection : public QObject {

  Q_OBJECT

public:

  VP1GraphicsItemCollection( QObject * parent=0 );
  virtual ~VP1GraphicsItemCollection();

  enum INTERACTIONMODE { INERT, SINGLEPICKS, EVENTS, SELECTIONS };
  void setInteractionMode( const INTERACTIONMODE& );  //Can only be called while not attached to a view.
                                                      //Default is INERT.
  INTERACTIONMODE interactionMode() const;

  void addItem( QGraphicsItem*, const bool& active = false, const bool&movable=false );
  //Beware: All items added will be owned by this class - but if you call removeItem below, you will own them again.

  //The next methods returns false if the item has not been previously added to the collection:
  bool setActive(QGraphicsItem*, const bool& active = true);
  bool setMovable(QGraphicsItem* item, const bool& movable=true);
  bool removeItem(QGraphicsItem*);//Does not delete item.
  bool hasItem(QGraphicsItem*) const;

  void clear(const bool& deleteitems = true);//Removes and possibly deletes all items.

  //These two can be called from e.g. the systems to avoid updates during intensive work:
  void detachFromView();//If was not attached, do nothing
  void reattachToView();//Only does something if it has been previously attached.
  bool isAttachedToView();

  //These three gives the same answer irrespective of the temporary ignore state
  int nItems() const;
  int nActiveItems() const;
  int nMovableItems() const;
  //These two always returns 0 if there is a temporary ignore state:
  int nPresentlyActiveItems() const;
  int nPresentlyMovableItems() const;

  bool itemBelongsAndIsPresentlyActive(QGraphicsItem*) const;

signals:
  void itemPicked(QGraphicsItem*) const;//Emitted for active items when in SINGLEPICK mode.
  void itemGotEvent(QGraphicsItem*,QEvent*) const;//Emitted for active items when in EVENTS mode.
  void selectionChanged(QList<QGraphicsItem*>) const;//Emitted for active items when in SELECTIONS mode.

private:
  VP1GraphicsItemCollection( const VP1GraphicsItemCollection & );
  VP1GraphicsItemCollection & operator= ( const VP1GraphicsItemCollection & );
  class Imp;
  Imp * m_d;
private:
  friend class VP1GraphicsView;
  void attachToView(VP1GraphicsView*);
  void real_detachFromView();
  void setTemporaryIgnoreInteractions( const bool& );
  void setTemporaryIgnoreMovable( const bool& );
  void itemPickedPrivate(QGraphicsItem*) const;
  void itemGotEventPrivate(QGraphicsItem*,QEvent*) const;
  void selectionChangedPrivate(QList<QGraphicsItem*>) const;
};

#endif
