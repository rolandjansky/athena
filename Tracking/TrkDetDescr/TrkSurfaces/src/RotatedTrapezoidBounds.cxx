/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RotatedTrapezoidBounds.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSurfaces/RotatedTrapezoidBounds.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <cmath>
#include <iomanip>
#include <iostream>

// default constructor
Trk::RotatedTrapezoidBounds::RotatedTrapezoidBounds()
  : m_boundValues(RotatedTrapezoidBounds::bv_length, 0.)
  , m_kappa(0.)
  , m_delta(0.)
{}

// constructor from arguments I
Trk::RotatedTrapezoidBounds::RotatedTrapezoidBounds(double halex, double minhalex, double maxhalex)
  : m_boundValues(RotatedTrapezoidBounds::bv_length, 0.)
  , m_kappa(0.)
  , m_delta(0.)
{
  m_boundValues[RotatedTrapezoidBounds::bv_halfX] = fabs(halex);
  m_boundValues[RotatedTrapezoidBounds::bv_minHalfY] = fabs(minhalex);
  m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY] = fabs(maxhalex);
  // swap if needed
  if (m_boundValues[RotatedTrapezoidBounds::bv_minHalfY] > m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY])
    swap(m_boundValues[RotatedTrapezoidBounds::bv_minHalfY], m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY]);
  // assign kappa and delta
  initCache();
}


bool
Trk::RotatedTrapezoidBounds::operator==(const Trk::SurfaceBounds& sbo) const
{
  // check the type first not to compare apples with oranges
  const Trk::RotatedTrapezoidBounds* trabo = dynamic_cast<const Trk::RotatedTrapezoidBounds*>(&sbo);
  if (!trabo)
    return false;
  return (m_boundValues == trabo->m_boundValues);
}

// checking if inside bounds
bool
Trk::RotatedTrapezoidBounds::inside(const Amg::Vector2D& locpo, double tol1, double tol2) const
{

  // the cases:
  double fabsX = fabs(locpo[Trk::locX]);
  double fabsY = fabs(locpo[Trk::locY]);
  // (1) a fast FALSE
  if (fabsX > (m_boundValues[RotatedTrapezoidBounds::bv_halfX] + tol1))
    return false;
  // (2) a fast FALSE
  if (fabsY > (m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY] + tol2))
    return false;
  // (3) a fast TRUE
  if (fabsY < (m_boundValues[RotatedTrapezoidBounds::bv_minHalfY] - tol2))
    return true;
  // (4) it is inside the rectangular shape solve the isBelow
  return (isBelow(locpo[Trk::locX], fabsY, tol1, tol2));
}

// checking if local point lies above a line
bool
Trk::RotatedTrapezoidBounds::isBelow(double locX, double fabsLocY, double tol1, double tol2) const
{
  // the most tolerant approach for tol1 and tol2
  return ((m_kappa * (locX + tol1) + m_delta) > fabsLocY - tol2);
}

double
Trk::RotatedTrapezoidBounds::minDistance(const Amg::Vector2D& pos) const
{
  const int Np = 4;

  double X[4] = { -m_boundValues[RotatedTrapezoidBounds::bv_halfX],
                  -m_boundValues[RotatedTrapezoidBounds::bv_halfX],
                  m_boundValues[RotatedTrapezoidBounds::bv_halfX],
                  m_boundValues[RotatedTrapezoidBounds::bv_halfX] };
  double Y[4] = { -m_boundValues[RotatedTrapezoidBounds::bv_minHalfY],
                  m_boundValues[RotatedTrapezoidBounds::bv_minHalfY],
                  m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY],
                  -m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY] };

  double dm = 1.e+20;
  double Ao = 0.;
  bool in = true;

  for (int i = 0; i != Np; ++i) {

    int j = i + 1;
    if (j == Np)
      j = 0;

    double x = X[i] - pos[0];
    double y = Y[i] - pos[1];
    double dx = X[j] - X[i];
    double dy = Y[j] - Y[i];
    double A = x * dy - y * dx;
    double S = -(x * dx + y * dy);

    if (S <= 0.) {
      double d = x * x + y * y;
      if (d < dm)
        dm = d;
    } else {
      double a = dx * dx + dy * dy;
      if (S <= a) {
        double d = (A * A) / a;
        if (d < dm)
          dm = d;
      }
    }
    if (i && in && Ao * A < 0.)
      in = false;
    Ao = A;
  }
  if (in){
    return -sqrt(dm);
  }
  return sqrt(dm);
}

// ostream operator overload

MsgStream&
Trk::RotatedTrapezoidBounds::dump(MsgStream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::RotatedTrapezoidBounds:  (halfX, minHalfX, maxHalfY) = "
     << "(" << m_boundValues[RotatedTrapezoidBounds::bv_halfX] << ", "
     << m_boundValues[RotatedTrapezoidBounds::bv_minHalfY] << ", " << m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY]
     << ")";
  sl << std::setprecision(-1);
  return sl;
}

std::ostream&
Trk::RotatedTrapezoidBounds::dump(std::ostream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::RotatedTrapezoidBounds:  (halfX, minHalfX, maxHalfY) = "
     << "(" << m_boundValues[RotatedTrapezoidBounds::bv_halfX] << ", "
     << m_boundValues[RotatedTrapezoidBounds::bv_minHalfY] << ", " << m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY]
     << ")";
  sl << std::setprecision(-1);
  return sl;
}
