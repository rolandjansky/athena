/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TriangleBounds.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSurfaces/TriangleBounds.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <iomanip>
#include <iostream>

// default constructor
Trk::TriangleBounds::TriangleBounds()
  : m_boundValues(TriangleBounds::bv_length, 0.)
{}

// rectangle constructor - float constructor
Trk::TriangleBounds::TriangleBounds(const std::vector<std::pair<float, float>>& vertices)
  : m_boundValues(TriangleBounds::bv_length, 0.)
{
  size_t ib = 0;
  for (const std::pair<float, float>& p : vertices) {
    m_boundValues[2 * ib] = p.first;
    m_boundValues[2 * ib + 1] = p.second;
    if (ib == 2)
      break;
    ++ib;
  }
}

// rectangle constructor - double constructor
Trk::TriangleBounds::TriangleBounds(const std::vector<std::pair<double, double>>& vertices)
  : m_boundValues(TriangleBounds::bv_length, 0.)
{
  size_t ib = 0;
  for (const std::pair<double, double>& p : vertices) {
    m_boundValues[2 * ib] = p.first;
    m_boundValues[2 * ib + 1] = p.second;
    if (ib == 2)
      break;
    ++ib;
  }
}

// constructor from three points
Trk::TriangleBounds::TriangleBounds(const Amg::Vector2D& p1, const Amg::Vector2D& p2, const Amg::Vector2D& p3)
  : m_boundValues(TriangleBounds::bv_length, 0.)
{
  m_boundValues[TriangleBounds::bv_x1] = p1.x();
  m_boundValues[TriangleBounds::bv_y1] = p1.y();
  m_boundValues[TriangleBounds::bv_x2] = p2.x();
  m_boundValues[TriangleBounds::bv_y2] = p2.y();
  m_boundValues[TriangleBounds::bv_x3] = p3.x();
  m_boundValues[TriangleBounds::bv_y3] = p3.y();
}

bool
Trk::TriangleBounds::operator==(const Trk::SurfaceBounds& sbo) const
{
  // check the type first not to compare apples with oranges
  const Trk::TriangleBounds* tribo = dynamic_cast<const Trk::TriangleBounds*>(&sbo);
  if (!tribo)
    return false;
  return (m_boundValues == tribo->m_boundValues);
}

double
Trk::TriangleBounds::minDistance(const Amg::Vector2D& pos) const
{
  const int Np = 3;

  double X[3] = { m_boundValues[TriangleBounds::bv_x1],
                  m_boundValues[TriangleBounds::bv_x2],
                  m_boundValues[TriangleBounds::bv_x3] };
  double Y[3] = { m_boundValues[TriangleBounds::bv_y1],
                  m_boundValues[TriangleBounds::bv_y2],
                  m_boundValues[TriangleBounds::bv_y3] };

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
Trk::TriangleBounds::dump(MsgStream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::TriangleBounds:  generating vertices (X, Y) " << '\n';
  sl << "(" << m_boundValues[TriangleBounds::bv_x1] << " , " << m_boundValues[TriangleBounds::bv_y1] << ") " << '\n';
  sl << "(" << m_boundValues[TriangleBounds::bv_x2] << " , " << m_boundValues[TriangleBounds::bv_y2] << ") " << '\n';
  sl << "(" << m_boundValues[TriangleBounds::bv_x3] << " , " << m_boundValues[TriangleBounds::bv_y3] << ") ";
  sl << std::setprecision(-1);
  return sl;
}

std::ostream&
Trk::TriangleBounds::dump(std::ostream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::TriangleBounds:  generating vertices (X, Y)";
  sl << "(" << m_boundValues[TriangleBounds::bv_x1] << " , " << m_boundValues[TriangleBounds::bv_y1] << ") " << '\n';
  sl << "(" << m_boundValues[TriangleBounds::bv_x2] << " , " << m_boundValues[TriangleBounds::bv_y2] << ") " << '\n';
  sl << "(" << m_boundValues[TriangleBounds::bv_x3] << " , " << m_boundValues[TriangleBounds::bv_y3] << ") ";
  sl << std::setprecision(-1);
  return sl;
}
