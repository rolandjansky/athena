/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_ForwardModuleSideDesign.cxx
//   Implementation file for class SCT_ForwardModuleSideDesign
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.16 29/10/2001 Alessandro Fornaini
// Modified: Grant Gorfine
///////////////////////////////////////////////////////////////////

#include "SCT_ReadoutGeometry/SCT_ForwardModuleSideDesign.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "InDetReadoutGeometry/SiDiodesParameters.h"

namespace InDetDD {

// Constructor with parameters:
SCT_ForwardModuleSideDesign::SCT_ForwardModuleSideDesign(const double thickness,
							 const int crystals,
                                                         const int diodes,
                                                         const int cells,
                                                         const int shift,
							 const bool swapStripReadout,
                                                         InDetDD::CarrierType carrierType,
							 const double radius1,
                                                         const double halfHeight1,
                                                         const double radius2,          
                                                         const double halfHeight2, 
                                                         const double step,
                                                         const double etaCenter,
                                                         const double phiCenter,
							 int readoutSide) :
  SCT_ModuleSideDesign(thickness,
		       true,  // phi 
		       false, // eta: For trapezoid we cant swap z (xEta) axis.
		       true,  // depth
		       crystals,diodes,cells,shift,swapStripReadout,carrierType,readoutSide),
  m_geometry(radius1,halfHeight1,radius2,halfHeight2,diodes,step,crystals),
  m_frame(etaCenter,phiCenter,radius1)
{ 
  if (crystals > 1) {
    double radius = 0.5 * (radius1 + radius2 + halfHeight2 - halfHeight1);
    m_frame=SCT_ForwardFrameTransformation(etaCenter,phiCenter,radius);
  }
  m_bounds = Trk::TrapezoidBounds(0.5*minWidth(), 0.5*maxWidth(), 0.5*length());
}

void
SCT_ForwardModuleSideDesign::distanceToDetectorEdge(const SiLocalPosition & localPosition,
						   double & etaDist, double & phiDist) const
{
  m_geometry.distanceToDetectorEdge(localPosition,
				    m_frame.polarFromCartesian(localPosition),
				    etaDist, phiDist);
}

bool 
SCT_ForwardModuleSideDesign::nearBondGap(const SiLocalPosition & localPosition, double etaTol) const
{
  return m_geometry.nearBondGap(localPosition, etaTol);
}  

// check if the position is in active area
bool SCT_ForwardModuleSideDesign::inActiveArea(const SiLocalPosition &chargePos, bool checkBondGap) const
{
  //const SCT_ForwardPolarPosition polar=m_frame.polarFromCartesian(chargePos);
  //return m_geometry.inActiveArea(m_frame.polarFromCartesian(chargePos));
  return m_geometry.inActiveArea(chargePos, checkBondGap);
}

// give distance to the nearest diode in units of pitch, from 0.0 to 0.5, 
// this method should be fast as it is called for every surface charge
// in the SCT_SurfaceChargesGenerator
double SCT_ForwardModuleSideDesign::scaledDistanceToNearestDiode(const SiLocalPosition &chargePos) const
{
  const SCT_ForwardPolarPosition polar=m_frame.polarFromCartesian(chargePos);
  return m_geometry.scaledDistanceToNearestDiode(polar);
}


// give the strip pitch (dependence on position needed for forward)  
double SCT_ForwardModuleSideDesign::stripPitch(const SiLocalPosition & chargePos) const
{
  const SCT_ForwardPolarPosition polar=m_frame.polarFromCartesian(chargePos);
  // No longer make inActive check
  // if (!m_geometry.inActiveArea(polar)) return 0;
  return m_geometry.stripPitch(polar);
}

// give the strip pitch (dependence on position needed for forward)  
double SCT_ForwardModuleSideDesign::stripPitch() const
{
  // Use center.
  const SCT_ForwardPolarPosition polarCenter(m_frame.radius(), 0);
  // No longer make inActive check
  //  if (!m_geometry.inActiveArea(polar)) return 0;
  return m_geometry.stripPitch(polarCenter);
}


 
// this method returns the extremities of a strip passing by the point
std::pair<SiLocalPosition,SiLocalPosition> SCT_ForwardModuleSideDesign::endsOfStrip(const SiLocalPosition &position) const
{
  const SCT_ForwardPolarPosition polarPosition = 
    m_frame.polarFromCartesian(position);
  const double theta = polarPosition.theta();
  const double innerRadius = (m_geometry.radius1()-m_geometry.halfHeight1())/cos(theta);
  double outerRadius;
  if (crystals()==1) {
    outerRadius = (m_geometry.radius1()+m_geometry.halfHeight1())/cos(theta);
  } else {
    outerRadius = (m_geometry.radius2()+m_geometry.halfHeight2())/cos(theta);
  }    
  const SCT_ForwardPolarPosition innerPointPolar = SCT_ForwardPolarPosition(innerRadius,theta);
  const SCT_ForwardPolarPosition outerPointPolar = SCT_ForwardPolarPosition(outerRadius,theta);
  const SiLocalPosition innerPoint = m_frame.cartesianFromPolar(innerPointPolar);
  const SiLocalPosition outerPoint = m_frame.cartesianFromPolar(outerPointPolar);
    
  return std::pair<SiLocalPosition,SiLocalPosition>(innerPoint,outerPoint);
}


// method for stereo angle computation - returns a vector parallel to the 
// strip being hit
HepGeom::Vector3D<double> SCT_ForwardModuleSideDesign::phiMeasureSegment(const SiLocalPosition &position) const
{
  const SCT_ForwardPolarPosition secondPolarPosition(0,0);
  const SiLocalPosition secondPosition = m_frame.cartesianFromPolar(secondPolarPosition); 
  
  HepGeom::Vector3D<double> notUnitVector;
  notUnitVector[phiAxis()]=position.xPhi()-secondPosition.xPhi();
  notUnitVector[etaAxis()]=position.xEta()-secondPosition.xEta();

  return notUnitVector.unit();
}
	    
double SCT_ForwardModuleSideDesign::length() const
{
  return m_geometry.length();
}

double SCT_ForwardModuleSideDesign::width() const
{
  return m_geometry.width();
}

double SCT_ForwardModuleSideDesign::minWidth() const
{
  return m_geometry.minWidth();
}

double SCT_ForwardModuleSideDesign::maxWidth() const
{
  return m_geometry.maxWidth();
}

double SCT_ForwardModuleSideDesign::deadAreaLength() const
{
  return m_geometry.deadAreaLength();
}

double SCT_ForwardModuleSideDesign::deadAreaUpperBoundary() const
{
  return m_geometry.deadAreaUpperBoundary();
}

double SCT_ForwardModuleSideDesign::deadAreaLowerBoundary() const
{
  return m_geometry.deadAreaLowerBoundary();
} 

//this method returns the position of the centre of a strips
SiLocalPosition 
SCT_ForwardModuleSideDesign::localPositionOfCell(const SiCellId & cellId) const
{
  // This method returns the position of the centre of the cell
  
  // NB. No check is made that cellId is valid or in the correct range.

  int strip =  cellId.strip();

  double clusterCenter = strip - 0.5*cells() + 0.5;

  const double theta = m_geometry.angularPitch()*clusterCenter;
  const double r =  radius() / cos(theta);

  // positions in polar coordinates and then cartesian
  const SCT_ForwardPolarPosition polarPosition(r, theta);
  return m_frame.cartesianFromPolar(polarPosition);
}

//this method returns the position of the centre of the cluster of strips
SiLocalPosition 
SCT_ForwardModuleSideDesign::localPositionOfCluster(const SiCellId & cellId,
						    int clusterSize) const
{
  // This method returns the position of the centre of the cluster starting at cellId.strip()
  
  // NB. No check is made that cellId is valid or in the correct range.

  if (clusterSize < 1) clusterSize = 1;

  int strip =  cellId.strip();

  double clusterCenter = strip - 0.5*cells() + 0.5;
  if (clusterSize>1) clusterCenter += 0.5 * (clusterSize-1);

  const double theta = angularPitch()*clusterCenter;
  const double r = radius() / cos(theta);

  // positions in polar coordinates and then cartesian
  const SCT_ForwardPolarPosition polarPosition(r,theta);
  return m_frame.cartesianFromPolar(polarPosition);
}

SiDiodesParameters 
SCT_ForwardModuleSideDesign::parameters(const SiCellId & cellId) const
{
  int strip =  cellId.strip();

  double clusterCenter = strip - 0.5*cells() + 0.5;

  const double theta = m_geometry.angularPitch()*clusterCenter;
  const double r =  radius() / cos(theta);

  // positions in polar coordinates and then cartesian
  const SCT_ForwardPolarPosition polarPosition(r, theta);
  SiLocalPosition center = m_frame.cartesianFromPolar(polarPosition);

  // The strip is not rectangular put we return pitch at radius.
  double stripWidth = angularPitch()*r;
  double stripLength = length() / cos(theta);

  SiLocalPosition width(stripLength, stripWidth);
  return SiDiodesParameters(center, width);
}


SiCellId 
SCT_ForwardModuleSideDesign::cellIdOfPosition(const SiLocalPosition & localPosition) const
{
  // NB We do not distinguish between the two crystals anymore.
  // Check if we are in the active region. No bondgap check.
  if (!inActiveArea(localPosition, false)) return SiCellId(); // return an invalid id

  const SCT_ForwardPolarPosition polar = m_frame.polarFromCartesian(localPosition);
  double theta = polar.theta();

  double dstrip = theta/angularPitch() + 0.5*diodes();

  if (dstrip < 0) return SiCellId(); // return an invalid id
  int strip = static_cast<int>(dstrip);
  if (strip > diodes())  return SiCellId();// return an invalid id if strip # greater 
                                           // than number of diodes.
  return SiCellId(strip-shift()); // strip numbering starts from first readout strip. 
                                   // Those to the left will have negative numbers.
}


DetectorShape 
SCT_ForwardModuleSideDesign::shape() const
{
  return InDetDD::Trapezoid;
}

const Trk::SurfaceBounds & 
SCT_ForwardModuleSideDesign::bounds() const
{
  return m_bounds;
}

} // namespace InDetDD
