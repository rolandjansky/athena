/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETREADOUTGEOMETRY_STRIPBOXDESIGN_H
#define INDETREADOUTGEOMETRY_STRIPBOXDESIGN_H

//
//    Upgrade strip barrel sensor with several rows of strips 
//

// Base class
#include "SCT_ReadoutGeometry/SCT_ModuleSideDesign.h"

#include "GeoModelKernel/GeoDefinitions.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "TrkSurfaces/RectangleBounds.h"

#include "CLHEP/Geometry/Vector3D.h" // For unused phiMeasureSegment

#include <stdexcept> // For throw stuff
#include <vector>

namespace Trk {
  class SurfaceBounds;
}

namespace InDetDD {
class SiDiodesParameters;

class StripBoxDesign: public SCT_ModuleSideDesign {
public:
    StripBoxDesign(); // Just for access to Axis; or can it be private?

    StripBoxDesign(const SiDetectorDesign::Axis stripDirection,
                   const SiDetectorDesign::Axis thicknessDirection,
                   const double thickness,
                   const int readoutSide,
                   const InDetDD::CarrierType carrier,
                   const int nRows,
                   const int nStrips,
                   const double pitch,
                   const double length);

    ~StripBoxDesign() = default;

    // Copy constructor and assignment:
    StripBoxDesign(const StripBoxDesign &design);
    StripBoxDesign &operator = (const StripBoxDesign &design);
//
//    I make a one-dimensional strip number to store in the SiCellId, so etaIndex is always 0,
//    even with multi-row detectors. This was an easier way to get digitization than doing 
//    the more natural 2D (strip, row) identifier. The following methods convert 1D to 2D and v.v.
//
    void getStripRow(SiCellId id, int *strip, int *row) const;
    int strip1Dim(int strip, int row) const;
    int diodes() const; 
    int diodesInRow(const int row) const; 
//
//    Pure virtual methods in base class:
//
    // Distance to nearest detector active edge (+ve = inside, -ve = outside)
    void distanceToDetectorEdge(const SiLocalPosition &localPosition, double &etaDist,
                                double &phiDist) const;

    // check if the position is in active area
    bool inActiveArea(const SiLocalPosition &chargePos, bool checkBondGap = true) const;

    // Element boundary
    const Trk::SurfaceBounds &bounds() const;

    // Retrieve the two ends of a "strip"
    std::pair<SiLocalPosition, SiLocalPosition> endsOfStrip(
        const SiLocalPosition &position) const;

    // Phi-pitch (strip-width). Two names for same thing
    double stripPitch(const SiLocalPosition &localPosition) const;
    double stripPitch(const SiCellId &cellId) const;
    double stripPitch() const;
    double phiPitch(const SiLocalPosition &localPosition) const;
    double phiPitch(const SiCellId &cellId) const;
    double phiPitch() const;

    // distance to the nearest diode in units of pitch, from 0.0 to 0.5,
    // this method should be fast as it is called for every surface charge
    // in the SCT_SurfaceChargesGenerator
    // an active area check, done in the Generator anyway, is removed here
    double scaledDistanceToNearestDiode(const SiLocalPosition &chargePos) const;

    // readout or diode id -> position, size
    SiDiodesParameters parameters(const SiCellId &cellId) const;
    SiLocalPosition localPositionOfCell(const SiCellId &cellId) const;
    SiLocalPosition localPositionOfCluster(const SiCellId &cellId, int clusterSize) const;

    // position -> id
    SiCellId cellIdOfPosition(const SiLocalPosition &localPos) const;
    // id to position
    SiLocalPosition positionFromStrip(const SiCellId &cellId) const;
    SiLocalPosition positionFromStrip(const int stripNumber) const;

    // row and strip from 1-dim strip number
    int row(int stripId1Dim) const;
    int strip(int stripId1Dim) const;

    // Find and fill a vector with all neighbour strips of a given cell
    void neighboursOfCell(const SiCellId &cellId,
                          std::vector<SiCellId> &neighbours) const;
    SiCellId cellIdInRange(const SiCellId &) const;

