/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <stdexcept>
#include "GeoModelKernel/GeoShapeAction.h"

#include "GeoModelKernel/GeoSimplePolygonBrep.h"

#include "PolygonTriangulator.h"//For volume.

const std::string GeoSimplePolygonBrep::classType = "SimplePolygonBrep";
const ShapeType GeoSimplePolygonBrep::classTypeID = 0x20;

GeoSimplePolygonBrep::GeoSimplePolygonBrep(double dz):
  dZ(dz)
{
}

GeoSimplePolygonBrep::~GeoSimplePolygonBrep()
{
}

double GeoSimplePolygonBrep::volume () const
{
  if (!isValid())
    throw std::runtime_error ("Volume requested for incomplete simple polygon brep");

  //Todo: Cache the polygon area so we wont have to do this every time.
  PolygonTriangulator pt(xVertices,yVertices);

  double area(0);//Fixme: Should be cached!

  double x1, x2, x3, y1, y2, y3;
  PolygonTriangulator::Triangles::const_iterator it, itE = pt.triangles()->end();
  for (it = pt.triangles()->begin(); it!=itE; ++it) {
    x1 = xVertices.at(it->at(0)-1);
    x2 = xVertices.at(it->at(1)-1);
    x3 = xVertices.at(it->at(2)-1);
    y1 = yVertices.at(it->at(0)-1);
    y2 = yVertices.at(it->at(1)-1);
    y3 = yVertices.at(it->at(2)-1);
    area += fabs ( -x2*y1 + x3*y1 + x1 * y2 - x3 * y2 - x1*y3 + x2*y3 );//This is actually twice the area of the triangle we are adding
  }

  return area * dZ;//area is thus twice the area, but dZ is only half the depth, so the two factors of two cancel out.
}

const std::string & GeoSimplePolygonBrep::type() const
{
  return classType;
}

ShapeType GeoSimplePolygonBrep::typeID() const
{
  return classTypeID;
}

void GeoSimplePolygonBrep::addVertex(double XVertex, double YVertex)
{
  xVertices.push_back(XVertex);
  yVertices.push_back(YVertex);
}

void GeoSimplePolygonBrep::exec(GeoShapeAction *action) const
{
  action->handleSimplePolygonBrep(this);
}

