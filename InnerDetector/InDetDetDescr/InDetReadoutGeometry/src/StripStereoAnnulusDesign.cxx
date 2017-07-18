/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdexcept>
#include <algorithm> // For upper_bound
#include <iterator>  // for std::distance()
#include <cmath>
#include "InDetReadoutGeometry/StripStereoAnnulusDesign.h"
#include "Identifier/Identifier.h"
#include "TrkSurfaces/AnnulusBounds.h"

using namespace std;

namespace InDetDD {
StripStereoAnnulusDesign::StripStereoAnnulusDesign(const SiDetectorDesign::Axis stripDirection,
                               const SiDetectorDesign::Axis thicknessDirection,
                               const double thickness,
                               const int readoutSide,
                               const InDetDD::CarrierType carrier,
                               const int nRows,
                               const vector<int> nStrips,
                               const vector<double> pitch,
                               const vector<double> stripStartRadius,
                               const vector<double> stripEndRadius,
                               const double stereoAngle,
                               const double centreR) :
    SCT_ModuleSideDesign(thickness, true, true, true, 1, 0, 0, 0, false, carrier,
                         readoutSide, stripDirection, thicknessDirection),
    m_nRows(nRows),
    m_nStrips(nStrips),
    m_pitch(pitch),
    m_stripStartRadius(stripStartRadius),
    m_stripEndRadius(stripEndRadius),
    m_stereo(stereoAngle),
    m_R(centreR),
    m_lengthBF(2. * centreR * sin(stereoAngle / 2.)) // Eq. 5 p. 7
{
    if (nRows < 0) {
        throw std::runtime_error(
                  "ERROR: StripStereoAnnulusDesign called with negative number of rows");
    }

    if (pitch.size() != (unsigned) nRows) {
        throw std::runtime_error(
                  "ERROR: StripStereoAnnulusDesign called with insufficiant pitch values for no. of rows");
    }

    if (stripStartRadius.size() != (unsigned) nRows) {
        throw std::runtime_error(
                  "ERROR: StripStereoAnnulusDesign called with insufficiant strip start-positions for no. of rows");
    }

    if (stripEndRadius.size() != (unsigned) nRows) {
        throw std::runtime_error(
                  "ERROR: StripStereoAnnulusDesign called with insufficiant strip end-positions for no. of rows");
    }

    int startStrip = 0;
    for (int r = 0; r < nRows; ++r) {
        m_firstStrip.push_back(startStrip);
        startStrip += m_nStrips[r];
    }
    int totalStrips = startStrip;
    m_firstStrip.push_back(totalStrips); // Add one more as total number of strips, so you can safely use row + 1 in a loop

    m_scheme.setCells(totalStrips);
    m_scheme.setDiodes(totalStrips);

// AnnulusBounds(double minR, double maxR, double R, double phi, double phiS)
    m_bounds = new Trk::AnnulusBounds(m_stripStartRadius[0], m_stripEndRadius.back(), m_R, m_nStrips[0] * m_pitch[0], m_stereo);
}

StripStereoAnnulusDesign::~StripStereoAnnulusDesign() {
    delete m_bounds;
}

HepGeom::Point3D<double> StripStereoAnnulusDesign::sensorCenter() const {
    return HepGeom::Point3D<double>(m_R, 0., 0.);
}

double StripStereoAnnulusDesign::sinStripAngleReco(double phiCoord, double etaCoord) const {
//
//    Transform to strip frame SF (eq. 36 in ver G, combined with eq. 2 since we are in beam frame)
//
    double x = etaCoord;
    double y = phiCoord;
    double xSF = cos(-m_stereo) * (x - m_R) - sin(-m_stereo) * y + m_R;
    double ySF = sin(-m_stereo) * (x - m_R) + cos(-m_stereo) * y;
    double phiPrime = atan2(ySF, xSF);

    // The minus sign below is because this routine is called by tracking software, which swaps x and y, then measures angles from y 
    // to x
    return -sin(phiPrime + m_stereo);
}

void StripStereoAnnulusDesign::getStripRow(SiCellId cellId, int *strip2D, int *rowNum) const {
    int strip1D = cellId.phiIndex();
    *rowNum = row(strip1D);
    *strip2D = strip1D - m_firstStrip[*rowNum];
    return;
}

int  StripStereoAnnulusDesign::strip1Dim(int strip, int row) const {

    return m_firstStrip[row] + strip;
}

void StripStereoAnnulusDesign::neighboursOfCell(const SiCellId &cellId, std::vector<SiCellId> &neighbours) const {


    neighbours.clear();

    if (!cellId.isValid()) {
        return;
    }

    int strip, row;
    getStripRow(cellId, &strip, &row);
    int stripM = strip - 1;
    int stripP = strip + 1;

    if (stripM >= m_firstStrip[row]) {
        neighbours.push_back(stripM);
    }
    if (stripP < m_firstStrip[row] + m_nStrips[row]) {
        neighbours.push_back(stripP);
    }

    return;
}

const Trk::SurfaceBounds &StripStereoAnnulusDesign::bounds() const {

    return *m_bounds;
}

SiCellId StripStereoAnnulusDesign::cellIdOfPosition(SiLocalPosition const &pos) const {
//
//    Find the row
//
    double r = pos.r();
    if (r < m_stripStartRadius[0] || r >= m_stripEndRadius.back()) { 
        return SiCellId(); // return an invalid id
    }

    vector<double>::const_iterator endPtr = upper_bound(m_stripStartRadius.begin(), m_stripStartRadius.end(), r);
    int row = distance(m_stripStartRadius.begin(), endPtr) - 1;
    // Following should never happen, check is done on r above
    if (row < 0 || row >= m_nRows) {
        cout << "ERROR! cellIdOfPosition: bad row = " << row << " for r = " << r << endl;
        return SiCellId(); // return an invalid id
    }
//
//    Find the strip
//
    double x = pos.xEta();
    double y = pos.xPhi();
//
//    Transform to strip frame SF (eq. 36 in ver G, combined with eq. 2 since we are in beam frame)
//
    double xSF = cos(-m_stereo) * (x - m_R) - sin(-m_stereo) * y + m_R;
    double ySF = sin(-m_stereo) * (x - m_R) + cos(-m_stereo) * y;
    double phiPrime = atan2(ySF, xSF); 
    int strip = floor(phiPrime / m_pitch[row]) + m_nStrips[row] / 2.0;
    if (strip < 0) { // Outside
        return SiCellId(); // return an invalid id
    }
    if (strip >= m_nStrips[row]) { // Outside
        return SiCellId(); // return an invalid id
    }

    int strip1D = strip1Dim(strip, row);
    return SiCellId(strip1D, 0);
}




	
void StripStereoAnnulusDesign::closestStripRowOfPosition(SiLocalPosition const &pos, int & strip, int & row )  const  {	   

  double r = pos.r()                                                  	;
	    
  if (r < m_stripStartRadius[0] )       r = m_stripStartRadius[0]     	; 
  if (r >= m_stripEndRadius.back())     r = m_stripEndRadius.back()   	;
	  	
  vector<double>::const_iterator endPtr = upper_bound( m_stripStartRadius.begin(), m_stripStartRadius.end(), r);
  row = distance( m_stripStartRadius.begin(), endPtr) - 1              	;

  // Following should never happen, check is done on r above
  if (row < 0         ) row = 0                                       	;
  if (row >= m_nRows  ) row = (m_nRows-1)                            	;
	  	       
  //
  //    Find the strip   
  //
  double x = pos.xEta();
  double y = pos.xPhi();
  //
  //    Transform to strip frame SF (eq. 36 in ver G, combined with eq. 2 since we are in beam frame)
  //
  double xSF = cos( - m_stereo ) * (x - m_R) - sin(-m_stereo) * y + m_R	;
  double ySF = sin( - m_stereo ) * (x - m_R) + cos(-m_stereo) * y    	;
  double phiPrime = atan2(ySF, xSF)                                   	;
	  
  strip = floor(phiPrime / m_pitch[row]) + m_nStrips[row] / 2.0        	;
  if (strip < 0)                      strip  = 0                      	;
  if (strip >= m_nStrips[row])          strip  = (m_nStrips[row]-1)    	;       
}
	  	






SiLocalPosition StripStereoAnnulusDesign::localPositionOfCell(SiCellId const &cellId) const {
    int strip, row;
    getStripRow(cellId, &strip, &row);
    double r = (m_stripEndRadius[row] + m_stripStartRadius[row]) / 2.;
    return stripPosAtR(strip, row, r);
}







double StripStereoAnnulusDesign::localModuleCentreRadius() const {
  return m_R;
}

SiLocalPosition StripStereoAnnulusDesign::stripPosAtR(int strip, int row, double r) const {

    double phiPrime = (strip - m_nStrips[row] / 2. + 0.5) * m_pitch[row];

    double b = -2. * m_lengthBF * sin(m_stereo/2. + phiPrime);
    double c = m_lengthBF * m_lengthBF - r * r;
    double rPrime = (-b + sqrt(b * b - 4. * c))/2.;

    double xPrime = rPrime * cos(phiPrime);
    double yPrime = rPrime * sin(phiPrime);
    double x = cos(m_stereo) * (xPrime - m_R) - sin(m_stereo) * yPrime + m_R;
    double y = sin(m_stereo) * (xPrime - m_R) + cos(m_stereo) * yPrime;

    return SiLocalPosition(x, y, 0.0);
}

SiLocalPosition StripStereoAnnulusDesign::localPositionOfCluster(SiCellId const &cellId, int clusterSize) const {
//
//    Return the average centre-position of the first and last strips.
//

    SiLocalPosition startPos = localPositionOfCell(cellId);

    if (clusterSize <= 1) {
        return startPos;
    }

    int strip, row;
    getStripRow(cellId, &strip, &row);
    int stripEnd = strip + clusterSize - 1;
    SiCellId endId = strip1Dim(stripEnd, row);
    SiLocalPosition endPos = localPositionOfCell(endId);

    return (startPos + endPos) / 2.0;
}

/// Give end points of the strip that covers the given position
std::pair<SiLocalPosition, SiLocalPosition> StripStereoAnnulusDesign::endsOfStrip(SiLocalPosition const &pos) const {

    int strip, row;
    closestStripRowOfPosition(pos, strip, row ) ;

    SiLocalPosition innerEnd = stripPosAtR(strip, row, m_stripStartRadius[row]);
    SiLocalPosition outerEnd = stripPosAtR(strip, row, m_stripEndRadius[row]);

    return pair<SiLocalPosition, SiLocalPosition>(innerEnd, outerEnd);
}

bool StripStereoAnnulusDesign::inActiveArea(SiLocalPosition const &pos, bool /*checkBondGap*/) const {

    SiCellId id = cellIdOfPosition(pos);
    bool inside = id.isValid();

    return inside;
}

// Used in surfaceChargesGenerator
double StripStereoAnnulusDesign::scaledDistanceToNearestDiode(SiLocalPosition const &pos) const {

//    Get phiPrime of pos
//
    double posxP = cos(-m_stereo) * (pos.xEta() - m_R) - sin(-m_stereo) * pos.xPhi() + m_R;
    double posyP = sin(-m_stereo) * (pos.xEta() - m_R) + cos(-m_stereo) * pos.xPhi();
    double posphiP = atan2(posyP, posxP);
//
//    Get phiPrime of strip
//
    SiCellId cellId = cellIdOfPosition(pos);
    SiLocalPosition posStrip = localPositionOfCell(cellId);
    double posStripxP = cos(-m_stereo) * (posStrip.xEta() - m_R) - sin(-m_stereo) * posStrip.xPhi() + m_R;
    double posStripyP = sin(-m_stereo) * (posStrip.xEta() - m_R) + cos(-m_stereo) * posStrip.xPhi();
    double posStripphiP = atan2(posStripyP, posStripxP);
    int strip, row;
    getStripRow(cellId, &strip, &row);
//cout << "Frac. dist = " << fabs(posphiP - posStripphiP) / m_pitch[row] << endl;
    return fabs(posphiP - posStripphiP) / m_pitch[row];
}

/// Return strip width, centre, length etc. Hard to find if this is used or not.
SiDiodesParameters StripStereoAnnulusDesign::parameters(SiCellId const & /*cellId*/) const {
    throw std::runtime_error("Call to StripStereoAnnulusDesign::parameters; not yet implemented");
}

// Used in VP1 graphics. DEPRECATED.
SiLocalPosition StripStereoAnnulusDesign::positionFromStrip(SiCellId const &cellId) const {
//    throw std::runtime_error("Deprecated positionFromStrip called.");
    return localPositionOfCell(cellId);
}

// DEPRECATED but pure virtual in base class; which row?? - assume row 0.
SiLocalPosition StripStereoAnnulusDesign::positionFromStrip(const int stripNumber) const {
    return localPositionOfCell(SiCellId(stripNumber, 0));
}

/// DEPRECATED: only used in a stupid example (2014)
///Check if cell is in range. Returns the original cellId if it is in range, otherwise it
// returns an invalid id.
SiCellId StripStereoAnnulusDesign::cellIdInRange(const SiCellId &cellId) const {

    if (!cellId.isValid()) {
        return SiCellId(); // Invalid
    }
    int row = cellId.etaIndex();
    int strip = cellId.phiIndex();
    if (strip < 0 || row < 0 || row >= m_nRows || strip >= m_nStrips[row]) {
        return SiCellId(); // Invalid
    }
    return cellId;
}

double StripStereoAnnulusDesign::length() const {
// Return the total length of all strips, i.e. the active area length.
    return m_stripEndRadius.back() - m_stripStartRadius[0];
}

double StripStereoAnnulusDesign::width() const {
// Return approximate width between the two central rows
    int middleRow = m_stripStartRadius.size() / 2 - 1;
    if (middleRow < 0) {
        throw std::runtime_error(
         "StripStereoAnnulusDesign::width: the sensor had one or less rows of strips. Code assumes two or more.");
    }

    return 2. * tan((m_pitch[middleRow] * m_nStrips[middleRow]) / 2.) * m_stripEndRadius[middleRow];
}

double StripStereoAnnulusDesign::minWidth() const {
    return 2. * tan((m_pitch[0] * m_nStrips[0]) / 2.) * m_stripStartRadius[0];
}

double StripStereoAnnulusDesign::maxWidth() const {
    return 2. * tan((m_pitch.back() * m_nStrips.back()) / 2.) * m_stripEndRadius.back();
}

double StripStereoAnnulusDesign::etaPitch() const {
// Return average strip length
    return length() / m_stripStartRadius.size();
}

HepGeom::Vector3D<double> StripStereoAnnulusDesign::phiMeasureSegment(const SiLocalPosition & /*position*/)
const {
    throw std::runtime_error("Call to phiMeasureSegment, DEPRECATED, not implemented.");
}

void StripStereoAnnulusDesign::distanceToDetectorEdge(SiLocalPosition const & pos,
                                            double & etaDist,
                                            double & phiDist) const {
                                              
                                              
  etaDist = length()/2.0 - abs(pos.xEta());
  phiDist = (minWidth() + maxWidth())/4.0 - abs(pos.xPhi());

}

DetectorShape StripStereoAnnulusDesign::shape() const
 {
   return InDetDD::Annulus;
 }

  double StripStereoAnnulusDesign::stripLength(const InDetDD::SiCellId &cellId) const 
  {
  SiLocalPosition lpoc = localPositionOfCell(cellId);
  std::pair<SiLocalPosition, SiLocalPosition> end = endsOfStrip(lpoc);
  double dx = end.second.xEta() - end.first.xEta();
  double dy = end.second.xPhi() - end.first.xPhi();
  return sqrt(dx * dx + dy * dy);
  }


} // namespace InDetDD

