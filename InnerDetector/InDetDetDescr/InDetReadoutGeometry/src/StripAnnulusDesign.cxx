/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdexcept>
#include <algorithm> // For upper_bound
#include <iterator>  // for std::distance()
#include <cmath>
#include "InDetReadoutGeometry/StripAnnulusDesign.h"
#include "Identifier/Identifier.h"
#include "TrkSurfaces/RectangleBounds.h"

using namespace std;

namespace InDetDD {
StripAnnulusDesign::StripAnnulusDesign(const SiDetectorDesign::Axis stripDirection,
                               const SiDetectorDesign::Axis thicknessDirection,
                               const double thickness,
                               const int readoutSide,
                               const InDetDD::CarrierType carrier,
                               const int nRows,
                               const vector<int> nStrips,
                               const vector<double> pitch,
                               const vector<double> stripStartRadius,
                               const vector<double> stripEndRadius) :
    SCT_ModuleSideDesign(thickness, true, true, true, 1, 0, 0, 0, false, carrier,
                         readoutSide, stripDirection, thicknessDirection),m_nRows(nRows) {
    if (nRows < 0) {
        throw std::runtime_error(
                  "ERROR: StripAnnulusDesign called with negative number of rows");
    }

    if (pitch.size() != (unsigned) nRows) {
        throw std::runtime_error(
                  "ERROR: StripAnnulusDesign called with insufficiant pitch values for no. of rows");
    }

    if (stripStartRadius.size() != (unsigned) nRows) {
        throw std::runtime_error(
                  "ERROR: StripAnnulusDesign called with insufficiant strip start-positions for no. of rows");
    }

    if (stripEndRadius.size() != (unsigned) nRows) {
        throw std::runtime_error(
                  "ERROR: StripAnnulusDesign called with insufficiant strip end-positions for no. of rows");
    }

    m_nStrips = nStrips;
    m_pitch = pitch;
    m_stripStartRadius = stripStartRadius;
    m_stripEndRadius = stripEndRadius;

    int startStrip = 0;
    for (int r = 0; r < nRows; ++r) {
        m_firstStrip.push_back(startStrip);
        startStrip += m_nStrips[r];
    }
    int totalStrips = startStrip;
    m_firstStrip.push_back(totalStrips); // Add one more as total number of strips; remove if not useful.

    m_scheme.setCells(totalStrips);
    m_scheme.setDiodes(totalStrips);

    double width = m_nStrips[0] * m_pitch[0]; // Assume all rows have same width
    double length = m_stripEndRadius.back() - m_stripStartRadius[0];
    m_bounds = new Trk::RectangleBounds(width / 2.0, length / 2.0); // Awaiting new boundclass for Annulus shape
}

StripAnnulusDesign::~StripAnnulusDesign() {
    delete m_bounds;
}

HepGeom::Point3D<double> StripAnnulusDesign::sensorCenter() const {
    double centerR = (m_stripStartRadius[0] + m_stripEndRadius.back()) / 2.;
    return HepGeom::Point3D<double>(centerR, 0., 0.);
}

void StripAnnulusDesign::getStripRow(SiCellId cellId, int *strip2D, int *rowNum) const {
    int strip1D = cellId.phiIndex();
    *rowNum = row(strip1D);
    *strip2D = strip1D - m_firstStrip[*rowNum];
cout << "getStripRow: cellId = " << hex << cellId << dec << " gives row = " << *rowNum << " and strip = " << *strip2D << endl;
    return;
}

int  StripAnnulusDesign::strip1Dim(int strip, int row) const {
    return m_firstStrip[row] + strip;
}

void StripAnnulusDesign::neighboursOfCell(const SiCellId &cellId, std::vector<SiCellId> &neighbours) const {

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




const Trk::SurfaceBounds &StripAnnulusDesign::bounds() const {

    return *m_bounds;
}

SiCellId StripAnnulusDesign::cellIdOfPosition(SiLocalPosition const &pos) const {

    double r = pos.r();
    double phi = pos.phi();
//
//    Find the row
//
    vector<double>::const_iterator endPtr = upper_bound(m_stripEndRadius.begin(), m_stripEndRadius.end(), r);
    int row = distance(m_stripEndRadius.begin(), endPtr);
    if (row < 0 || row >= m_nRows) {
        return SiCellId(); // return an invalid id
    }
//
//    Find the strip
//
    int strip = floor(phi / m_pitch[row]) + m_nStrips[row] / 2.0;
    if (strip < m_firstStrip[row]) {
        return SiCellId(); // return an invalid id
    }
    if (strip >= m_firstStrip[row + 1]) {
        return SiCellId(); // return an invalid id
    }
    int strip1D = strip1Dim(strip, row);
    return SiCellId(strip1D, 0);
}



void StripAnnulusDesign::closestStripRowOfPosition(SiLocalPosition const &pos, int & strip, int & row ) const {

    double r = pos.r()                                              		;
    double phi = pos.phi()                                          		;             
    ////    Find the row
    ////
    vector<double>::const_iterator endPtr = upper_bound( m_stripEndRadius.begin(),  m_stripEndRadius.end(), r);
    row = distance( m_stripEndRadius.begin(), endPtr)             		;
    if ( row < 0 ) row = 0                                          		;
    if ( row >= m_nRows) row = (m_nRows-1)                              	;
  	
    //    Find the strip
    strip = floor(phi / m_pitch[row]) + m_nStrips[row]/2.0            		;
    if (strip <  m_firstStrip[row]) 	strip = m_firstStrip[row] 		;       
    if (strip >= m_firstStrip[row + 1]) strip = (m_firstStrip[row +1] -1)	;
  	
}
    	






SiLocalPosition StripAnnulusDesign::localPositionOfCell(SiCellId const &cellId) const {

    int strip, row;
    getStripRow(cellId, &strip, &row);
    double r = (m_stripEndRadius[row] - m_stripStartRadius[row]) / 2.;
    double phi = (strip - m_nStrips[row] / 2. + 0.5) * m_pitch[row];
    double xEta = r * cos(phi);
    double xPhi = r * sin(phi);
    return SiLocalPosition(xEta, xPhi, 0.0);
}

SiLocalPosition StripAnnulusDesign::localPositionOfCluster(SiCellId const &cellId, int clusterSize) const {

    SiLocalPosition pos = localPositionOfCell(cellId);

    if (clusterSize <= 1) {
        return pos;
    }

    int strip, row;
    getStripRow(cellId, &strip, &row);
    double clusterWidth = clusterSize * m_pitch[row];
    double phi = pos.phi() + clusterWidth / 2.;
    double r = pos.r();

    pos.xPhi(r * sin(phi));
    pos.xEta(r * cos(phi));

    return pos;
}

/// Give end points of the strip that covers the given position
std::pair<SiLocalPosition, SiLocalPosition> StripAnnulusDesign::endsOfStrip(SiLocalPosition const &pos) const {


    int strip, row;

    closestStripRowOfPosition(pos, strip, row);

    double rStart = m_stripStartRadius[row];
    double rEnd = m_stripEndRadius[row];
    double phi = (strip - m_nStrips[row] / 2. + 0.5) * m_pitch[row];

    SiLocalPosition end1(rStart * cos(phi), rStart * sin(phi), 0.0);
    SiLocalPosition end2(rEnd * cos(phi), rEnd * sin(phi), 0.0);

    return pair<SiLocalPosition, SiLocalPosition>(end1, end2);
}

bool StripAnnulusDesign::inActiveArea(SiLocalPosition const &pos, bool /*checkBondGap*/) const {


    SiCellId id = cellIdOfPosition(pos);

    return id.isValid();
}

// Used in surfaceChargesGenerator
double StripAnnulusDesign::scaledDistanceToNearestDiode(SiLocalPosition const &pos) const {

    SiCellId cellId = cellIdOfPosition(pos);
    SiLocalPosition posStrip = localPositionOfCell(cellId);
    int strip, row;
    getStripRow(cellId, &strip, &row);

    return fabs(pos.xPhi() - posStrip.xPhi()) / m_pitch[row];
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

SiLocalPosition StripAnnulusDesign::positionFromStrip(const int stripNumber1D) const {
    return localPositionOfCell(SiCellId(stripNumber1D, 0));
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
    int strip, row;
    getStripRow(cellId, &strip, &row);
    if (row < 0 || row >= m_nRows || strip < m_firstStrip[row] ||  strip >= m_firstStrip[row + 1]) {
        return SiCellId(); // Invalid
    }
    return cellId;
}

double StripAnnulusDesign::length() const { // DEPRECATED: which row? assume 0 here
    return m_stripEndRadius[0] - m_stripStartRadius[0];
}

double StripAnnulusDesign::width() const { // DEPRECATED
    return m_pitch[0] * m_nStrips[0];
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
} // namespace InDetDD
