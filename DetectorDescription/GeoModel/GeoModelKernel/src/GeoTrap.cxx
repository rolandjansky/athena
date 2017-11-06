/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoShapeAction.h"

const std::string GeoTrap::s_classType = "Trap";
const ShapeType GeoTrap::s_classTypeID = 0x15;

GeoTrap::GeoTrap (double ZHalfLength, double Theta, double Phi, double Dydzn, double Dxdyndzn, double Dxdypdzn, double Angleydzn, double Dydzp, double Dxdyndzp, double Dxdypdzp, double Angleydzp)
  : m_zHalfLength (ZHalfLength)
  , m_theta (Theta)
  , m_phi (Phi)
  , m_dydzn (Dydzn)
  , m_dxdyndzn (Dxdyndzn)
  , m_dxdypdzn (Dxdypdzn)
  , m_angleydzn (Angleydzn)
  , m_dydzp (Dydzp)
  , m_dxdyndzp (Dxdyndzp)
  , m_dxdypdzp (Dxdypdzp)
  , m_angleydzp (Angleydzp)
{
}

GeoTrap::~GeoTrap()
{
}

double GeoTrap::volume () const
{
  double fDz = m_zHalfLength;
  double fDy1 = m_dydzn;		              
  double pDx1 = m_dxdyndzn;	              
  double pDx2 = m_dxdypdzn;	              
  double fDy2 = m_dydzp;		              
  double pDx3 = m_dxdyndzp;	              
  double pDx4 = m_dxdypdzp;	                  


  double fDx1 = 0.5 * (pDx1 + pDx2);
  double fDx2 = 0.5 * (pDx3 + pDx4);

  return 4.0 * ((fDx1 + fDx2) * (fDy1 + fDy2) * (fDz * 0.5) +
		(fDx2 - fDx1) * (fDy2 - fDy1) * (fDz * (1./6)));
}

const std::string & GeoTrap::type () const
{
  return s_classType;
}

ShapeType GeoTrap::typeID () const
{
  return s_classTypeID;
}

void GeoTrap::exec (GeoShapeAction *action) const
{
  action->handleTrap(this);
}
