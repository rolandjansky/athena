/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETREADOUTGEOMETRY_STRIPANNULUSDESIGN_H
#define INDETREADOUTGEOMETRY_STRIPANNULUSDESIGN_H

//
//    Annulus shaped sensor design, no stereo angle. Developed (i) as a stepping stone to the more difficult
//    StripStereoAnnulusDesign (ii) so we can at some time compare performance with one side of a petal radial, the other 
//    stereo. Totally untested as of 17/Feb/2015; I moved on to the StripStereoAnnulusDesign as soon as this 
//    compiled and linked.
//
//    Local reference system is centred on the beamline (i.e. not in the wafer centre; it is made in GeoModel as
//    an intersection of a tube with a (translated) generic trap. x is along the centre strip; y is in phi direction;
//    z is the depth direction. Strips are on the +ve z side.
//
//        /)       ^ y
//       /  )      |
//      )   )      --> x
//       \  )
//        \)       z towards you and you are looking at the strip side
//
//    A sensor can have many rows of strips, with varying numbers of strips in each row. Row-lengths can be different.
//
//    Strips are identified by a single index, starting at 0 in the inner-most row low-y side, 
//    and ending at totalNStrips - 1 
//    
//

// Base class
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"

#include <vector>
#include <stdexcept> // For throw stuff
#include "InDetReadoutGeometry/SiCellId.h"
#include "CLHEP/Geometry/Vector3D.h" // For unused phiMeasureSegment

namespace Trk {
class RectangleBounds;
class SurfaceBounds;
}

namespace InDetDD {
class SiDiodesParameters;

class StripAnnulusDesign: public SCT_ModuleSideDesign {
public:
    StripAnnulusDesign(const SiDetectorDesign::Axis stripDirection,
                   const SiDetectorDesign::Axis thicknessDirection,
                   const double thickness,
                   const int readoutSide,
                   const InDetDD::CarrierType carrier,
                   const int nRows,
                   const std::vector<int> nStrips,
                   const std::vector<double> pitch,
                   const std::vector<double> stripStart,
                   const std::vector<double> stripEnd);

    ~StripAnnulusDesign();

    // Copy constructor and assignment:
    StripAnnulusDesign(const StripAnnulusDesign &design);
    StripAnnulusDesign &operator = (const StripAnnulusDesign &design);

    HepGeom::Point3D<double> sensorCenter() const;
    void getStripRow(SiCellId cellId, int *strip, int *row) const;
    int strip1Dim(int strip, int row) const;
    int diodesInRow(const int row) const;
//
//    Pure virtual methods in base class:
//
    // Distance to nearest detector active edge (+ve = inside, -ve = outside)
    void distanceToDetectorEdge(const SiLocalPosition &localPosition, double &etaDist,
                                double &phiDist) const;

    // check if the position is in active area
    bool inActiveArea(const SiLocalPosition &chargePos, bool checkBondGap = false) const;

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


   void closestStripRowOfPosition(SiLocalPosition const &pos, int & strip, int & row ) const; 

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
    std::vector<int> m_nStrips;
    std::vector<int> m_firstStrip; // Length is one more than the number of rows; Last entry is total number of strips. 
    std::vector<double> m_pitch;
    std::vector<double> m_stripStartRadius;
    std::vector<double> m_stripEndRadius;
    Trk::RectangleBounds *m_bounds;
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline int StripAnnulusDesign::diodesInRow(const int row) const {
    return m_nStrips[row];
}

// Unfortunately SCT introduced the name stripPitch as an alternative to phiPitch so
// everything gets doubled

inline double StripAnnulusDesign::stripPitch(const SiLocalPosition &pos) const {
    return phiPitch(pos);
}

inline double StripAnnulusDesign::stripPitch(const SiCellId &cellId) const {
    return phiPitch(cellId);
}

inline double StripAnnulusDesign::stripPitch() const { // Don't use; which strip?
    return phiPitch();
}

inline double StripAnnulusDesign::phiPitch(const SiLocalPosition &pos) const {
    return phiPitch(cellIdOfPosition(pos));
}

inline double StripAnnulusDesign::phiPitch(const SiCellId &cellId) const {
    int strip, row;
    getStripRow(cellId, &strip, &row);
    return m_pitch[row];
}

inline double StripAnnulusDesign::phiPitch() const {
    return stripPitch(SiCellId(0, 0));
}

/*
   inline double StripAnnulusDesign::etaStripPatternCentre() const {
    return 0.0;
   }

   inline double StripAnnulusDesign::phiStripPatternCentre() const {
    return 0.0;
   }
 */

inline bool StripAnnulusDesign::nearBondGap(const SiLocalPosition &, double) const {
// No bond gap in strip modules
    return false;
}

inline SiReadoutCellId StripAnnulusDesign::readoutIdOfCell(const SiCellId &cellId) const {
    int strip1D = cellId.phiIndex();
    return SiReadoutCellId(strip1D, 0);
}

inline int StripAnnulusDesign::row(int stripId1Dim) const {
    std::vector<int>::const_iterator endPtr = std::upper_bound(m_firstStrip.begin(), m_firstStrip.end(), stripId1Dim);
    return std::distance(m_firstStrip.begin(), endPtr) - 1;
}

inline int StripAnnulusDesign::strip(int stripId1Dim) const {
    int rowNum = row(stripId1Dim);
    return stripId1Dim - m_firstStrip[rowNum];
}

HepGeom::Vector3D<double> StripAnnulusDesign::phiMeasureSegment(const SiLocalPosition & /*position*/)
const {
    throw std::runtime_error("Call to phiMeasureSegment, DEPRECATED, not implemented.");
}

/// DEPRECATED: Unused (2014)
void StripAnnulusDesign::distanceToDetectorEdge(SiLocalPosition const & /*pos*/,
                                            double & /*etaDist*/,
                                            double & /*phiDist*/) const {
    throw std::runtime_error(
              "Call to distanceToDetectorEdge which is Deprecated and not yet implemented");
}

/// DEPRECATED for StripAnnulusDesign; no dead area
double StripAnnulusDesign::deadAreaUpperBoundary() const {
    return 0.;
}

double StripAnnulusDesign::deadAreaLowerBoundary() const {
    return 0.;
}

double StripAnnulusDesign::deadAreaLength() const {
    return 0.;
}

bool StripAnnulusDesign::swapHitPhiReadoutDirection() const {
    return false;
}

bool StripAnnulusDesign::swapHitEtaReadoutDirection() const {
    return false;
}
} // namespace InDetDD
#endif // INDETREADOUTGEOMETRY_STRIPANNULUSDESIGN_H
