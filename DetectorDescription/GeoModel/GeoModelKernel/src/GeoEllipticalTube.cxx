/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include "GeoModelKernel/GeoPolyhedrizeAction.h"
#include "GeoModelKernel/GeoShapeAction.h"

#include "GeoModelKernel/GeoEllipticalTube.h"
const std::string GeoEllipticalTube::s_classType = "EllipticalTube";
const ShapeType GeoEllipticalTube::s_classTypeID = 0x22;


GeoEllipticalTube::GeoEllipticalTube(double XHalfLength, double YHalfLength, double ZHalfLength)
  : m_xHalfLength(XHalfLength)
  , m_yHalfLength(YHalfLength)
  , m_zHalfLength(ZHalfLength)
{
}


GeoEllipticalTube::~GeoEllipticalTube()
{
}

double GeoEllipticalTube::volume () const
{
#ifndef M_PI
  double M_PI = acos (-1.0);
#endif
  return 2 * M_PI * m_xHalfLength * m_yHalfLength * m_zHalfLength;
}

const std::string & GeoEllipticalTube::type () const
{
  return s_classType;
}

ShapeType GeoEllipticalTube::typeID () const
{
  return s_classTypeID;
}

void GeoEllipticalTube::exec (GeoShapeAction *action) const
{
  action->handleEllipticalTube(this);
}

