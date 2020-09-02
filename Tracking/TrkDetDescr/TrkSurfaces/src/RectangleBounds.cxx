/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RectangleBounds.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSurfaces/RectangleBounds.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <iomanip>
#include <iostream>

// default constructor
Trk::RectangleBounds::RectangleBounds()
  : m_boundValues(RectangleBounds::bv_length, 0.)
{}

// rectangle constructor
Trk::RectangleBounds::RectangleBounds(double halex, double haley)
  : m_boundValues(RectangleBounds::bv_length, 0.)
{
  m_boundValues[RectangleBounds::bv_halfX] = halex;
  m_boundValues[RectangleBounds::bv_halfY] = haley;
}

// copy constructor
Trk::RectangleBounds::RectangleBounds(const RectangleBounds& recbo)
  : Trk::SurfaceBounds()
  , m_boundValues(recbo.m_boundValues)
{}

// destructor
Trk::RectangleBounds::~RectangleBounds() = default;

Trk::RectangleBounds&
Trk::RectangleBounds::operator=(const RectangleBounds& recbo)
{
  if (this != &recbo)
    m_boundValues = recbo.m_boundValues;
  return *this;
}

bool
Trk::RectangleBounds::operator==(const Trk::SurfaceBounds& sbo) const
{
  // check the type first not to compare apples with oranges
  const Trk::RectangleBounds* recbo = dynamic_cast<const Trk::RectangleBounds*>(&sbo);
  if (!recbo)
    return false;
  return (m_boundValues == recbo->m_boundValues);
}

double
Trk::RectangleBounds::minDistance(const Amg::Vector2D& pos) const
{
  double dx = fabs(pos[0]) - m_boundValues[RectangleBounds::bv_halfX];
  double dy = fabs(pos[1]) - m_boundValues[RectangleBounds::bv_halfY];

  if (dx <= 0. || dy <= 0.) {
    if (dx > dy){
      return dx;
    }
    return dy;
  }
  return sqrt(dx * dx + dy * dy);
}

// ostream operator overload
MsgStream&
Trk::RectangleBounds::dump(MsgStream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::RectangleBounds:  (halflenghtX, halflengthY) = "
     << "(" << m_boundValues[RectangleBounds::bv_halfX] << ", " << m_boundValues[RectangleBounds::bv_halfY] << ")";
  sl << std::setprecision(-1);
  return sl;
}

std::ostream&
Trk::RectangleBounds::dump(std::ostream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::RectangleBounds:  (halflenghtX, halflengthY) = "
     << "(" << m_boundValues[RectangleBounds::bv_halfX] << ", " << m_boundValues[RectangleBounds::bv_halfY] << ")";
  sl << std::setprecision(-1);
  return sl;
}
