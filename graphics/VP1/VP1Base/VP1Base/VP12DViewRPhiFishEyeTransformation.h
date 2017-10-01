/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP12DViewRPhiFishEyeTransformation  //
//                                                            //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>     //
//                                                            //
//  Initial version: April 2007                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP12DVIEWRPHIFISHEYETRANSFORMATION_H
#define VP12DVIEWRPHIFISHEYETRANSFORMATION_H

#include "VP1Base/IVP12DViewTransformation.h"

class VP12DViewRPhiFishEyeTransformation : public IVP12DViewTransformation {
public:

  VP12DViewRPhiFishEyeTransformation();
  virtual ~VP12DViewRPhiFishEyeTransformation() {};

  QPointF transform(const QPointF &source) const;
  QPointF inverseTransform(const QPointF &source) const;

  virtual QRectF inverseTransform(const QRectF &source) const { return IVP12DViewTransformation::inverseTransform(source); }
  virtual QRectF transform(const QRectF &source) const { return IVP12DViewTransformation::inverseTransform(source); }

private:

  double m_fisheyepar;

};

/////////////////// INLINES //////////////////////////

#include <cmath>

inline QPointF VP12DViewRPhiFishEyeTransformation::transform(const QPointF &source) const
{
  QPointF p(source);
  p /= 1.0+m_fisheyepar*sqrt(source.x()*source.x()+source.y()*source.y());
  return p;
}

inline QPointF VP12DViewRPhiFishEyeTransformation::inverseTransform(const QPointF &source) const
{
  QPointF p(source);
  p /= 1.0-m_fisheyepar*sqrt(source.x()*source.x()+source.y()*source.y());
  return p;
}

#endif
