/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_ReadoutGeometry/StripStereoAnnulusDesign.h"
#include "Identifier/Identifier.h"

#include <stdexcept>
#include <algorithm> // For upper_bound
#include <iterator>  // for std::distance()
#include <cmath>

namespace InDetDD {
StripStereoAnnulusDesign::StripStereoAnnulusDesign(const SiDetectorDesign::Axis &stripDirection,
                               const SiDetectorDesign::Axis &thicknessDirection,
                               const double &thickness,
                               const int &readoutSide,
                               const InDetDD::CarrierType &carrier,
                               const int &nRows,
                               const std::vector<int> &nStrips,
                               const std::vector<double> &pitch,
                               const std::vector<double> &stripStartRadius,
                               const std::vector<double> &stripEndRadius,
                               const double &stereoAngle,
                               const double &centreR) :
    SCT_ModuleSideDesign(thickness, false, false, true, 1, 0, 0, 0, false, carrier,
                         readoutSide, stripDirection, thicknessDirection),
  m_nRows(nRows),
  m_nStrips(nStrips),
  m_pitch(pitch),
  m_stripStartRadius(stripStartRadius),
  m_stripEndRadius(stripEndRadius),
  m_stereo(stereoAngle),
  m_R(centreR),
  m_lengthBF(2. * centreR * std::sin(stereoAngle*0.5)),// Eq. 5 p. 7
  m_sinStereo(std::sin(-m_stereo)),
  m_cosStereo(std::cos(-m_stereo))
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
    m_bounds = Trk::AnnulusBounds(m_stripStartRadius[0], m_stripEndRadius.back(), m_R, m_nStrips[0] * m_pitch[0], m_stereo);

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
    
    double xSF = m_cosStereo * (x - m_R) - m_sinStereo * y + m_R;
    double ySF = m_sinStereo * (x - m_R) + m_cosStereo * y;
    double phiPrime = std::atan2(ySF, xSF);

    // The minus sign below is because this routine is called by tracking software, which swaps x and y, then measures angles from y 
    // to x
    return -std::sin(phiPrime + m_stereo);
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

    return m_bounds;
}

