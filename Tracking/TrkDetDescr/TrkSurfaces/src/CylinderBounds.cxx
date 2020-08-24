/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderBounds.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSurfaces/CylinderBounds.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <cmath>
#include <iomanip>
#include <iostream>

Trk::CylinderBounds::CylinderBounds()
  : m_boundValues(CylinderBounds::bv_length, 0.)
  , m_checkPhi(false)
{}

Trk::CylinderBounds::CylinderBounds(double radius, double halez)
  : m_boundValues(CylinderBounds::bv_length, 0.)
  , m_checkPhi(false)
{
  m_boundValues[CylinderBounds::bv_radius] = fabs(radius);
  m_boundValues[CylinderBounds::bv_halfPhiSector] = M_PI;
  m_boundValues[CylinderBounds::bv_halfZ] = fabs(halez);
}

Trk::CylinderBounds::CylinderBounds(double radius, double haphi, double halez)
  : m_boundValues(CylinderBounds::bv_length, 0.)
  , m_checkPhi(true)
{
  m_boundValues[CylinderBounds::bv_radius] = fabs(radius);
  m_boundValues[CylinderBounds::bv_halfPhiSector] = haphi;
  m_boundValues[CylinderBounds::bv_halfZ] = fabs(halez);
}

Trk::CylinderBounds::CylinderBounds(double radius, double haphi, double averagephi, double halez)
  : m_boundValues(CylinderBounds::bv_length, 0.)
  , m_checkPhi(true)
{
  m_boundValues[CylinderBounds::bv_radius] = fabs(radius);
  m_boundValues[CylinderBounds::bv_averagePhi] = averagephi;
  m_boundValues[CylinderBounds::bv_halfPhiSector] = haphi;
  m_boundValues[CylinderBounds::bv_halfZ] = fabs(halez);
}

bool
Trk::CylinderBounds::operator==(const SurfaceBounds& sbo) const
{
  // check the type first not to compare apples with oranges
  const Trk::CylinderBounds* cylbo = dynamic_cast<const Trk::CylinderBounds*>(&sbo);
  if (!cylbo)
    return false;
  return (m_boundValues == cylbo->m_boundValues);
}

double
Trk::CylinderBounds::minDistance(const Amg::Vector2D& pos) const
{
  const double pi2 = 2. * M_PI;

  double sZ = fabs(pos[locZ]) - m_boundValues[CylinderBounds::bv_halfZ];
  double wF = m_boundValues[CylinderBounds::bv_halfPhiSector];
  if (wF >= M_PI)
    return sZ;
  double dF =
    fabs(pos[locRPhi] / m_boundValues[CylinderBounds::bv_radius] - m_boundValues[CylinderBounds::bv_averagePhi]);
  if (dF > M_PI)
    dF = pi2 - dF;
  double sF = 2. * m_boundValues[CylinderBounds::bv_radius] * sin(.5 * (dF - wF));

  if (sF <= 0. || sZ <= 0.) {
    if (sF > sZ){
      return sF;
    }

    return sZ;
  }
  return sqrt(sF * sF + sZ * sZ);
}

// ostream operator overload
MsgStream&
Trk::CylinderBounds::dump(MsgStream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::CylinderBounds: (radius, averagePhi, halfPhiSector, halflengthInZ) = ";
  sl << "(" << this->r() << ", " << this->averagePhi() << ", ";
  sl << this->halfPhiSector() << ", " << this->halflengthZ() << ")";
  sl << std::setprecision(-1);
  return sl;
}

std::ostream&
Trk::CylinderBounds::dump(std::ostream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::CylinderBounds: (radius, averagePhi, halfPhiSector, halflengthInZ) = ";
  sl << "(" << this->r() << ", " << this->averagePhi() << ", ";
  sl << this->halfPhiSector() << ", " << this->halflengthZ() << ")";
  sl << std::setprecision(-1);
  return sl;
}
