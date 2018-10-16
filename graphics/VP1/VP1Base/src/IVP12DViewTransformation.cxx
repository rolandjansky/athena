/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class IVP12DViewTransformation       //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/IVP12DViewTransformation.h"

#include <QAbstractGraphicsShapeItem>
#include <QGraphicsLineItem>
#include <QGraphicsPixmapItem>
#include <QPainter>

#include <iostream>//fixme
#include <cassert>

//____________________________________________________________________
void IVP12DViewTransformation::transformPath(const QPainterPath &source, QPainterPath &target) const
{
  assert(target.elementCount()==0);
  target.addPath(source);
  for (int i=0; i<target.elementCount(); ++i) {//Fixme:iterator better and possible?
    const QPainterPath::Element &e = target.elementAt(i);
    QPointF newpos = transform(QPointF(e.x,e.y));
    target.setElementPositionAt(i,newpos.x(),newpos.y());
  }
}

//____________________________________________________________________
void IVP12DViewTransformation::inverseTransformPath(const QPainterPath &source, QPainterPath &target) const
{
  assert(target.elementCount()==0);
  target.addPath(source);
  for (int i=0; i<target.elementCount(); ++i) {//Fixme:iterator better and possible?
    const QPainterPath::Element &e = target.elementAt(i);
    QPointF newpos = inverseTransform(QPointF(e.x,e.y));
    target.setElementPositionAt(i,newpos.x(),newpos.y());
  }
}

//____________________________________________________________________
QRectF IVP12DViewTransformation::inverseTransform(const QRectF &source) const
{
  QPointF p1 = inverseTransform(source.bottomLeft());
  QPointF p2 = inverseTransform(source.bottomRight());
  QPointF p3 = inverseTransform(source.topLeft());
  QPointF p4 = inverseTransform(source.topRight());

  QPointF topleft(qMin(qMin(qMin(p1.x(),p2.x()),p3.x()),p4.x()),
		  qMin(qMin(qMin(p1.y(),p2.y()),p3.y()),p4.y()));
  QPointF bottomright(qMax(qMax(qMax(p1.x(),p2.x()),p3.x()),p4.x()),
		      qMax(qMax(qMax(p1.y(),p2.y()),p3.y()),p4.y()));
  return QRectF(topleft,QSizeF(bottomright.x()-topleft.x(),bottomright.y()-topleft.y()));
}

//____________________________________________________________________
QRectF IVP12DViewTransformation::transform(const QRectF &source) const
{
  QPointF p1 = transform(source.bottomLeft());
  QPointF p2 = transform(source.bottomRight());
  QPointF p3 = transform(source.topLeft());
  QPointF p4 = transform(source.topRight());

  QPointF topleft(qMin(qMin(qMin(p1.x(),p2.x()),p3.x()),p4.x()),
		  qMin(qMin(qMin(p1.y(),p2.y()),p3.y()),p4.y()));
  QPointF bottomright(qMax(qMax(qMax(p1.x(),p2.x()),p3.x()),p4.x()),
		      qMax(qMax(qMax(p1.y(),p2.y()),p3.y()),p4.y()));
  return QRectF(topleft,QSizeF(bottomright.x()-topleft.x(),bottomright.y()-topleft.y()));
}

//____________________________________________________________________
void IVP12DViewTransformation::paintItem(QPainter *painter, const QGraphicsItem*item) const
{
  //We are redoing most of the stuff as found in the various paint
  //methods in qgraphicsitem.cpp. We dont care about
  //QStyleOptionGraphicsItem's since they only seem to involve drawing
  //rectangles around selected items (which we do another way anyway).

  const QAbstractGraphicsShapeItem * shapeitem = qgraphicsitem_cast<const QAbstractGraphicsShapeItem*>(item);
  const QGraphicsLineItem * lineitem(0);
  if (!shapeitem)
    lineitem = qgraphicsitem_cast<const QGraphicsLineItem*>(item);

  if (shapeitem||lineitem) {
    std::cout<<"IVP12DViewTransformation::paintItem: INFO: Draws QAbstractGraphicsShapeItem/QGraphicsLineItem"<<std::endl;
    if (shapeitem) {
      const QGraphicsSimpleTextItem * simpletextitem = qgraphicsitem_cast<const QGraphicsSimpleTextItem*>(/*shape*/item);
      if (simpletextitem) {
	std::cout<<"IVP12DViewTransformation::paintItem: WARNING - does not presently draw QGraphicsSimpleTextItems"<<std::endl;
	return;
      }
    }
    painter->setPen(shapeitem?shapeitem->pen():lineitem->pen());
    if (shapeitem)
      painter->setBrush(shapeitem->brush());
    QPainterPath path;
    transformPath( (shapeitem?shapeitem->shape():lineitem->shape()), path);
    painter->drawPath(path);//Fixme:transform
    //   QPainterPath QGraphicsPolygonItem::shape();
    //   QPainterPath QGraphicsPathItem::shape();
    //   QPainterPath QGraphicsRectItem::shape();
    //   QPainterPath QGraphicsEllipseItem::shape();//good enough
    //Fixme: Most standard QT shape items have slightly more efficient methods that might be useful...
    return;
  }
  const QGraphicsItemGroup * groupitem = qgraphicsitem_cast<const QGraphicsItemGroup*>(item);
  if (groupitem) {
    std::cout<<"IVP12DViewTransformation::paintItem: INFO: drawing QGraphicsGroupItem"<<std::endl;
    return;
  }
  const QGraphicsPixmapItem * pixmapitem = qgraphicsitem_cast<const QGraphicsPixmapItem*>(item);
  if (pixmapitem) {
    std::cout<<"IVP12DViewTransformation::paintItem: WARNING - does not presently draw QGraphicsPixmapItems"<<std::endl;
    //Pixmap: construct matrix transformation from the corners, and hope that the transformation is linear enough that it works.
    //Perhaps test the assumption on a few other points within the pixmap.
    return;
  }

  std::cout<<"IVP12DViewTransformation::paintItem: Can not paint graphics item in transformed view."
	   <<" Please only use items derived from either QAbstractGraphicsShapeItem, QGraphicsLineItem, QGraphicsGroupItem or QGraphicsPixmapItem."<<std::endl;
  return;


}
