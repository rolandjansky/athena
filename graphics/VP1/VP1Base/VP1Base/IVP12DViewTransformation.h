/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class IVP12DViewTransformation         //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef IVP12DVIEWTRANSFORMATION_H
#define IVP12DVIEWTRANSFORMATION_H

#include <QtGlobal>
#include <QPainterPath>

class QPainter;
class QGraphicsItem;

//The practically most important thing for performance is that
//transformPath() is as efficient as possible!!

class IVP12DViewTransformation {
public:

  IVP12DViewTransformation() {};
  virtual ~IVP12DViewTransformation() {};

  virtual QPointF transform(const QPointF &source) const = 0;
  virtual QPointF inverseTransform(const QPointF &source) const = 0;

  //Transform a painter path. The default implementation simply
  //transforms the positions of each of the elements in the path. For
  //non-linear transformations this might not be exactly what is
  //wanted (but might be "good enough":
  virtual void transformPath(const QPainterPath &source, QPainterPath &target) const;
  virtual void inverseTransformPath(const QPainterPath &source, QPainterPath &target) const;

  //The returned rectangle for the next two methods should be the
  //smallest rectangle that contains all transformed points from the
  //source rectangle. The default implementation returns the smallest
  //rectangle that contains all of the four inversely transformed
  //corners of the original rectangle (for some nonlinear
  //transformations this might not be a correct solution!):
  virtual QRectF inverseTransform(const QRectF &source) const;
  virtual QRectF transform(const QRectF &source) const;

  //Fixme: Stuff for getting key events delivered (by e.g. registering
  //that it is interested in Key_Plus+Shift, Key_Minus+Shift). And it
  //should report back if the view now needs to be updated.

  //Paint an item transformed:
  void paintItem(QPainter *, const QGraphicsItem*) const;

private:

  IVP12DViewTransformation(const IVP12DViewTransformation & );
  IVP12DViewTransformation & operator= (const IVP12DViewTransformation & );
};

#endif
