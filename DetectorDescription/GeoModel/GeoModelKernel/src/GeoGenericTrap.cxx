/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoGenericTrap.h"
#include "GeoModelKernel/GeoShapeAction.h"

const std::string GeoGenericTrap::classType = "GenericTrap";
const ShapeType GeoGenericTrap::classTypeID = 0x23;

GeoGenericTrap::GeoGenericTrap(double ZHalfLength, const GeoGenericTrapVertices& Vertices):
  m_zHalfLength(ZHalfLength),
  m_vertices(Vertices)
{
}

GeoGenericTrap::~GeoGenericTrap()
{
}

double GeoGenericTrap::volume() const
{
  // -- ToDo
  return 0.;
}

const std::string& GeoGenericTrap::type() const
{
  return classType;
}

ShapeType GeoGenericTrap::typeID() const
{
  return classTypeID;
}

void GeoGenericTrap::exec(GeoShapeAction *action) const
{
  action->handleGenericTrap(this);
}

double GeoGenericTrap::getZHalfLength() const
{
  return m_zHalfLength;
}

const GeoGenericTrapVertices& GeoGenericTrap::getVertices() const
{
  return m_vertices;
}

