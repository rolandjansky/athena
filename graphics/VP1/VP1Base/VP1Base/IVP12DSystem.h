/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class IVP12DSystem                     //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef IVP12DSYSTEM_H
#define IVP12DSYSTEM_H

#include "VP1Base/IVP1System.h"

#include <QSet>

class VP1GraphicsItemCollection;
class QGraphicsItem;

class IVP12DSystem : public IVP1System {

  Q_OBJECT

public:

  ////////////////////////////////////////////////////////////////////////
  ///////////  This is the interesting bit for those           ///////////
  ///////////  who are deriving from, or using, this class...  ///////////
  ////////////////////////////////////////////////////////////////////////

  IVP12DSystem(const QString & name, const QString & information, const QString & contact_info);

  //Derived classes must call the following method exactly once on
  //each item collection they provide:
  void registerItemCollection(VP1GraphicsItemCollection*);

  //Reimplement if you need to catch user picks, selections or events (you need to enable the item collections for this!):

  virtual void itemPicked(VP1GraphicsItemCollection*,QGraphicsItem*);
  virtual void itemGotEvent(VP1GraphicsItemCollection*,QGraphicsItem*,QEvent*);
  virtual void selectionChanged(VP1GraphicsItemCollection*,QList<QGraphicsItem*>);

  //This is for use by the channels:
  QSet<VP1GraphicsItemCollection*> getItemCollections();

  ////////////////////////////////////////////////////////////////////////
  ////////////  The rest of this file is just internal stuff  ////////////
  ////////////           - dont worry about it -              ////////////
  ////////////////////////////////////////////////////////////////////////

  virtual ~IVP12DSystem();

signals:
  void itemFromSystemSelected();//Should be emitted whenever item in this system is selected (mainly to give channel chance to show the system controller)

private:
  IVP12DSystem( const IVP12DSystem & );
  IVP12DSystem & operator= ( const IVP12DSystem & );
  class Imp;
  Imp * m_d;

private slots:
  void itemPickedPrivate(QGraphicsItem*);
  void itemGotEventPrivate(QGraphicsItem*,QEvent*);
  void selectionChangedPrivate(QList<QGraphicsItem*>);
};

#endif
