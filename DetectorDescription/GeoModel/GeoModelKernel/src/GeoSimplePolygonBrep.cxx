/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoShapeAction.h"
#include "PolygonTriangulator.h"//For volume.
#include <cmath>
#include <stdexcept>

const std::string GeoSimplePolygonBrep::s_classType = "SimplePolygonBrep";
const ShapeType GeoSimplePolygonBrep::s_classTypeID = 0x20;

GeoSimplePolygonBrep::GeoSimplePolygonBrep(double dz)
   : m_dZ(dz)
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
  PolygonTriangulator pt(m_xVertices,m_yVertices);

  double area(0);//Fixme: Should be cached!

  double x1, x2, x3, y1, y2, y3;
  PolygonTriangulator::Triangles::const_iterator it, itE = pt.triangles()->end();
  for (it = pt.triangles()->begin(); it!=itE; ++it) {
    x1 = m_xVertices.at(it->at(0)-1);
    x2 = m_xVertices.at(it->at(1)-1);
    x3 = m_xVertices.at(it->at(2)-1);
    y1 = m_yVertices.at(it->at(0)-1);
    y2 = m_yVertices.at(it->at(1)-1);
    y3 = m_yVertices.at(it->at(2)-1);
    area += fabs ( -x2*y1 + x3*y1 + x1 * y2 - x3 * y2 - x1*y3 + x2*y3 );//This is actually twice the area of the triangle we are adding
  }

  return area * m_dZ;//area is thus twice the area, but dZ is only half the depth, so the two factors of two cancel out.
}

const std::string & GeoSimplePolygonBrep::type() const
{
  return s_classType;
}

ShapeType GeoSimplePolygonBrep::typeID() const
{
  return s_classTypeID;
}

void GeoSimplePolygonBrep::addVertex(double XVertex, double YVertex)
{
  m_xVertices.push_back(XVertex);
  m_yVertices.push_back(YVertex);
}

void GeoSimplePolygonBrep::exec(GeoShapeAction *action) const
{
  action->handleSimplePolygonBrep(this);
}

