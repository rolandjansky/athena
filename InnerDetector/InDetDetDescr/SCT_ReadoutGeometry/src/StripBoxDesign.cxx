/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_ReadoutGeometry/StripBoxDesign.h"

#include "GeoModelKernel/Units.h"
#include "Identifier/Identifier.h"

#include <stdexcept>
#include <cmath>

namespace InDetDD {
StripBoxDesign::StripBoxDesign(const SiDetectorDesign::Axis stripDirection,
                               const SiDetectorDesign::Axis thicknessDirection,
                               const double thickness,
                               const int readoutSide,
                               const InDetDD::CarrierType carrier,
                               const int nRows,
                               const int nStrips,
                               const double pitch,
                               const double length) : 
    SCT_ModuleSideDesign(thickness, true, true, true, 1, nRows * nStrips, nRows * nStrips, 0, false, carrier,
                         readoutSide, stripDirection, thicknessDirection) {
    if (nRows <= 0) {
        throw std::runtime_error(
                  "ERROR: StripBoxDesign called with non-positive number of rows");
    }

    m_nRows = nRows;
    m_nStrips = nStrips;
    m_pitch = pitch;
    m_length = length;

    double width = m_nStrips * m_pitch;
    double fullLength = m_nRows * m_length;
    m_bounds = Trk::RectangleBounds(width / 2.0, fullLength / 2.0);
}

void StripBoxDesign::getStripRow(SiCellId cellId, int *strip, int *row) const {
    int strip1D = cellId.phiIndex();
    *row = strip1D / m_nStrips;
    *strip = strip1D % m_nStrips;
    return;
}

int StripBoxDesign::strip1Dim(int strip, int row) const {
    return m_nStrips * row + strip;
}

void StripBoxDesign::neighboursOfCell(const SiCellId &cellId,
                                      std::vector<SiCellId> &neighbours) const {


    neighbours.clear();

    if (!cellId.isValid()) {
        return;
    }

    int strip, row;
    getStripRow(cellId, &strip, &row);
    int stripM = strip - 1;
    int stripP = strip + 1;

    if (stripM >= 0) {
        neighbours.push_back(stripM);
    }
    if (stripP < m_nStrips) {
        neighbours.push_back(stripP);
    }

    return;
}

const Trk::SurfaceBounds &StripBoxDesign::bounds() const {
//    Return smallest rectangle that fully encompasses the active area.

    return m_bounds;
}

SiCellId StripBoxDesign::cellIdOfPosition(SiLocalPosition const &pos) const {
//
//    Find the row
//
    int strip = static_cast<int>(std::floor(pos.xPhi() / m_pitch) + m_nStrips / 2);
    if (strip < 0 || strip >= m_nStrips) {

        return SiCellId(); // return an invalid id
    }

    int row = static_cast<int>(std::floor(pos.xEta() / m_length) + m_nRows / 2);
    if (row < 0 || row >= m_nRows) {

        return SiCellId(); // return an invalid id
    }
    int strip1D = strip1Dim(strip, row);

    return SiCellId(strip1D, 0);
}

SiLocalPosition StripBoxDesign::localPositionOfCell(SiCellId const &cellId) const {

    int row, strip;
    getStripRow(cellId, &strip, &row);
    double eta = ((double) row - (double) m_nRows / 2. + 0.5) * m_length;

    double phi = ((double) strip - (double) m_nStrips / 2. + 0.5) * m_pitch;


    return SiLocalPosition(eta, phi, 0.0);
}

SiLocalPosition StripBoxDesign::localPositionOfCluster(SiCellId const &cellId,
                                                       int clusterSize) const {

    SiLocalPosition pos = localPositionOfCell(cellId);

    if (clusterSize <= 1) {
        return pos;
    }

    double clusterWidth = clusterSize * m_pitch;
    pos.xPhi(pos.xPhi() + clusterWidth / 2.); // get then set xPhi

    return pos;
}

/// Give end points of the strip that covers the given position
std::pair<SiLocalPosition, SiLocalPosition> StripBoxDesign::endsOfStrip(
    SiLocalPosition const &pos) const {

    SiCellId cellId = cellIdOfPosition(pos);

    int strip, row;
    getStripRow(cellId, &strip, &row);

    double etaStart = (row - m_nRows / 2.) * m_length;
    double etaEnd = etaStart + m_length;

    double phi = (strip - m_nStrips / 2. + 0.5) * m_pitch;
// cout << "etaStart = " << etaStart << "; etaEnd = " << etaEnd << "; phi coord. = " << phi << endl; 

    SiLocalPosition end1(etaStart, phi, 0.0);
    SiLocalPosition end2(etaEnd, phi, 0.0);

    return std::pair<SiLocalPosition, SiLocalPosition>(end1, end2);
}

bool StripBoxDesign::inActiveArea(SiLocalPosition const &pos,
                                  bool /*checkBondGap*/) const {

    SiCellId id = cellIdOfPosition(pos);


    return id.isValid();
}

// Used in surfaceChargesGenerator
double StripBoxDesign::scaledDistanceToNearestDiode(SiLocalPosition const &pos) const {

    SiCellId cellId = cellIdOfPosition(pos);
    SiLocalPosition posStrip = localPositionOfCell(cellId);


    return std::abs(pos.xPhi() - posStrip.xPhi()) / m_pitch;
}

/// Return strip width, centre, length etc. Hard to find if this is used or not.
SiDiodesParameters StripBoxDesign::parameters(SiCellId const & /*cellId*/) const {
    throw std::runtime_error("Call to StripBoxDesign::parameters; not yet implemented");
}

// Used in VP1 graphics. DEPRECATED.
SiLocalPosition StripBoxDesign::positionFromStrip(SiCellId const &cellId) const {
//    throw std::runtime_error("Deprecated positionFromStrip called.");
    return localPositionOfCell(cellId);
}

// DEPRECATED but pure virtual in base class; which row?? - assume row 0.
SiLocalPosition StripBoxDesign::positionFromStrip(const int stripNumber) const {
    return localPositionOfCell(SiCellId(stripNumber, 0));
}

/// DEPRECATED: only used in a stupid example (2014)
///Check if cell is in range. Returns the original cellId if it is in range, otherwise it
// returns an invalid id.
SiCellId StripBoxDesign::cellIdInRange(const SiCellId &cellId) const {

    if (!cellId.isValid()) {
        return SiCellId(); // Invalid
    }
    int strip, row;
    getStripRow(cellId, &strip, &row);
    if (strip < 0 || row < 0 || row >= m_nRows || strip >= m_nStrips) {
        return SiCellId(); // Invalid
    }
    return cellId;
}

double StripBoxDesign::length() const {
    return m_length*m_nRows;
}

double StripBoxDesign::width() const {
    return m_pitch * m_nStrips;
}

double StripBoxDesign::minWidth() const {
    return width();
}

double StripBoxDesign::maxWidth() const {
    return width();
}

double StripBoxDesign::etaPitch() const {
    return m_length;
}

HepGeom::Vector3D<double> StripBoxDesign::phiMeasureSegment(const SiLocalPosition & /*position*/)
const {
    throw std::runtime_error("Call to phiMeasureSegment, DEPRECATED, not implemented.");
}

/// DEPRECATED: Unused (2014)
void StripBoxDesign::distanceToDetectorEdge(SiLocalPosition const & pos,
                                            double & etaDist,
                                            double & phiDist) const {
    
 // As the calculation is symmetric around 0,0 we only have to test it for one side.
  double xEta = std::abs(pos.xEta()); //assuming centered around 0?!?
   double xPhi = std::abs(pos.xPhi());
 
   double xEtaEdge = 0.5 * length();
   double xPhiEdge = 0.5 * width();
 
   // Distance to top/bottom
   etaDist = xEtaEdge - xEta;
   
   // Distance to right/left edge
   phiDist = xPhiEdge - xPhi;


}

  const GeoTrf::Transform3D StripBoxDesign::SiHitToGeoModel() const {
    return GeoTrf::RotateY3D(90.*GeoModelKernelUnits::deg);
}

} // namespace InDetDD
