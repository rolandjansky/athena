/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_ForwardPolarPosition.cxx
//   Implementation file for class SCT_ForwardPolarPosition
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0a 1/6/2001 Class creator tool
///////////////////////////////////////////////////////////////////

#include "InDetReadoutGeometry/SCT_ForwardPolarPosition.h"
#include <new>
#include <sstream>

namespace InDetDD {

// Implicit constructor:
SCT_ForwardPolarPosition::SCT_ForwardPolarPosition() :
  m_r(0),
  m_theta(0)
{}

// Copy constructor:
SCT_ForwardPolarPosition::SCT_ForwardPolarPosition(const SCT_ForwardPolarPosition &polar) :
  m_r(polar.m_r),
  m_theta(polar.m_theta)
{}

// Constructor with parameters:
SCT_ForwardPolarPosition::SCT_ForwardPolarPosition(const double r,const double theta) :
  m_r(r),
  m_theta(theta)
{
  //check if parameters are consistent
  if (m_r<0) m_r=0;
}

// Assignment operator:
SCT_ForwardPolarPosition &SCT_ForwardPolarPosition::operator=(const SCT_ForwardPolarPosition &polar)
{
  if (this!=&polar) {
    m_r=polar.m_r;
    m_theta=polar.m_theta;
  } else {}
  return *this;
}

//Comparison operator
bool SCT_ForwardPolarPosition::operator==(const SCT_ForwardPolarPosition &polar) const
{
  return (m_r==polar.m_r && m_theta==polar.m_theta);
}

//Print method:
std::string SCT_ForwardPolarPosition::print() const
{
  std::ostringstream out;
  out<<"r = "<<m_r<<" theta = "<<m_theta;
  return out.str();
}

} // namespace InDetDD
