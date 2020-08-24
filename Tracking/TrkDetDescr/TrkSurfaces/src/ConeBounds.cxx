/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ConeBounds.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSurfaces/ConeBounds.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <cmath>
#include <iomanip>
#include <iostream>

Trk::ConeBounds::ConeBounds()
  : m_boundValues(ConeBounds::bv_length, 0.)
  , m_tanAlpha(0.)
  , m_sinAlpha(0.)
  , m_cosAlpha(0.)
{}

Trk::ConeBounds::ConeBounds(double alpha, bool symm, double halfphi, double avphi)
  : m_boundValues(ConeBounds::bv_length, 0.)
  , m_tanAlpha(0.)
  , m_sinAlpha(0.)
  , m_cosAlpha(0.)
{
  m_boundValues[ConeBounds::bv_alpha] = alpha;
  m_boundValues[ConeBounds::bv_minZ] = symm ? -MAXBOUNDVALUE : 0.;
  m_boundValues[ConeBounds::bv_maxZ] = MAXBOUNDVALUE;
  m_boundValues[ConeBounds::bv_averagePhi] = avphi;
  m_boundValues[ConeBounds::bv_halfPhiSector] = halfphi;
  initCache();
}

Trk::ConeBounds::ConeBounds(double alpha, double zmin, double zmax, double halfphi, double avphi)
  : m_boundValues(ConeBounds::bv_length, 0.)
  , m_tanAlpha(0.)
  , m_sinAlpha(0.)
  , m_cosAlpha(0.)
{
  m_boundValues[ConeBounds::bv_alpha] = alpha;
  m_boundValues[ConeBounds::bv_minZ] = zmin;
  m_boundValues[ConeBounds::bv_maxZ] = zmax;
  m_boundValues[ConeBounds::bv_averagePhi] = avphi;
  m_boundValues[ConeBounds::bv_halfPhiSector] = halfphi;
  initCache();
}

bool
Trk::ConeBounds::operator==(const SurfaceBounds& sbo) const
{
  // check the type first not to compare apples with oranges
  const Trk::ConeBounds* conebo = dynamic_cast<const Trk::ConeBounds*>(&sbo);
  if (!conebo)
    return false;
  return (m_boundValues == conebo->m_boundValues);
}

double
Trk::ConeBounds::minDistance(const Amg::Vector2D& pos) const
{
  // This needs to be split based on where pos is with respect to the
  // cone. Inside, its easy, inside the z-region or inside the phi
  // region, just get the difference from the outside quantity, for
  // outside both the z and dphi regions, need to get the distance to
  // the cone corner, but remember, the cone piece will be symmetric
  // about the center of phi

  // TODO: The whole scheme here assumes that the local position is in
  // a half of R^3 where the cone is defined. If the local position is
  // in say the z < 0 half, and the cone is only defined for z > 0,
  // then it won't work

  // find the minimum distance along the z direction
  double toMinZ = m_boundValues[ConeBounds::bv_minZ] - pos[locZ];
  double toMaxZ = pos[locZ] - m_boundValues[ConeBounds::bv_maxZ];
  double toZ = (fabs(toMinZ) < fabs(toMaxZ)) ? toMinZ : toMaxZ;

  // NB this works only if the localPos is in the same hemisphere as
  // the cone (i.e. if the localPos has z < 0 and the cone only
  // defined for z > z_min where z_min > 0, this is wrong)
  double zDist = sqrt(toZ * toZ * (1. + m_tanAlpha * m_tanAlpha));
  if (toZ < 0.) // positive if outside the cone only
    zDist = -zDist;

  // if the cone is complete, or pos is in the same phi range as the
  // cone piece then its just the distance along the cone.
  if (m_boundValues[ConeBounds::bv_halfPhiSector] >= M_PI)
    return zDist;

  // we have a conical segment, so find also the phi distance
  // Note that here we take the phi distance as the distance from
  // going to the correct phi by a straight line at the point that was
  // input by the user (not at the point of closest approach to the
  // cone)
  double posR = pos[locZ] * m_tanAlpha;
  double deltaPhi = pos[locRPhi] / posR - m_boundValues[ConeBounds::bv_averagePhi]; // from center
  if (deltaPhi > M_PI)
    deltaPhi = 2 * M_PI - deltaPhi;
  if (deltaPhi < -M_PI)
    deltaPhi = 2 * M_PI + deltaPhi;

  // straight line distance (goes off cone)
  double phiDist = 2 * posR * sin(.5 * (deltaPhi - m_boundValues[ConeBounds::bv_halfPhiSector]));

  // if inside the cone, return the smaller length (since both are
  // negative, the *larger* of the 2 is the *smaller* distance)
  if (phiDist <= 0. && zDist <= 0) {
    if (phiDist > zDist){
      return phiDist;
    }
    return zDist;
  }

  // if inside the phi or z boundary, return the other
  if (phiDist <= 0.){
    return zDist;
  }
  if (zDist <= 0.){
    return phiDist;
  }

  // otherwise, return both (this should be the distance to the corner
  // closest to the cone
  return sqrt(zDist * zDist + phiDist * phiDist);
}

// ostream operator overload

MsgStream&
Trk::ConeBounds::dump(MsgStream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::ConeBounds: (tanAlpha, minZ, maxZ, averagePhi, halfPhiSector) = ";
  sl << "(" << this->tanAlpha() << ", " << this->minZ() << ", " << this->maxZ() << ", " << this->averagePhi() << ", "
     << this->halfPhiSector() << ")";
  sl << std::setprecision(-1);
  return sl;
}

std::ostream&
Trk::ConeBounds::dump(std::ostream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::ConeBounds: (tanAlpha, minZ, maxZ, averagePhi, halfPhiSector) = ";
  sl << "(" << this->tanAlpha() << ", " << this->minZ() << ", " << this->maxZ() << ", " << this->averagePhi() << ", "
     << this->halfPhiSector() << ")";
  sl << std::setprecision(-1);
  return sl;
}