SiCellId StripStereoAnnulusDesign::cellIdOfPosition(SiLocalPosition const &pos) const {
//
//    Find the row
//
    double r = pos.r();
    if (r < m_stripStartRadius[0] || r >= m_stripEndRadius.back()) { 
        return SiCellId(); // return an invalid id
    }

    std::vector<double>::const_iterator endPtr = upper_bound(m_stripStartRadius.begin(), m_stripStartRadius.end(), r);
    int row = distance(m_stripStartRadius.begin(), endPtr) - 1;
    // Following should never happen, check is done on r above
    if (row < 0 || row >= m_nRows) {
        std::cout << "ERROR! cellIdOfPosition: bad row = " << row << " for r = " << r << std::endl;
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
    double xSF = m_cosStereo * (x - m_R) - m_sinStereo * y + m_R;
    double ySF = m_sinStereo * (x - m_R) + m_cosStereo * y;
    double phiPrime = std::atan2(ySF, xSF); 
    int strip = std::floor(phiPrime / m_pitch[row]) + m_nStrips[row] *0.5;
    if (strip < 0) { // Outside
        return SiCellId(); // return an invalid id
    }
    if (strip >= m_nStrips[row]) { // Outside
        return SiCellId(); // return an invalid id
    }

    int strip1D = strip1Dim(strip, row);
    return SiCellId(strip1D, 0);
}

SiLocalPosition StripStereoAnnulusDesign::localPositionOfCell(SiCellId const &cellId) const {
    int strip, row;
    getStripRow(cellId, &strip, &row);
    double r = (m_stripEndRadius[row] + m_stripStartRadius[row])*0.5;
    return stripPosAtR(strip, row, r);
}

SiLocalPosition StripStereoAnnulusDesign::stripPosAtR(int strip, int row, double r) const {

    double phiPrime = (strip - m_nStrips[row]*0.5 + 0.5) * m_pitch[row];

    double b = -2. * m_lengthBF * std::sin(m_stereo*0.5 + phiPrime);
    double c = m_lengthBF * m_lengthBF - r * r;
    double rPrime = (-b + std::sqrt(b * b - 4. * c))*0.5;

    double xPrime = rPrime * std::cos(phiPrime);
    double yPrime = rPrime * std::sin(phiPrime);
   
    double x = m_cosStereo * (xPrime - m_R) - m_sinStereo * yPrime + m_R;
    double y = m_sinStereo * (xPrime - m_R) + m_cosStereo * yPrime;

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

    return (startPos + endPos)*0.5;
}

SiLocalPosition StripStereoAnnulusDesign::localPositionOfCellPC(SiCellId const &cellId) const {
  
    int strip, row;
    getStripRow(cellId, &strip, &row);
    // this is the radius in the module / radial system
    double r = (m_stripEndRadius[row] + m_stripStartRadius[row])*0.5;

    // get phi of strip in the strip system
    double phiPrime = (strip - m_nStrips[row]*0.5 + 0.5) * m_pitch[row];

    double b = -2. * m_lengthBF * std::sin(m_stereo*0.5 + phiPrime);
    double c = m_lengthBF * m_lengthBF - r * r;
    // this is the radius in the strip system
    double rPrime = (-b + std::sqrt(b * b - 4. * c))*0.5;

    // flip this, since coordinate system is defined the other way round
    double phi = -1*phiPrime;

    // xEta => r, xPhi = phi
    return SiLocalPosition(rPrime, phi); 
}

SiLocalPosition StripStereoAnnulusDesign::localPositionOfClusterPC(SiCellId const &cellId, int clusterSize) const {
    SiLocalPosition startPos = localPositionOfCellPC(cellId);

    if (clusterSize <= 1) {
        return startPos;
    }

    int strip, row;
    getStripRow(cellId, &strip, &row);
    int stripEnd = strip + clusterSize - 1;
    SiCellId endId = strip1Dim(stripEnd, row);
    SiLocalPosition endPos = localPositionOfCellPC(endId);

    return (startPos + endPos)*0.5;
}

/// Give end points of the strip that covers the given position
std::pair<SiLocalPosition, SiLocalPosition> StripStereoAnnulusDesign::endsOfStrip(SiLocalPosition const &pos) const {

    SiCellId cellId = cellIdOfPosition(pos);

    int strip, row;
    getStripRow(cellId, &strip, &row);

    SiLocalPosition innerEnd = stripPosAtR(strip, row, m_stripStartRadius[row]);

    SiLocalPosition outerEnd = stripPosAtR(strip, row, m_stripEndRadius[row]);

    return std::pair<SiLocalPosition, SiLocalPosition>(innerEnd, outerEnd);
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
  double posxP = m_cosStereo * (pos.xEta() - m_R) - m_sinStereo * pos.xPhi() + m_R;
  double posyP = m_sinStereo * (pos.xEta() - m_R) + m_cosStereo * pos.xPhi();
  double posphiP = std::atan2(posyP, posxP);
  //
  //    Get phiPrime of strip
  //
  SiCellId cellId = cellIdOfPosition(pos);
  SiLocalPosition posStrip = localPositionOfCell(cellId);
  double posStripxP = m_cosStereo * (posStrip.xEta() - m_R) - m_sinStereo * posStrip.xPhi() + m_R;
  double posStripyP = m_sinStereo * (posStrip.xEta() - m_R) + m_cosStereo * posStrip.xPhi();
  double posStripphiP = std::atan2(posStripyP, posStripxP);
  int strip, row;
  getStripRow(cellId, &strip, &row);
  return std::abs(posphiP - posStripphiP) / m_pitch[row];
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
    int middleRow = m_stripStartRadius.size()*0.5 - 1;
    if (middleRow < 0) {
        throw std::runtime_error(
         "StripStereoAnnulusDesign::width: the sensor had one or less rows of strips. Code assumes two or more.");
    }

    return 2. * tan((m_pitch[middleRow] * m_nStrips[middleRow])*0.5) * m_stripEndRadius[middleRow];
}

double StripStereoAnnulusDesign::minWidth() const {
    return 2. * tan((m_pitch[0] * m_nStrips[0]) *0.5) * m_stripStartRadius[0];
}

double StripStereoAnnulusDesign::maxWidth() const {
    return 2. * tan((m_pitch.back() * m_nStrips.back()) *0.5) * m_stripEndRadius.back();
}

double StripStereoAnnulusDesign::etaPitch() const {
// Return average strip length
    return length() / m_stripStartRadius.size();
}

HepGeom::Vector3D<double> StripStereoAnnulusDesign::phiMeasureSegment(const SiLocalPosition & /*position*/)
const {
    throw std::runtime_error("Call to phiMeasureSegment, DEPRECATED, not implemented.");
}

void StripStereoAnnulusDesign::distanceToDetectorEdge(SiLocalPosition const & pos, double & etaDist, double & phiDist) const {
// For eta, we use the Strip frame. This is centred at the beamline, x along eta, y along phi, z along depth
// Happens to coincide with SiLocalPosition; no transform needed.
  double rInner = m_stripStartRadius[0];
  double rOuter = m_stripEndRadius[m_nRows - 1];
  double xEta = pos.xEta();
  double xPhi = pos.xPhi();
  double r = std::sqrt(xEta * xEta + xPhi * xPhi);
  if (r < rInner)
    etaDist = r - rInner;
  else if (r > rOuter)
    etaDist = rOuter - r;
  else
    etaDist = std::min(rOuter - r, r - rInner);
 
// For phi, we use the Strip frame. Transform to Strip-frame:
  double etaStrip = m_cosStereo * (xEta - m_R) - m_sinStereo * xPhi + m_R;
  double phiStrip = m_sinStereo * (xEta - m_R) + m_cosStereo * xPhi;
// Put these into polar coordinates
  double rStrip = std::sqrt(etaStrip * etaStrip + phiStrip * phiStrip);
  double phiAngleStrip = std::atan2(phiStrip, etaStrip);

  double phiAngleMax = m_pitch[0] * m_nStrips[0]*0.5;
  double phiAngleMin = -phiAngleMax;

  if (phiAngleStrip < phiAngleMin)
    phiDist = rStrip * (phiAngleStrip - phiAngleMin);
  else if (phiAngleStrip > phiAngleMax)
    phiDist = rStrip * (phiAngleMax - phiAngleStrip);
  else
    phiDist = rStrip * std::min(phiAngleMax - phiAngleStrip, phiAngleStrip - phiAngleMin);

  return;
}

DetectorShape StripStereoAnnulusDesign::shape() const
 {
   return InDetDD::Annulus;
 }

const HepGeom::Transform3D StripStereoAnnulusDesign::SiHitToGeoModel() const {
   return HepGeom::RotateY3D(90.*CLHEP::deg) ;
}

double StripStereoAnnulusDesign::stripLength(const InDetDD::SiCellId &cellId) const
{
  SiLocalPosition lpoc = localPositionOfCell(cellId);
  std::pair<SiLocalPosition, SiLocalPosition> end = endsOfStrip(lpoc);
  double dx = end.second.xEta() - end.first.xEta();
  double dy = end.second.xPhi() - end.first.xPhi();
  return std::sqrt(dx * dx + dy * dy);
}


} // namespace InDetDD

