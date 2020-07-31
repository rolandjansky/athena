/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RotatedDiamondBounds.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSurfaces/RotatedDiamondBounds.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <cmath>
#include <iomanip>
#include <iostream>

// default constructor
Trk::RotatedDiamondBounds::RotatedDiamondBounds()
  : m_boundValues(RotatedDiamondBounds::bv_length, 0.)
  , m_alpha1(0.)
  , m_alpha2(0.)
{}

// constructor from arguments I
Trk::RotatedDiamondBounds::RotatedDiamondBounds(double minhalex,
                                                double medhalex,
                                                double maxhalex,
                                                double haley1,
                                                double haley2)
  : m_boundValues(RotatedDiamondBounds::bv_length, 0.)
  , m_alpha1(0.)
  , m_alpha2(0.)
{
  m_boundValues[RotatedDiamondBounds::bv_minHalfX] = minhalex;
  m_boundValues[RotatedDiamondBounds::bv_medHalfX] = medhalex;
  m_boundValues[RotatedDiamondBounds::bv_maxHalfX] = maxhalex;
  m_boundValues[RotatedDiamondBounds::bv_halfY1] = haley1;
  m_boundValues[RotatedDiamondBounds::bv_halfY2] = haley2;
  if (minhalex > maxhalex)
    swap(m_boundValues[RotatedDiamondBounds::bv_minHalfX], m_boundValues[RotatedDiamondBounds::bv_maxHalfX]);
  initCache();
}

bool
Trk::RotatedDiamondBounds::operator==(const Trk::SurfaceBounds& sbo) const
{
  // check the type first not to compare apples with oranges
  const Trk::RotatedDiamondBounds* diabo = dynamic_cast<const Trk::RotatedDiamondBounds*>(&sbo);
  if (!diabo)
    return false;
  return (m_boundValues == diabo->m_boundValues);
}

// checking if inside bounds (Full symmetrical Diamond)
bool
Trk::RotatedDiamondBounds::inside(const Amg::Vector2D& locpo, double tol1, double tol2) const
{
  return this->insideFull(locpo, tol1, tol2);
}

// checking if inside bounds (Full symmetrical Diamond)
bool
Trk::RotatedDiamondBounds::insideFull(const Amg::Vector2D& locpo, double tol1, double tol2) const
{
  // the cases:
  // (0)
  if (!m_boundValues[RotatedDiamondBounds::bv_halfY1] && !m_boundValues[RotatedDiamondBounds::bv_minHalfX])
    return false;
  // (1)
  if (locpo[Trk::locX] < -2. * m_boundValues[RotatedDiamondBounds::bv_halfY1] - tol2)
    return false;
  if (locpo[Trk::locX] > 2. * m_boundValues[RotatedDiamondBounds::bv_halfY2] + tol2)
    return false;
  // (2)
  if (fabs(locpo[Trk::locY]) > (m_boundValues[RotatedDiamondBounds::bv_medHalfX] + tol1))
    return false;
  // (3)
  if (fabs(locpo[Trk::locY]) <
      (fmin(m_boundValues[RotatedDiamondBounds::bv_minHalfX], m_boundValues[RotatedDiamondBounds::bv_maxHalfX]) - tol1))
    return true;
  // (4)
  /** use basic calculation of a straight line */
  if (locpo[Trk::locX] < 0) {
    double k =
      m_boundValues[RotatedDiamondBounds::bv_halfY1] > 0.
        ? (m_boundValues[RotatedDiamondBounds::bv_medHalfX] - m_boundValues[RotatedDiamondBounds::bv_minHalfX]) / 2 /
            m_boundValues[RotatedDiamondBounds::bv_halfY1]
        : 0.;
    return (fabs(locpo[Trk::locY]) <= m_boundValues[RotatedDiamondBounds::bv_medHalfX] - k * fabs(locpo[Trk::locX]));
  }
    double k =
      m_boundValues[RotatedDiamondBounds::bv_halfY2] > 0.
        ? (m_boundValues[RotatedDiamondBounds::bv_medHalfX] - m_boundValues[RotatedDiamondBounds::bv_maxHalfX]) / 2 /
            m_boundValues[RotatedDiamondBounds::bv_halfY2]
        : 0.;
    return (fabs(locpo[Trk::locY]) <= m_boundValues[RotatedDiamondBounds::bv_medHalfX] - k * fabs(locpo[Trk::locX]));

}

// opening angle in point A
double
Trk::RotatedDiamondBounds::alpha1() const
{
  return m_alpha1;
}

// opening angle in point A'
double
Trk::RotatedDiamondBounds::alpha2() const
{
  return m_alpha2;
}

double
Trk::RotatedDiamondBounds::minDistance(const Amg::Vector2D& pos) const
{
  const int Np = 6;

  double y1 = 2. * m_boundValues[RotatedDiamondBounds::bv_halfY1];
  double y2 = 2. * m_boundValues[RotatedDiamondBounds::bv_halfY2];

  double X[6] = { -m_boundValues[RotatedDiamondBounds::bv_minHalfX], -m_boundValues[RotatedDiamondBounds::bv_medHalfX],
                  -m_boundValues[RotatedDiamondBounds::bv_maxHalfX], m_boundValues[RotatedDiamondBounds::bv_maxHalfX],
                  m_boundValues[RotatedDiamondBounds::bv_medHalfX],  m_boundValues[RotatedDiamondBounds::bv_minHalfX] };
  double Y[6] = { -y1, 0., y2, y2, 0., -y1 };

  double dm = 1.e+20;
  double Ao = 0.;
  bool in = true;

  for (int i = 0; i != Np; ++i) {

    int j = i + 1;
    if (j == Np)
      j = 0;
    // interchange locx and locy
    double x = X[i] - pos[1];
    double y = Y[i] - pos[0];
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
  if (in)
    return -sqrt(dm);
  return sqrt(dm);
}

// ostream operator overload

MsgStream&
Trk::RotatedDiamondBounds::dump(MsgStream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::RotatedDiamondBounds:  (minHlenghtX, medHlengthX, maxHlengthX, hlengthY1, hlengthY2 ) = ";
  sl << "(" << m_boundValues[RotatedDiamondBounds::bv_minHalfX] << ", "
     << m_boundValues[RotatedDiamondBounds::bv_medHalfX] << ", " << m_boundValues[RotatedDiamondBounds::bv_maxHalfX]
     << ", " << m_boundValues[RotatedDiamondBounds::bv_halfY1] << ", " << m_boundValues[RotatedDiamondBounds::bv_halfY2]
     << ")";
  sl << std::setprecision(-1);
  return sl;
}

std::ostream&
Trk::RotatedDiamondBounds::dump(std::ostream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::RotatedDiamondBounds:  (minHlenghtX, medHlengthX, maxHlengthX, hlengthY1, hlengthY2 ) = ";
  sl << "(" << m_boundValues[RotatedDiamondBounds::bv_minHalfX] << ", "
     << m_boundValues[RotatedDiamondBounds::bv_medHalfX] << ", " << m_boundValues[RotatedDiamondBounds::bv_maxHalfX]
     << ", " << m_boundValues[RotatedDiamondBounds::bv_halfY1] << ", " << m_boundValues[RotatedDiamondBounds::bv_halfY2]
     << ")";
  sl << std::setprecision(-1);
  return sl;
}
