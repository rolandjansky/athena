/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_ForwardModuleSideGeometry.cxx
//   Implementation file for class SCT_ForwardModuleSideGeometry
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.6 09/7/2001 Alessandro Fornaini
///////////////////////////////////////////////////////////////////

#include "SCT_ReadoutGeometry/SCT_ForwardModuleSideGeometry.h"
#include <cmath>

namespace InDetDD {

  using std::cos;
  using std::sin;

// Constructor with parameters:
SCT_ForwardModuleSideGeometry::SCT_ForwardModuleSideGeometry(const double radius1,
                                                             const double halfHeight1, 
                                                             const double radius2,                    
							     const double halfHeight2,
                                                             const int strips,
                                                             const double step,
                                                             const int nCrystals ) :
  m_radius1(radius1),
  m_halfHeight1(halfHeight1),
  m_radius2(radius2),
  m_halfHeight2(halfHeight2),
  m_strips(strips),
  m_step(step) ,
  m_nCrystals(nCrystals),
  m_leftSide(step*strips*(-0.5)),
  m_rightSide(step*strips*0.5),
  m_upperSide1(radius1+halfHeight1),
  m_lowerSide1(radius1-halfHeight1),
  m_upperSide2(radius2+halfHeight2),
  m_lowerSide2(radius2-halfHeight2)


{
  if (m_radius1<0) m_radius1=0.0;
  if (m_halfHeight1<0) m_halfHeight1=0.0;
  if (m_radius2<0) m_radius2=0.0;
  if (m_halfHeight2<0) m_halfHeight2=0.0; 
  if (m_strips<0) m_strips=0;
  if (m_step<0) m_step=0.0;
  if (m_nCrystals==0) m_nCrystals=1; 
  if (m_nCrystals<2) {
    m_radius2=0;
    m_halfHeight2=0;
  }

  if (m_nCrystals==1) {
    m_radius = m_radius1;
    m_halfLength = m_halfHeight1;
  } else {
    m_radius = 0.5 * (m_radius1 + m_radius2 + m_halfHeight2 - m_halfHeight1);
    m_halfLength = 0.5 * (m_radius2 + m_halfHeight2 - m_radius1 + m_halfHeight1);
  }

  m_tangent = tan(m_rightSide);

  if (m_nCrystals==1) {
    m_deadRegionUpper = 0; 
    m_deadRegionLower = 0; 
  } else {
    m_deadRegionUpper = m_radius2 - m_halfHeight2 - m_radius;
    m_deadRegionLower = m_radius1 + m_halfHeight1 - m_radius;
  }
  
}



// Returns distance to nearest detector edge 
// +ve = inside
// -ve = outside
void
SCT_ForwardModuleSideGeometry::distanceToDetectorEdge(const SiLocalPosition & localPosition,
						      const SCT_ForwardPolarPosition & polar,
						      double & etaDist, double & phiDist) const
{ 

  // Distance to top/bottom.
  // Calculation symmetric around zero so we can use abs.
  etaDist = m_halfLength - std::abs(localPosition.xEta());

  // For phi distance use r*phiDiff.
  phiDist = polar.r() * (m_rightSide - std::abs(polar.theta()));

}


bool SCT_ForwardModuleSideGeometry::nearBondGap(const SiLocalPosition & localPosition, double etaTol) const
{
  if (m_nCrystals == 1) return false; // No bond gap (eg for truncated middle detectors)
  
  return ((localPosition.xEta() < m_deadRegionUpper + etaTol ) &&
	  (localPosition.xEta() > m_deadRegionLower - etaTol));
    
}  


// check if the position is in active area
bool SCT_ForwardModuleSideGeometry::inActiveArea(const SiLocalPosition &chargePos, 
						 bool checkBondGap) const
{
  double xEta = chargePos.xEta();
  double xPhi = chargePos.xPhi();

  if (std::abs(xEta) > m_halfLength) return false;

  if (checkBondGap && m_nCrystals > 1) {
    // Check if in bond gap
    if (xEta > m_deadRegionLower && xEta < m_deadRegionUpper) return false;
  }
  
  // edge at xEta
  double edge = (xEta + m_radius) * m_tangent;
  
  return (std::abs(xPhi) < edge);
}

// DEPRECATED
// check if the position is in active area
bool SCT_ForwardModuleSideGeometry::inActiveArea(const SCT_ForwardPolarPosition
						 &polarPos ) const
{
  bool conditionTheta=false;
  bool conditionEtaOne=false;
  bool conditionEtaTwo=false;
  double polarR = polarPos.r();
  double polarTheta = polarPos.theta();
  double cosPolarTheta = cos(polarTheta);
  

  if (polarR*cosPolarTheta<m_upperSide1 &&
      polarR*cosPolarTheta>m_lowerSide1)
    conditionEtaOne=true;
  if (polarR*cosPolarTheta<m_upperSide2 &&
      polarR*cosPolarTheta>m_lowerSide2)
    conditionEtaTwo=true;
  if (polarTheta<m_rightSide &&
      polarTheta>m_leftSide) conditionTheta=true;
  return (conditionEtaOne || conditionEtaTwo) && conditionTheta;
}   

// give the strip pitch (dependence on position needed for forward)  
double SCT_ForwardModuleSideGeometry::stripPitch(const SCT_ForwardPolarPosition
						 &polarPos) const
{
  return 2.0*polarPos.r()*std::abs(sin(m_step*0.5));
}

// give distance to the nearest diode in units of pitch, from 0.0 to 0.5, 
// this method should be fast as it is called for every surface charge
// in the SCT_SurfaceChargesGenerator
double SCT_ForwardModuleSideGeometry::scaledDistanceToNearestDiode(const SCT_ForwardPolarPosition &polarPos) const
{
  int stripIndex;
  if (polarPos.theta()>0) stripIndex = 
			    static_cast<int>(polarPos.theta()/m_step);
  else stripIndex = static_cast<int>(polarPos.theta()/m_step-1);
  const double thetaStrip = (stripIndex+0.5)*m_step;
  const double pitch = 2.0*polarPos.r()*std::abs(sin(m_step*0.5));
  return polarPos.r()*std::abs(sin(polarPos.theta()-thetaStrip))/pitch;
}  

// Method to calculate length of a module
double SCT_ForwardModuleSideGeometry::length() const
{
  return 2 * m_halfLength;
}

// Method to calculate average width of a module
double SCT_ForwardModuleSideGeometry::width() const
{
    return 2 * m_radius * m_tangent;
}

// Method to calculate minimum width of a module
double SCT_ForwardModuleSideGeometry::minWidth() const
{
    return 2 * m_lowerSide1 * m_tangent;
}

// Method to calculate maximum width of a module
double SCT_ForwardModuleSideGeometry::maxWidth() const
{
  if (m_nCrystals==1) {
    return 2 * m_upperSide1 * m_tangent;
  }
  else {
    return 2 * m_upperSide2 * m_tangent;
  }
}

// give length of dead area
double SCT_ForwardModuleSideGeometry::deadAreaLength() const
{
  if (m_nCrystals==1) { 
    return 0;
  } else {
    return m_deadRegionUpper - m_deadRegionLower;
  }
  
}


// give upper boundary of dead area
double SCT_ForwardModuleSideGeometry::deadAreaUpperBoundary() const
{
  return m_deadRegionUpper;
}

// give lower boundary of dead area
double SCT_ForwardModuleSideGeometry::deadAreaLowerBoundary() const
{
  return m_deadRegionLower;
}

} // namespace InDetDD
