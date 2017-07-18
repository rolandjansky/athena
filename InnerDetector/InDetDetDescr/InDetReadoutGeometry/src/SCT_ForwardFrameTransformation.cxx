/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_ForwardFrameTransformation.cxx
//   Implementation file for class SCT_ForwardFrameTransformation
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.4 09/7/2001 Alessandro Fornaini
///////////////////////////////////////////////////////////////////

#include "InDetReadoutGeometry/SCT_ForwardFrameTransformation.h"
#include "InDetReadoutGeometry/SCT_ForwardPolarPosition.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"
#include <cmath>

namespace InDetDD {

// Copy constructor:
SCT_ForwardFrameTransformation::SCT_ForwardFrameTransformation(const SCT_ForwardFrameTransformation &frame) :
  m_etaCenter(frame.m_etaCenter),
  m_phiCenter(frame.m_phiCenter),
  m_radius(frame.m_radius)
{}

// Constructor with parameters:
SCT_ForwardFrameTransformation::SCT_ForwardFrameTransformation(const double etaCenter,const double phiCenter,const double radius) :
  m_etaCenter(etaCenter),
  m_phiCenter(phiCenter),
  m_radius(radius)
{ 
  //check if parameters are consistent
  if (m_radius<0) m_radius=0;
}


// Assignment operator:
SCT_ForwardFrameTransformation &SCT_ForwardFrameTransformation::operator=(const SCT_ForwardFrameTransformation &frame)
{
  if (this!=&frame) {
    m_etaCenter=frame.m_etaCenter;
    m_phiCenter=frame.m_phiCenter;
    m_radius=frame.m_radius;
  } else {}
  return *this;
}


// create a SCT_ForwardPolarPosition from a SiLocalPosition
SCT_ForwardPolarPosition SCT_ForwardFrameTransformation::polarFromCartesian(const SiLocalPosition &cartesian) const
{ 
  const double deltaPhi = cartesian.xPhi()-m_phiCenter;
  const double deltaEta = cartesian.xEta()-m_etaCenter+m_radius;
  const double r = sqrt(deltaPhi*deltaPhi+deltaEta*deltaEta);
  if (r<=0) return SCT_ForwardPolarPosition();
  const double theta = asin((cartesian.xPhi()-m_phiCenter)/r);
  return SCT_ForwardPolarPosition(r,theta);
}

// create a SiLocalPosition from a SCT_ForwardPolarPosition
SiLocalPosition SCT_ForwardFrameTransformation::cartesianFromPolar(const SCT_ForwardPolarPosition &polar) const
{
  const double phi = m_phiCenter + polar.r()*sin(polar.theta());
  const double eta = m_etaCenter + polar.r()*cos(polar.theta()) - m_radius;
  return SiLocalPosition(eta, phi);
}

} // namespace InDetDD
