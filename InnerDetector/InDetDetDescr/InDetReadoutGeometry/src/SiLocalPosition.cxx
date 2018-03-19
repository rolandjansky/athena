/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiLocalPosition.cxx
//   Implementation file for class SiLocalPosition
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 2.1 01/08/2001 David Calvet
///////////////////////////////////////////////////////////////////

#include "InDetReadoutGeometry/SiLocalPosition.h"

namespace InDetDD {

// Implicit constructor:
SiLocalPosition::SiLocalPosition() :
  m_xEta(0),
  m_xPhi(0),
  m_xDepth(0)
{}

// Copy constructor:
SiLocalPosition::SiLocalPosition(const SiLocalPosition &position) :
  m_xEta(position.m_xEta),
  m_xPhi(position.m_xPhi),
  m_xDepth(position.m_xDepth)
{}

// Constructor with parameters:
SiLocalPosition::SiLocalPosition(const double xEta,const double xPhi,
				 const double xDepth) :
  m_xEta(xEta),
  m_xPhi(xPhi),
  m_xDepth(xDepth)
{}

SiLocalPosition::SiLocalPosition(const Amg::Vector2D &position)
  : m_xEta(position[Trk::distEta]),
    m_xPhi(position[Trk::distPhi]),
    m_xDepth(0)
{}
  

SiLocalPosition::operator Amg::Vector2D(void) const
{
  return Amg::Vector2D(m_xPhi, m_xEta);
}

// Assignment operator:
SiLocalPosition &SiLocalPosition::operator=(const SiLocalPosition &position)
{
  if (this!=&position) {
    m_xEta=position.m_xEta;
    m_xPhi=position.m_xPhi;
    m_xDepth=position.m_xDepth;
  } else {}
  return *this;
}

// addition of positions:
SiLocalPosition &SiLocalPosition::operator+=(const SiLocalPosition &position)
{
  m_xEta+=position.m_xEta;
  m_xPhi+=position.m_xPhi;
  m_xDepth+=position.m_xDepth;  
  return *this;
}

// scaling:
SiLocalPosition &SiLocalPosition::operator*=(const double factor)
{
  m_xEta*=factor;
  m_xPhi*=factor;
  m_xDepth*=factor;
  return *this;
}

// scaling:
SiLocalPosition &SiLocalPosition::operator/=(const double factor)
{
  if (0!=factor) {
    m_xEta/=factor;
    m_xPhi/=factor;
    m_xDepth/=factor;
  } else {}
  return *this;
}

SiLocalPosition operator+(const SiLocalPosition &position1,
			  const SiLocalPosition &position2)
{
  SiLocalPosition result(position1);
  result+=position2;
  return result;
}

SiLocalPosition operator*(const SiLocalPosition &position,const double factor)
{
  SiLocalPosition result(position);
  result*=factor;
  return result;
}

SiLocalPosition operator/(const SiLocalPosition &position,const double factor)
{
  SiLocalPosition result(position);
  result/=factor;
  return result;
}

} // namespace InDetDD
