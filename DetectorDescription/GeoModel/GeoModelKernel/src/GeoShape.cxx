/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeShift.h"

GeoShape::GeoShape ()
{
}

GeoShape::~GeoShape()
{
}

const GeoShapeUnion & GeoShape::add (const GeoShape& shape) const
{
  GeoShapeUnion *unionNode = new GeoShapeUnion (this, &shape);
  return *unionNode;
}

const GeoShapeSubtraction & GeoShape::subtract (const GeoShape& shape) const
{
  GeoShapeSubtraction *subNode = new GeoShapeSubtraction (this, &shape);
  return *subNode;
}

const GeoShapeIntersection & GeoShape::intersect (const GeoShape& shape) const
{
  GeoShapeIntersection *intNode = new GeoShapeIntersection (this, &shape);
  return *intNode;
}

const GeoShapeShift & GeoShape::operator << (const HepGeom::Transform3D &shift) const
{
  GeoShapeShift *shiftNode = new GeoShapeShift (this, shift);
  return *shiftNode;
}
