/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EllipseBounds.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSurfaces/EllipseBounds.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <iomanip>
#include <iostream>

Trk::EllipseBounds::EllipseBounds()
  : m_boundValues(EllipseBounds::bv_length, 0.)
{}

Trk::EllipseBounds::EllipseBounds(double minradX, double minradY, double maxradX, double maxradY, double hphisec)
  : m_boundValues(EllipseBounds::bv_length, 0.)
{
  m_boundValues[EllipseBounds::bv_rMinX] = minradX;
  m_boundValues[EllipseBounds::bv_rMinY] = minradY;
  m_boundValues[EllipseBounds::bv_rMaxX] = maxradX;
  m_boundValues[EllipseBounds::bv_rMaxY] = maxradY;
  m_boundValues[EllipseBounds::bv_averagePhi] = 0.;
  m_boundValues[EllipseBounds::bv_halfPhiSector] = hphisec;
  if (m_boundValues[EllipseBounds::bv_rMinX] > m_boundValues[EllipseBounds::bv_rMaxX])
    swap(m_boundValues[EllipseBounds::bv_rMinX], m_boundValues[EllipseBounds::bv_rMaxX]);
  if (m_boundValues[EllipseBounds::bv_rMinY] > m_boundValues[EllipseBounds::bv_rMaxY])
    swap(m_boundValues[EllipseBounds::bv_rMinY], m_boundValues[EllipseBounds::bv_rMaxY]);
}

Trk::EllipseBounds::EllipseBounds(double minradX,
                                  double minradY,
                                  double maxradX,
                                  double maxradY,
                                  double avephi,
                                  double hphisec)
  : m_boundValues(EllipseBounds::bv_length, 0.)
{

  m_boundValues[EllipseBounds::bv_rMinX] = minradX;
  m_boundValues[EllipseBounds::bv_rMinY] = minradY;
  m_boundValues[EllipseBounds::bv_rMaxX] = maxradX;
  m_boundValues[EllipseBounds::bv_rMaxY] = maxradY;
  m_boundValues[EllipseBounds::bv_averagePhi] = avephi;
  m_boundValues[EllipseBounds::bv_halfPhiSector] = hphisec;
  if (m_boundValues[EllipseBounds::bv_rMinX] > m_boundValues[EllipseBounds::bv_rMaxX])
    swap(m_boundValues[EllipseBounds::bv_rMinX], m_boundValues[EllipseBounds::bv_rMaxX]);
  if (m_boundValues[EllipseBounds::bv_rMinY] > m_boundValues[EllipseBounds::bv_rMaxY])
    swap(m_boundValues[EllipseBounds::bv_rMinY], m_boundValues[EllipseBounds::bv_rMaxY]);
}

bool
Trk::EllipseBounds::operator==(const Trk::SurfaceBounds& sbo) const
{
  // check the type first not to compare apples with oranges
  const Trk::EllipseBounds* discbo = dynamic_cast<const Trk::EllipseBounds*>(&sbo);
  if (!discbo)
    return false;
  return (m_boundValues == discbo->m_boundValues);
}

// For ellipse bound this is only approximation which is valid
// only if m_boundValues[EllipseBounds::bv_rMinX] ~= m_boundValues[EllipseBounds::bv_rMinY]
// and m_boundValues[EllipseBounds::bv_rMaxX] ~= m_boundValues[EllipseBounds::bv_rMaxY]
//
double
Trk::EllipseBounds::minDistance(const Amg::Vector2D& pos) const
{
  const double pi2 = 2. * M_PI;

  double r = sqrt(pos[0] * pos[0] + pos[1] * pos[1]);
  if (r == 0.) {
    if (m_boundValues[EllipseBounds::bv_rMinX] <= m_boundValues[EllipseBounds::bv_rMinY])
      return m_boundValues[EllipseBounds::bv_rMinX];
    return m_boundValues[EllipseBounds::bv_rMinY];
  }

  const double inv_r = 1. / r;
  double sn = pos[1] * inv_r;
  double cs = pos[0] * inv_r;
  double sf = 0.;
  double dF = 0.;

  if (m_boundValues[EllipseBounds::bv_halfPhiSector] < M_PI) {

    dF = atan2(cs, sn) - m_boundValues[EllipseBounds::bv_averagePhi];
    dF += (dF > M_PI) ? -pi2 : (dF < -M_PI) ? pi2 : 0;
    double df = fabs(dF) - m_boundValues[EllipseBounds::bv_halfPhiSector];
    sf = r * sin(df);
    if (df > 0.)
      r *= cos(df);
  } else {
    sf = -1.e+10;
  }

  if (sf <= 0.) {

    double a = cs / m_boundValues[EllipseBounds::bv_rMaxX];
    double b = sn / m_boundValues[EllipseBounds::bv_rMaxY];
    double sr0 = r - 1. / sqrt(a * a + b * b);
    if (sr0 >= 0.)
      return sr0;
    a = cs / m_boundValues[EllipseBounds::bv_rMinX];
    b = sn / m_boundValues[EllipseBounds::bv_rMinY];
    double sr1 = 1. / sqrt(a * a + b * b) - r;
    if (sr1 >= 0.)
      return sr1;
    if (sf < sr0)
      sf = sr0;
    if (sf < sr1)
      sf = sr1;
    return sf;
  }

  double fb;
  fb = (dF > 0.) ? m_boundValues[EllipseBounds::bv_averagePhi] + m_boundValues[EllipseBounds::bv_halfPhiSector]
                 : m_boundValues[EllipseBounds::bv_averagePhi] - m_boundValues[EllipseBounds::bv_halfPhiSector];
  sn = sin(fb);
  cs = cos(fb);
  double a = cs / m_boundValues[EllipseBounds::bv_rMaxX];
  double b = sn / m_boundValues[EllipseBounds::bv_rMaxY];
  double sr0 = r - 1. / sqrt(a * a + b * b);
  if (sr0 >= 0.)
    return sqrt(sr0 * sr0 + sf * sf);
  a = cs / m_boundValues[EllipseBounds::bv_rMinX];
  b = sn / m_boundValues[EllipseBounds::bv_rMinY];
  double sr1 = 1. / sqrt(a * a + b * b) - r;
  if (sr1 >= 0.)
    return sqrt(sr1 * sr1 + sf * sf);
  return sf;
}

// ostream operator overload

MsgStream&
Trk::EllipseBounds::dump(MsgStream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::EllipseBounds:  (innerRadiusX, innerRadiusY, outerRadiusX, outerRadiusY, averagePhi, hPhiSector) = ";
  sl << "(" << this->rMinX() << ", " << this->rMinY() << ", " << this->rMaxX() << ", " << this->rMaxY() << ", "
     << this->averagePhi() << ", " << this->halfPhiSector() << ")";
  sl << std::setprecision(-1);
  return sl;
}

std::ostream&
Trk::EllipseBounds::dump(std::ostream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::EllipseBounds:  (innerRadiusX, innerRadiusY, outerRadiusX, outerRadiusY, hPhiSector) = ";
  sl << "(" << this->rMinX() << ", " << this->rMinY() << ", " << this->rMaxX() << ", " << this->rMaxY() << ", "
     << this->averagePhi() << ", " << this->halfPhiSector() << ")";
  sl << std::setprecision(-1);
  return sl;
}
