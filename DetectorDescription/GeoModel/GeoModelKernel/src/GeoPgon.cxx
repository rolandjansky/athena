/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoPgon.h"
#include "GeoModelKernel/GeoShapeAction.h"
#include <cmath>
#include <stdexcept>

const std::string GeoPgon::s_classType = "Pgon";
const ShapeType GeoPgon::s_classTypeID = 0x14;

GeoPgon::GeoPgon (double SPhi, double DPhi, unsigned int NSides)
  : m_sPhi (SPhi)
  , m_dPhi (DPhi)
  , m_nSides (NSides)
{
}

GeoPgon::~GeoPgon()
{
}

double GeoPgon::volume () const
{
  if (!isValid ())
    throw std::runtime_error ("Volume requested for incomplete polygon");
  double v = 0;
  int sides = getNSides ();
  double alpha = m_dPhi/sides;
  double sinAlpha = sin(alpha);
  
  for (size_t s = 0; s < getNPlanes () - 1; s++) {
    double z2 = getZPlane (s);;
    double z1 = getZPlane (s + 1);;
    double fRmin1 = getRMinPlane (s + 1);
    double fRmin2 = getRMinPlane (s);
    double fRmax1 = getRMaxPlane (s + 1);
    double fRmax2 = getRMaxPlane (s);

    double b1 = (fRmax1 - fRmax2)/(z1 - z2);
    double b2 = (fRmin1 - fRmin2)/(z1 - z2);

    double a1 = fRmax2  - b1 * z2;
    double a2 = fRmin2  - b2 * z2;

    //v+=fabs((a1*a1-a2*a2)*(z1-z2) + (a1*b1-a2*b2)*(z1*z1-z2*z2) + (b1*b1-b2*b2)*(z1*z1*z1-z2*z2*z2)/3.);
    // Equivalent which should be less sensitive to numerical precision errors:
    v += fabs(z1 - z2) * ((a1 - a2) * (a1 + a2)  + 
			  (a1*b1 - a2*b2) * (z1 + z2) +
			  (b1 - b2) * (b1 + b2) * (z1*z1+z2*z2+z1*z2)*(1./3));  
  } 
  v *=  0.5 * sides * sinAlpha;
  return v;
}

const std::string & GeoPgon::type () const
{
  return s_classType;
}

ShapeType GeoPgon::typeID () const
{
  return s_classTypeID;
}

void GeoPgon::addPlane (double ZPlane, double RMinPlane, double RMaxPlane)
{
  m_zPlane.push_back (ZPlane);
  m_rMinPlane.push_back (RMinPlane);
  m_rMaxPlane.push_back (RMaxPlane);
}

void GeoPgon::exec (GeoShapeAction *action) const
{
  action->handlePgon(this);
}
