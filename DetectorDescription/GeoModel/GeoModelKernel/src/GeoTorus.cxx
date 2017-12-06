/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoTorus.h"
#include "GeoModelKernel/GeoPolyhedron.h"
#include "GeoModelKernel/GeoShapeAction.h"

const std::string GeoTorus::s_classType = "Torus";
const ShapeType GeoTorus::s_classTypeID = 0x24;

GeoTorus::GeoTorus (double Rmin, double Rmax, double Rtor, double SPhi, double DPhi)
   : m_rMin (Rmin)
   , m_rMax (Rmax)
   , m_rTor(Rtor)
   , m_sPhi (SPhi)
   , m_dPhi (DPhi)
{
}


GeoTorus::~GeoTorus()
{
}

//## Other Operations (implementation)
double GeoTorus::volume () const
{
  return 8.0;
}

const std::string & GeoTorus::type () const
{
  return s_classType;
}

ShapeType GeoTorus::typeID () const
{
  return s_classTypeID;
}

void GeoTorus::exec (GeoShapeAction *action) const
{
  action->handleTorus(this);
}
