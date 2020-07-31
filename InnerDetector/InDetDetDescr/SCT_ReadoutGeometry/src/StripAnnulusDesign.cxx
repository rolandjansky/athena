/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <stdexcept>
#include <algorithm> // For upper_bound
#include <iterator>  // for std::distance()
#include <cmath>
#include "SCT_ReadoutGeometry/StripAnnulusDesign.h"
#include "Identifier/Identifier.h"
#include "TrkSurfaces/RectangleBounds.h"

namespace InDetDD {
StripAnnulusDesign::StripAnnulusDesign(const SiDetectorDesign::Axis &stripDirection,
                               const SiDetectorDesign::Axis &thicknessDirection,
                               const double &thickness,
                               const int &readoutSide,
                               const InDetDD::CarrierType &carrier,
                               const int &nStrips,
                               const double &pitch,
                               const double &stripStartRadius,
                               const double &stripEndRadius) :
    SCT_ModuleSideDesign(thickness, true, true, true, 1, 0, 0, 0, false, carrier, readoutSide, stripDirection, thicknessDirection) {

    m_nStrips = nStrips;
    m_pitch = pitch;
    m_stripStartRadius = stripStartRadius;
    m_stripEndRadius = stripEndRadius;

    m_scheme.setCells(m_nStrips);
    m_scheme.setDiodes(m_nStrips);

    double width = m_nStrips * m_pitch;
    double length = m_stripEndRadius - m_stripStartRadius;
    std::unique_ptr<Trk::SurfaceBounds> m_bounds = std::make_unique<Trk::RectangleBounds>(width / 2.0, length / 2.0); // Awaiting new boundclass for Annulus shape
}

HepGeom::Point3D<double> StripAnnulusDesign::sensorCenter() const {
    double centerR = (m_stripStartRadius + m_stripEndRadius) * 0.5;
    return HepGeom::Point3D<double>(centerR, 0., 0.);
}

void StripAnnulusDesign::neighboursOfCell(const SiCellId &cellId, std::vector<SiCellId> &neighbours) const {

    neighbours.clear();

    if (!cellId.isValid()) {
        return;
    }

    int strip = cellId.strip();
    int stripM = strip - 1;
    int stripP = strip + 1;

    if (stripM > 0) {
        neighbours.push_back(stripM);
    }
    if (stripP < m_nStrips) {
        neighbours.push_back(stripP);
    }

    return;
}

const Trk::SurfaceBounds &StripAnnulusDesign::bounds() const {

    return *m_bounds;
}

SiCellId StripAnnulusDesign::cellIdOfPosition(SiLocalPosition const &pos) const {
    double phi = pos.phi();
//
//    Find the strip
//
    int strip = std::floor(phi / m_pitch) + m_nStrips * 0.5;
    return SiCellId(strip);
}

SiLocalPosition StripAnnulusDesign::localPositionOfCell(SiCellId const &cellId) const {
  
  int strip = cellId.strip();
  double r = (m_stripEndRadius - m_stripStartRadius) *0.5;
  double phi = (strip - m_nStrips*0.5 + 0.5) * m_pitch;
  double xEta = r * std::cos(phi);
  double xPhi = r * std::sin(phi);
  return SiLocalPosition(xEta, xPhi, 0.0);
}

SiLocalPosition StripAnnulusDesign::localPositionOfCluster(SiCellId const &cellId, int clusterSize) const {

    SiLocalPosition pos = localPositionOfCell(cellId);

    if (clusterSize <= 1) {
        return pos;
    }

    double clusterWidth = clusterSize * m_pitch;
    double phi = pos.phi() + clusterWidth *0.5;
    double r = pos.r();

    pos.xPhi(r * std::sin(phi));
    pos.xEta(r * std::cos(phi));

    return pos;
}

/// Give end points of the strip that covers the given position
std::pair<SiLocalPosition, SiLocalPosition> StripAnnulusDesign::endsOfStrip(SiLocalPosition const &pos) const {

    SiCellId cellId = cellIdOfPosition(pos);

    int strip = cellId.strip();

    double rStart = m_stripStartRadius;
    double rEnd = m_stripEndRadius;
    double phi = (strip - m_nStrips*0.5 + 0.5) * m_pitch;

    SiLocalPosition end1(rStart * std::cos(phi), rStart * std::sin(phi), 0.0);
    SiLocalPosition end2(rEnd * std::cos(phi), rEnd * std::sin(phi), 0.0);

    return std::pair<SiLocalPosition, SiLocalPosition>(end1, end2);
}

bool StripAnnulusDesign::inActiveArea(SiLocalPosition const &pos, bool /*checkBondGap*/) const {


    SiCellId id = cellIdOfPosition(pos);

    return id.isValid();
}

// Used in surfaceChargesGenerator
double StripAnnulusDesign::scaledDistanceToNearestDiode(SiLocalPosition const &pos) const {

    SiCellId cellId = cellIdOfPosition(pos);
    SiLocalPosition posStrip = localPositionOfCell(cellId);

    return std::abs(pos.xPhi() - posStrip.xPhi()) / m_pitch;
}

/// Return strip width, centre, length etc. Hard to find if this is used or not.
SiDiodesParameters StripAnnulusDesign::parameters(SiCellId const & /*cellId*/) const {
    throw std::runtime_error("Call to StripAnnulusDesign::parameters; not yet implemented");
}

// Used in VP1 graphics. DEPRECATED.
SiLocalPosition StripAnnulusDesign::positionFromStrip(SiCellId const &cellId) const {
//    throw std::runtime_error("Deprecated positionFromStrip called.");
    return localPositionOfCell(cellId);
}

SiLocalPosition StripAnnulusDesign::positionFromStrip(const int strip) const {
    return localPositionOfCell(SiCellId(strip));
}
//
//    DEPRECATED: only used in a stupid example (2014). Make cellId's by correct methods and 
//    they are either invalid or in range.
//    Check if cell is in range. Returns the original cellId if it is in range, otherwise it
//    returns an invalid id.
//
SiCellId StripAnnulusDesign::cellIdInRange(const SiCellId &cellId) const {

    if (!cellId.isValid()) {
        return SiCellId(); // Invalid
    }
    int strip = cellId.strip();
    if (strip < 0 ||  strip >= m_nStrips) {
        return SiCellId(); // Invalid
    }
    return cellId;
}

double StripAnnulusDesign::length() const { // DEPRECATED: 
    return m_stripEndRadius - m_stripStartRadius;
}

double StripAnnulusDesign::width() const { // DEPRECATED
    return m_pitch * m_nStrips;
}

double StripAnnulusDesign::minWidth() const { // DEPRECATED
    return width();
}

double StripAnnulusDesign::maxWidth() const { // DEPRECATED
    return width();
}

double StripAnnulusDesign::etaPitch() const { // DEPRECATED
    return length();
}

const HepGeom::Transform3D StripAnnulusDesign::SiHitToGeoModel() const {
   return HepGeom::RotateY3D(90.*CLHEP::deg) ;
}

} // namespace InDetDD
