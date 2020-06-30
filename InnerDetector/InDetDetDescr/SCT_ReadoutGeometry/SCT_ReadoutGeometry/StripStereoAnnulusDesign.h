/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETREADOUTGEOMETRY_STRIPSTEREOANNULUSDESIGN_H
#define INDETREADOUTGEOMETRY_STRIPSTEREOANNULUSDESIGN_H

//
//    StereoAnnulus shaped sensor design.
//    Ref. N.P. Hessey "Building a Stereo-angle into strip-sensors for the ATLAS-Upgrade Inner-Tracker Endcaps"
//    2012.
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
#include "SCT_ReadoutGeometry/SCT_ModuleSideDesign.h"

#include "InDetReadoutGeometry/SiCellId.h"
#include "TrkSurfaces/AnnulusBounds.h"

#include "CLHEP/Geometry/Vector3D.h" // For unused phiMeasureSegment
#include "CLHEP/Geometry/Transform3D.h"

#include <vector>
#include <stdexcept> // For throw stuff

namespace Trk {
class SurfaceBounds;
}

namespace InDetDD {
class SiDiodesParameters;
//Vectors passed to constructor below will become single values once row is removed...
class StripStereoAnnulusDesign: public SCT_ModuleSideDesign {
public:
    StripStereoAnnulusDesign(const SiDetectorDesign::Axis &stripDirection,
                   const SiDetectorDesign::Axis &thicknessDirection,
                   const double &thickness,
                   const int &readoutSide,
                   const InDetDD::CarrierType &carrier,
                   const int &nRows,
                   const std::vector<int> &nStrips,
                   const std::vector<double> &pitch,
                   const std::vector<double> &stripStart,
                   const std::vector<double> &stripEnd,
                   const double &stereoAngle,
                   const double &centreR);

    ~StripStereoAnnulusDesign() = default;

    HepGeom::Point3D<double> sensorCenter() const;

    // Copy constructor and assignment:
    StripStereoAnnulusDesign(const StripStereoAnnulusDesign &design);
    StripStereoAnnulusDesign &operator = (const StripStereoAnnulusDesign &design);
    void getStripRow(SiCellId cellId, int *strip, int *row) const;
    int strip1Dim(int strip, int row) const;
    SiLocalPosition stripPosAtR(int strip, int row, double r) const;
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


    // above methods return mm
    // Use these methods if you're working with polar coordinates
    // AnnulusBoundsPC, in all other cases use phiPitch() from above
    double phiPitchPhi(const SiLocalPosition &localPosition) const;
    double phiPitchPhi(const SiCellId &cellId) const;
    double phiPitchPhi() const;

    // distance to the nearest diode in units of pitch, from 0.0 to 0.5,
    // this method should be fast as it is called for every surface charge
    // in the SCT_SurfaceChargesGenerator
    // an active area check, done in the Generator anyway, is removed here
    double scaledDistanceToNearestDiode(const SiLocalPosition &chargePos) const;

    // readout or diode id -> position, size
    SiDiodesParameters parameters(const SiCellId &cellId) const;
    SiLocalPosition localPositionOfCell(const SiCellId &cellId) const;
    SiLocalPosition localPositionOfCluster(const SiCellId &cellId, int clusterSize) const;
    
    // these return local position in STRIP PC
    // Use only if you work with the polar coordinates, in all other
    // cases, use the un-suffixes methods above
    SiLocalPosition localPositionOfCellPC(const SiCellId &cellId) const;
    SiLocalPosition localPositionOfClusterPC(const SiCellId &cellId, int clusterSize) const;

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

    /** Shape of element */
    virtual DetectorShape shape() const;

   // Transform from SiHit to GeoModel frame
   const HepGeom::Transform3D SiHitToGeoModel() const;