    // For Strip sensors, readout cell == diode cell. Overload the SCT_ModuleSideDesign
    // member
    SiReadoutCellId readoutIdOfCell(const SiCellId &cellId) const;

    // ---------------------------------------------------------------------------------------
    // DEPRECATED at least for Strips
    HepGeom::Vector3D<double> phiMeasureSegment(const SiLocalPosition &position) const;

    // Method to calculate length of a strip. Which strip??
    double length() const;

    // Method to calculate average width of a module. What is it used for??
    double width() const;

    // Method to calculate minimum width of a module
    double minWidth() const;

    // Method to calculate maximum width of a module
    double maxWidth() const;

    // Pitch in eta direction Deprecated for strips: it varies in endcap
    double etaPitch() const;

    // Return true if hit local direction is the same as readout direction.
    bool swapHitPhiReadoutDirection() const;
    bool swapHitEtaReadoutDirection() const;

    bool nearBondGap(const SiLocalPosition &, double) const;

    // Transform from SiHit to GeoModel frame
    const GeoTrf::Transform3D SiHitToGeoModel() const;

    // ------------------------------------------------------------------------------------------

//
//    Accessors
//
    double pitch(const SiCellId &cellId) const;
    double stripLength(const SiCellId &cellId) const;

    // Give upper and lower boundaries, and length, of dead area
    double deadAreaUpperBoundary() const;
    double deadAreaLowerBoundary() const;
    double deadAreaLength() const;
private:
    int m_nRows;
    int m_nStrips;
    double m_pitch;
    double m_length;
    Trk::RectangleBounds m_bounds;
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline int StripBoxDesign::diodes() const { // Total diodes
    return m_nRows * m_nStrips;
}

inline int StripBoxDesign::diodesInRow(const int /* row */) const {
    return m_nStrips;
}

// Unfortunately SCT introduced the name stripPitch as an alternative to phiPitch so
// everything gets doubled

inline double StripBoxDesign::stripPitch(const SiLocalPosition & /*pos not used */) const {
    return m_pitch;
}

inline double StripBoxDesign::stripPitch(const SiCellId & /*cellId not used */) const {
    return m_pitch;
}

inline double StripBoxDesign::stripPitch() const {
    return m_pitch;
}

inline double StripBoxDesign::phiPitch(const SiLocalPosition & /*pos not used */) const {
    return m_pitch;
}

inline double StripBoxDesign::phiPitch(const SiCellId & /*cellId not used */) const {
    return m_pitch;
}

inline double StripBoxDesign::phiPitch() const {
    return m_pitch;
}

inline bool StripBoxDesign::nearBondGap(const SiLocalPosition &, double) const {
// No bond gap in strip modules
    return false;
}

inline SiReadoutCellId StripBoxDesign::readoutIdOfCell(const SiCellId &cellId) const {
    int strip = cellId.phiIndex(); /* Gets a 1D strip id */
    int row = cellId.etaIndex();   /* is junk or 0 in 1D scheme */

    return SiReadoutCellId(strip, row);
}

inline int StripBoxDesign::row(int stripId1Dim) const {
    return stripId1Dim / m_nStrips; 
}

inline int StripBoxDesign::strip(int stripId1Dim) const {
    return stripId1Dim % m_nStrips; 
}


/// DEPRECATED for StripBoxDesign; no dead area
double StripBoxDesign::deadAreaUpperBoundary() const {
    return 0.;
}

double StripBoxDesign::deadAreaLowerBoundary() const {
    return 0.;
}

double StripBoxDesign::deadAreaLength() const {
    return 0.;
}

bool StripBoxDesign::swapHitPhiReadoutDirection() const {
    return false;
}

bool StripBoxDesign::swapHitEtaReadoutDirection() const {
    return false;
}

} // namespace InDetDD
#endif // INDETREADOUTGEOMETRY_STRIPBOXDESIGN_H
