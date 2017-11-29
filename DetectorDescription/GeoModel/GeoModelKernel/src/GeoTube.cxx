/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPolyhedrizeAction.h"
#include "GeoModelKernel/GeoShapeAction.h"
#include <cmath>

const std::string GeoTube::s_classType = "Tube";
const ShapeType GeoTube::s_classTypeID = 0x17;

GeoTube::GeoTube (double RMin, double RMax, double ZHalfLength)
  : m_rMin (RMin)
  , m_rMax (RMax)
  , m_zHalfLength (ZHalfLength)
{
}

GeoTube::~GeoTube()
{
}

double GeoTube::volume () const
{
#ifndef M_PI
  double M_PI = acos (-1.0);
#endif
  return 2 * M_PI * (m_rMax * m_rMax - m_rMin * m_rMin) * m_zHalfLength;
}

const std::string & GeoTube::type () const
{
  return s_classType;
}

ShapeType GeoTube::typeID () const
{
  return s_classTypeID;
}

void GeoTube::exec (GeoShapeAction *action) const
{
  action->handleTube(this);
}
