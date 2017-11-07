/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoShapeAction.h"
#include <cmath>
#include <stdexcept>

const std::string GeoPcon::s_classType = "Pcon";
const ShapeType GeoPcon::s_classTypeID = 0x13;

GeoPcon::GeoPcon (double SPhi, double DPhi)
  : m_sPhi (SPhi)
  , m_dPhi (DPhi)
{
}

GeoPcon::~GeoPcon()
{
}

double GeoPcon::volume () const
{
  if (!isValid ())
    throw std::runtime_error ("Volume requested for incomplete polycone");
  double v = 0;
  for (size_t s = 0; s < getNPlanes () - 1; s++)
    {
      double fDz = fabs (getZPlane (s + 1) - getZPlane (s)) / 2.0;
      double fRmin1 = getRMinPlane (s + 1);
      double fRmin2 = getRMinPlane (s);
      double fRmax1 = getRMaxPlane (s + 1);
      double fRmax2 = getRMaxPlane (s);
      v +=
	(m_dPhi * (1./3)) * fDz * (fRmax1 * fRmax1 + fRmax2 * fRmax2 +
                                   fRmax1 * fRmax2 - fRmin1 * fRmin1 -
                                   fRmin2 * fRmin2 - fRmin1 * fRmin2);
    }
  return v;
}

const std::string & GeoPcon::type () const
{
  return s_classType;
}

ShapeType GeoPcon::typeID () const
{
  return s_classTypeID;
}

void GeoPcon::addPlane (double ZPlane, double RMinPlane, double RMaxPlane)
{
  m_zPlane.push_back (ZPlane);
  m_rMinPlane.push_back (RMinPlane);
  m_rMaxPlane.push_back (RMaxPlane);
}

void GeoPcon::exec (GeoShapeAction *action) const
{
  action->handlePcon(this);
}

