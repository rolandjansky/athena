/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1GraphicsView                  //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1GRAPHICSVIEW_H
#define VP1GRAPHICSVIEW_H

// FIXME: need to include this before any QT header, because QT defines
// a macro `foreach' that collides with an identifier in AthenaKernel.
#include "xAODCore/BaseInfo.h"

#include <QGraphicsView>

class VP1GraphicsItemCollection;

class VP1GraphicsView : public QGraphicsView {

  Q_OBJECT

public:

  ////////////////////////////////////////////////////////////////////////
  ///////////////  This is the interesting bit for those  ////////////////
  ///////////////  who are using this class...            ////////////////
  ////////////////////////////////////////////////////////////////////////

  VP1GraphicsView ( QWidget * parent = 0 );
  VP1GraphicsView ( QGraphicsScene * scene, QWidget * parent = 0 );

  //Get interaction mode:
  enum Mode { PICK, CHANGEVIEW, SEEK, DRAGZOOM, DRAGZOOMHOLD };
  Mode mode() const;

public slots:

  //What collections to show:
  void addItemCollection(VP1GraphicsItemCollection*);
  void removeItemCollection(VP1GraphicsItemCollection*);

  //Set interaction mode for the view:
  void setMode(const Mode&);

  //Potentially limit the interactive capabilities of given itemcollections:
  void setDisallowInteractions(VP1GraphicsItemCollection*, const bool& disallow=true );//If true, the ic will act as INERT
  void setDisallowMovable(VP1GraphicsItemCollection*, const bool& disallow=true );//If true, items will never be movable.

  //Zoom:
  void scaleView(qreal scaleFactor);
  void fitViewToContents();
  void viewHome();
  void setHome();

  //Animated zoom:
  void initiateAnimatedZoomTo(QRectF);
  void setAnimatedZoomSteps(int);
  void setAnimatedZoomTime(double);//ms
  void setZoomFactorOnSeek(double);
  void abortZoomAnimation();
  void seekToPoint(QPointF);

  //Selections:
  void clearSelections();

  //Print:
  void print();
  void saveImage();

  ////////////////////////////////////////////////////////////////////////
  ////////////  The rest of this file is just internal stuff  ////////////
  ////////////           - dont worry about it -              ////////////
  ////////////////////////////////////////////////////////////////////////

public:
  virtual ~VP1GraphicsView();

protected:
  void keyPressEvent(QKeyEvent *);
  void wheelEvent(QWheelEvent *);
  void mouseDoubleClickEvent(QMouseEvent * event);
  void mousePressEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);
  void drawItems(QPainter *painter, int numItems,
		 QGraphicsItem *items[],
		 const QStyleOptionGraphicsItem options[]);
  void drawForeground ( QPainter * painter, const QRectF & rect );
private:

  class Imp;
  Imp * m_d;
  class HighLightItem;
  friend class HighLightItem;
  void emitSelectionChanged(VP1GraphicsItemCollection*);

private slots:
  void updateZoomAnimation();
};

#endif
