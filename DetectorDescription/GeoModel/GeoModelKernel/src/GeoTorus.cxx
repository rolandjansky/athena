/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoPolyhedron.h"
#include "GeoModelKernel/GeoShapeAction.h"

#include "GeoModelKernel/GeoTorus.h"

const std::string GeoTorus::classType = "Torus";
const ShapeType GeoTorus::classTypeID = 0x24;

GeoTorus::GeoTorus (double Rmin, double Rmax, double Rtor, double SPhi, double DPhi):
  rMin (Rmin),
  rMax (Rmax),
  rTor(Rtor),
  sPhi (SPhi),
  dPhi (DPhi)
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
  return classType;
}

ShapeType GeoTorus::typeID () const
{
  return classTypeID;
}

void GeoTorus::exec (GeoShapeAction *action) const
{
  action->handleTorus(this);
}
