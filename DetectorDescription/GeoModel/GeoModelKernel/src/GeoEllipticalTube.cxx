/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include "GeoModelKernel/GeoPolyhedrizeAction.h"
#include "GeoModelKernel/GeoShapeAction.h"

#include "GeoModelKernel/GeoEllipticalTube.h"
const std::string GeoEllipticalTube::classType = "EllipticalTube";
const ShapeType GeoEllipticalTube::classTypeID = 0x22;


GeoEllipticalTube::GeoEllipticalTube(double XHalfLength, double YHalfLength, double ZHalfLength)
  : xHalfLength(XHalfLength)
  , yHalfLength(YHalfLength)
  , zHalfLength(ZHalfLength)
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
  return 2 * M_PI * xHalfLength * yHalfLength * zHalfLength;
}

const std::string & GeoEllipticalTube::type () const
{
  return classType;
}

ShapeType GeoEllipticalTube::typeID () const
{
  return classTypeID;
}

void GeoEllipticalTube::exec (GeoShapeAction *action) const
{
  action->handleEllipticalTube(this);
}

