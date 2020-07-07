/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_BarrelModuleSideDesign.cxx
//   Implementation file for class SCT_BarrelModuleSideDesign
///////////////////////////////////////////////////////////////////

#include "SCT_ReadoutGeometry/SCT_BarrelModuleSideDesign.h"
#include "InDetReadoutGeometry/SiDiodesParameters.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Vector3D.h"

#include <cmath>

namespace InDetDD {

// Constructor with parameters:
SCT_BarrelModuleSideDesign::SCT_BarrelModuleSideDesign(const double thickness,
						       const int crystals,
						       const int diodes,
						       const int cells,
						       const int shift,
						       const bool swapStripReadout,
						       InDetDD::CarrierType carrierType,
						       const double stripPitch,
						       const double stripLength,
						       const double xEtaStripPatternCentre,
						       const double xPhiStripPatternCentre,
						       const double totalDeadLength,
						       int readoutSide) :
  SCT_ModuleSideDesign(thickness, 
		       true, true, true, // phi,eta,depth axes symmetric
		       crystals,diodes,cells,shift,swapStripReadout,carrierType,readoutSide),
  m_stripPitch(stripPitch),
  m_stripLength(stripLength),
  m_xEtaStripPatternCentre(xEtaStripPatternCentre),
  m_xPhiStripPatternCentre(xPhiStripPatternCentre),
  m_totalDeadLength(totalDeadLength)
{
  m_xPhiAbsSize = 0.5*m_stripPitch*diodes;
  if(crystals==1) {
    m_xEtaAbsSizeLow = m_totalDeadLength*0.5;
    if (m_xEtaAbsSizeLow == 0) m_xEtaAbsSizeLow = -1.0 * CLHEP::mm; // Anything negative will do
    m_xEtaAbsSizeHigh = m_stripLength/2;
  }
  else if(crystals==2) {
    m_xEtaAbsSizeLow = m_totalDeadLength*0.5;
    m_xEtaAbsSizeHigh =  m_stripLength + m_totalDeadLength*0.5; 
  } else {
    std::cout << "wrong number of crystals!\n";
    m_xEtaAbsSizeLow = m_xEtaAbsSizeHigh = m_xPhiAbsSize = 0.0;
  }
  
  m_bounds = Trk::RectangleBounds(0.5*width(), 0.5*length());
}


// Returns distance to nearest detector edge 
// +ve = inside
// -ve = outside
void
SCT_BarrelModuleSideDesign::distanceToDetectorEdge(const SiLocalPosition & localPosition,
						   double & etaDist, double & phiDist) const
{ 
  // As the calculation is symmetric around 0,0 we only have to test it for one side.
  double xEta = std::abs(localPosition.xEta() - m_xEtaStripPatternCentre);
  double xPhi = std::abs(localPosition.xPhi() - m_xPhiStripPatternCentre);

  double xEtaEdge = 0.5 * length();
  double xPhiEdge = 0.5 * width();

  // Distance to top/bottom
  etaDist = xEtaEdge - xEta;
  
  // Distance to right/left edge
  phiDist = xPhiEdge - xPhi;

}


bool SCT_BarrelModuleSideDesign::nearBondGap(const SiLocalPosition & localPosition, double etaTol) const
{
  // Symmetric around xEta = 0 so we can use absolute value.
  if (m_totalDeadLength==0) return false; 
  return ( std::abs(localPosition.xEta()) < 0.5*m_totalDeadLength + etaTol);
}

// check if the position is in active area
bool SCT_BarrelModuleSideDesign::inActiveArea(const SiLocalPosition &chargePos, 
					      bool checkBondGap) const 
{
  // in Phi
  if (std::abs(chargePos.xPhi()-m_xPhiStripPatternCentre) > m_xPhiAbsSize) return false;

  // in Eta
  double relEta = std::abs(chargePos.xEta() - m_xEtaStripPatternCentre);
  if (relEta > m_xEtaAbsSizeHigh) return false;

  // bond gap
  return !(checkBondGap && (relEta < m_xEtaAbsSizeLow));
}


// distance to the nearest diode in units of pitch, from 0.0 to 0.5, 
// this method should be fast as it is called for every surface charge
// in the SCT_SurfaceChargesGenerator
// an active area check, done in the Generator anyway, is removed here
double SCT_BarrelModuleSideDesign::scaledDistanceToNearestDiode(const SiLocalPosition &chargePos) const
{
    double dstrip=std::abs(chargePos.xPhi()-m_xPhiStripPatternCentre)/m_stripPitch;
    dstrip=dstrip-static_cast<double>(int(dstrip))-0.5;
    // the above -0.5 is because we have an even number of strips, centre of detector
    // is in the middle of an interstrip gap
    return std::abs(dstrip);
}

std::pair<SiLocalPosition,SiLocalPosition> SCT_BarrelModuleSideDesign::endsOfStrip(const SiLocalPosition &position) const
{
    // this method returns the ends of the strip
    // assume input xPhi

    SiLocalPosition end1(m_xEtaStripPatternCentre+m_xEtaAbsSizeHigh,position.xPhi());
    SiLocalPosition end2(m_xEtaStripPatternCentre-m_xEtaAbsSizeHigh,position.xPhi());
    
    std::pair<SiLocalPosition,SiLocalPosition> two_ends(end1,end2);
    return two_ends;
}


double SCT_BarrelModuleSideDesign::length() const
{
  return crystals()*m_stripLength+m_totalDeadLength;
}

double SCT_BarrelModuleSideDesign::width() const
{
  return m_stripPitch*diodes();
}

double SCT_BarrelModuleSideDesign::minWidth() const
{
  return width();
}

double SCT_BarrelModuleSideDesign::maxWidth() const
{
  return width();
}


// method for stereo angle computation - returns a vector parallel to the 
// strip being hit
HepGeom::Vector3D<double> SCT_BarrelModuleSideDesign::phiMeasureSegment(const SiLocalPosition& ) const
{
  HepGeom::Vector3D<double> segment;
  segment[etaAxis()]=1;
  return segment;
}

// give length of dead area
double SCT_BarrelModuleSideDesign::deadAreaLength() const
{
  return m_totalDeadLength;
}  

// give upper boundary of dead area (origin in module centre)
double SCT_BarrelModuleSideDesign::deadAreaUpperBoundary() const
{
  return m_totalDeadLength/2;
}
// give lower boundary of dead area (origin in module centre)
double SCT_BarrelModuleSideDesign::deadAreaLowerBoundary() const
{
  return -m_totalDeadLength/2;
}



SiLocalPosition 
SCT_BarrelModuleSideDesign::localPositionOfCell(const SiCellId & cellId) const
{

  // NB. No check is made that cellId is valid or in the correct range.

  int strip =  cellId.strip();

  // center of cluster (in units of number of strips) from detector center
  double clusterCenter = strip - 0.5*cells() + 0.5;

  double xPhi=m_xPhiStripPatternCentre + m_stripPitch * clusterCenter;

  // Return the position.
  // no matter how many crystals we have, xEta of the position is in the centre!
  return SiLocalPosition(m_xEtaStripPatternCentre, xPhi);
}

SiLocalPosition 
SCT_BarrelModuleSideDesign::localPositionOfCluster(const SiCellId & cellId, int clusterSize) const
{
  // This method returns the position of the centre of the cluster starting at cellId.strip()
  
  // NB. No check is made that cellId is valid or in the correct range.

  if (clusterSize < 1) clusterSize = 1;

  int strip =  cellId.strip();

  // center of cluster (in units of number of strips) from detector center
  double clusterCenter = strip - 0.5*cells() + 0.5;
  if (clusterSize>1) clusterCenter += 0.5 * (clusterSize-1);

  double xPhi=m_xPhiStripPatternCentre + m_stripPitch * clusterCenter;

  // Return the position.
  // no matter how many crystals we have, xEta of the position is in the centre!
  return SiLocalPosition(m_xEtaStripPatternCentre, xPhi);
}


SiDiodesParameters 
SCT_BarrelModuleSideDesign::parameters(const SiCellId & cellId) const
{
  // NB. We treat the two crytals as one.
  SiLocalPosition center=localPositionOfCell(cellId);
  double xPhiSize=m_stripPitch;
  double xEtaSize=2*m_xEtaAbsSizeHigh;
  SiLocalPosition width(xEtaSize,xPhiSize);
  return SiDiodesParameters(center,width);
}

SiCellId 
SCT_BarrelModuleSideDesign::cellIdOfPosition(const SiLocalPosition & localPosition) const
{
  // NB We do not distinguish between the two crystals anymore.
  // Check if we are in the active region. No bondgap check.
  if (!inActiveArea(localPosition, false)) return SiCellId(); // return an invalid id
  double xPhi=localPosition.xPhi();
  double dstrip=(xPhi-m_xPhiStripPatternCentre)/m_stripPitch+0.5*diodes();
  if (dstrip < 0) return SiCellId(); // return an invalid id
  int strip = static_cast<int>(dstrip);
  if (strip > diodes())  return SiCellId();// return an invalid id if strip # greater than number of diodes.
  return SiCellId(strip-shift()); // strip numbering starts from first readout strip. 
                                   // Those to the left will have negative numbers.

}

const Trk::SurfaceBounds & 
SCT_BarrelModuleSideDesign::bounds() const
{
  return m_bounds;
}

} // namespace InDetDD