   double sinStripAngleReco(double phiCoord, double etaCoord) const;

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
    const int m_nRows;
    const std::vector<int> m_nStrips;
    std::vector<int> m_firstStrip; // Length is one more than the number of rows; Last entry is total number of strips. 
    const std::vector<double> m_pitch;
    const std::vector<double> m_stripStartRadius;
    const std::vector<double> m_stripEndRadius;
    const double m_stereo;
    const double m_R;
    const double m_lengthBF;
    Trk::AnnulusBounds m_bounds;
    const double m_sinStereo;
    const double m_cosStereo;
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline int  StripStereoAnnulusDesign::diodesInRow(const int row) const {
    return m_nStrips[row];
}

// Unfortunately SCT introduced the name stripPitch as an alternative to phiPitch so
// everything gets doubled

inline double StripStereoAnnulusDesign::stripPitch(const SiLocalPosition &pos) const {
    return phiPitch(pos);
}

inline double StripStereoAnnulusDesign::stripPitch(const SiCellId &cellId) const {
    return phiPitch(cellId);
}

inline double StripStereoAnnulusDesign::stripPitch() const { // Don't use; which strip?
    return phiPitch();
}

inline double StripStereoAnnulusDesign::phiPitch(const SiLocalPosition &pos) const {
// Return pitch in mm for the strip at this position, at this point's distance along the strip.
    const SiCellId cellId = cellIdOfPosition(pos);
    const int row = cellId.etaIndex();
    const double radius = sqrt(pos.xEta() * pos.xEta() + pos.xPhi() * pos.xPhi());
    return m_pitch[row] * radius;
}

inline double StripStereoAnnulusDesign::phiPitch(const SiCellId &cellId) const {
// Return pitch in mm for centre of this strip.
    const int row = cellId.etaIndex();
    return m_pitch[row] * (m_stripStartRadius[row] + m_stripEndRadius[row])*0.5;
}

inline double StripStereoAnnulusDesign::phiPitch() const {
// Return pitch in mm for the row just below or including the centre
    const int middleRow = m_nRows*0.5;
    const int middleStrip = m_nStrips[middleRow]*0.5; 
    return phiPitch(SiCellId(middleStrip, middleRow));
}

inline double StripStereoAnnulusDesign::phiPitchPhi(const SiLocalPosition &pos) const {
  // even though SiLocalPosition might not be in PC, the etaIndex should be correct
  const SiCellId cellId = cellIdOfPosition(pos);
  const int row = cellId.etaIndex();
  return m_pitch[row];
}

inline double StripStereoAnnulusDesign::phiPitchPhi(const SiCellId &cellId) const {
  const int row = cellId.etaIndex();
  return m_pitch[row];
}

inline double StripStereoAnnulusDesign::phiPitchPhi() const {
  const int middleRow = m_nRows *0.5;
  const int middleStrip = m_nStrips[middleRow] *0.5; 
  return phiPitchPhi(SiCellId(middleStrip, middleRow));
}

inline bool StripStereoAnnulusDesign::nearBondGap(const SiLocalPosition &, double) const {
// No bond gap in strip modules
    return false;
}

inline SiReadoutCellId StripStereoAnnulusDesign::readoutIdOfCell(const SiCellId &cellId) const {
    int strip = cellId.phiIndex();
    int row = cellId.etaIndex();

    return SiReadoutCellId(strip, row);
}

inline int StripStereoAnnulusDesign::row(int stripId1Dim) const {

  //This method is scheduled for deletion
    std::vector<int>::const_iterator endPtr = std::upper_bound(m_firstStrip.begin(), m_firstStrip.end(), stripId1Dim);
    int rowNum = std::distance(m_firstStrip.begin(), endPtr) - 1;
    if (rowNum < 0 || rowNum >= m_nRows) {
      const std::string errMsg=std::string("StripId1Dim index out of acceptable range ") + __FILE__+std::string(": ")+std::to_string(__LINE__);
        throw std::runtime_error(errMsg);
    }
    return rowNum;
}

inline int StripStereoAnnulusDesign::strip(int stripId1Dim) const {
    int rowNum = row(stripId1Dim);

    int strip2D = stripId1Dim - m_firstStrip[rowNum];
    if (strip2D < 0 || strip2D >= m_firstStrip[rowNum + 1]) {
        std::cout << "str1D " << stripId1Dim << " gives strip " << strip2D << " which is outside range 0 - " << 
                     m_firstStrip[rowNum + 1] << "\n";
    }
    return strip2D;
}

/// DEPRECATED for StripStereoAnnulusDesign; no dead area
inline double StripStereoAnnulusDesign::deadAreaUpperBoundary() const {
    return 0.;
}

inline double StripStereoAnnulusDesign::deadAreaLowerBoundary() const {
    return 0.;
}

inline double StripStereoAnnulusDesign::deadAreaLength() const {
    return 0.;
}

inline bool StripStereoAnnulusDesign::swapHitPhiReadoutDirection() const {
    return false;
}

inline bool StripStereoAnnulusDesign::swapHitEtaReadoutDirection() const {
    return false;
 }
} // namespace InDetDD
#endif // INDETREADOUTGEOMETRY_STRIPSTEREOANNULUSDESIGN_H
