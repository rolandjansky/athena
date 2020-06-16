/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
//    
//

// Base class
#include "SCT_ReadoutGeometry/SCT_ModuleSideDesign.h"

#include "InDetReadoutGeometry/SiCellId.h"
#include "CLHEP/Geometry/Vector3D.h" // For unused phiMeasureSegment
#include "CLHEP/Geometry/Transform3D.h"

#include <stdexcept> // For throw stuff
#include <vector>

namespace Trk {
class RectangleBounds;
class SurfaceBounds;
}

namespace InDetDD {
class SiDiodesParameters;

class StripAnnulusDesign: public SCT_ModuleSideDesign {
public:
    StripAnnulusDesign(const SiDetectorDesign::Axis &stripDirection,
                   const SiDetectorDesign::Axis &thicknessDirection,
                   const double &thickness,
                   const int &readoutSide,
                   const InDetDD::CarrierType &carrier,
                   const int &nStrips,
                   const double &pitch,
                   const double &stripStart,
                   const double &stripEnd);

    ~StripAnnulusDesign() = default;

    HepGeom::Point3D<double> sensorCenter() const;

    // Copy constructor and assignment:
    StripAnnulusDesign(const StripAnnulusDesign &design);
    StripAnnulusDesign &operator = (const StripAnnulusDesign &design);
   
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
    double stripPitch() const;
    double phiPitch(const SiLocalPosition &localPosition) const;
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
    const HepGeom::Transform3D SiHitToGeoModel() const;
    
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
    int m_nStrips; 
    double m_pitch;
    double m_stripStartRadius;
    double m_stripEndRadius;
    std::unique_ptr<Trk::RectangleBounds> m_bounds;
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

// Unfortunately SCT introduced the name stripPitch as an alternative to phiPitch so
// everything gets doubled - one of these should be removed!

inline double StripAnnulusDesign::stripPitch(const SiLocalPosition &) const {
 return phiPitch();
}

inline double StripAnnulusDesign::stripPitch() const {
     return phiPitch();
}

inline double StripAnnulusDesign::phiPitch(const SiLocalPosition &) const {
    return m_pitch;
}

inline double StripAnnulusDesign::phiPitch() const {
    return m_pitch;
}

inline bool StripAnnulusDesign::nearBondGap(const SiLocalPosition &, double) const {
// No bond gap in strip modules
    return false;
}

inline SiReadoutCellId StripAnnulusDesign::readoutIdOfCell(const SiCellId &cellId) const {
    int strip1D = cellId.phiIndex();
    return SiReadoutCellId(strip1D, 0);
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
