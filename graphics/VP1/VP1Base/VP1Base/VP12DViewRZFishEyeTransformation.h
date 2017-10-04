/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////
//                                                          //
//  Header file for class VP12DViewRZFishEyeTransformation  //
//                                                          //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>   //
//                                                          //
//  Initial version: April 2007                             //
//                                                          //
//////////////////////////////////////////////////////////////

#ifndef VP12DVIEWRZFISHEYETRANSFORMATION_H
#define VP12DVIEWRZFISHEYETRANSFORMATION_H

#include "VP1Base/IVP12DViewTransformation.h"

class VP12DViewRZFishEyeTransformation : public IVP12DViewTransformation {
public:

  VP12DViewRZFishEyeTransformation();
  virtual ~VP12DViewRZFishEyeTransformation() {};

  QPointF transform(const QPointF &source) const;
  QPointF inverseTransform(const QPointF &source) const;

  virtual QRectF inverseTransform(const QRectF &source) const { return IVP12DViewTransformation::inverseTransform(source); }
  virtual QRectF transform(const QRectF &source) const { return IVP12DViewTransformation::inverseTransform(source); }

private:

  double m_fisheyepar;

};

/////////////////// INLINES //////////////////////////

#include <cmath>

inline QPointF VP12DViewRZFishEyeTransformation::transform(const QPointF &source) const
{
  return QPointF(source.x()/(1+fabs(source.x()*m_fisheyepar)),source.y()/(1+fabs(source.y()*m_fisheyepar)));
}

inline QPointF VP12DViewRZFishEyeTransformation::inverseTransform(const QPointF &source) const
{
  return QPointF(source.x()/(1-fabs(source.x()*m_fisheyepar)),source.y()/(1-fabs(source.y()*m_fisheyepar)));
}

#endif
