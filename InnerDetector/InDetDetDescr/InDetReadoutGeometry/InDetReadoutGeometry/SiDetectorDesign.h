/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiDetectorDesign.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_SIDETECTORDESIGN_H
#define INDETREADOUTGEOMETRY_SIDETECTORDESIGN_H

// Input/output classes
#include "InDetReadoutGeometry/SiDiodesParameters.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "GeoModelKernel/RCBase.h"
#include "InDetDD_Defs.h"

#include <list>
#include <vector>

class Identifier;

namespace Trk {
class SurfaceBounds;
}

namespace InDetDD {
class SiReadoutCellId;
class SiCellId;
class SiDiode;
class SiReadoutCell;
class SiLocalPosition;
class SiIntersect;

enum DetectorShape {
    Box=0, Trapezoid, Annulus, Other
};

/** @class SiDetectorDesign

   Base class for the detector design classes for Pixel and SCT.
   These hold the local description of the detector elements which are
   shared by a number of detector elements.

    @author A. Calvet, Grant Gorfine
 */

class SiDetectorDesign: public RCBase {
public:
    enum Axis {
        xAxis=0, yAxis, zAxis
    };


    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
public:
    /** Constructor
         @param thickness: Thickness of sensor
         @param phiSymmetric, etaSymmetric, depthSymmetric: Flag to indicate if
          element is symmetric about these axes. ie for rectangular sensor they
          are all true, for trapezoidal sensor etaSymmetric
          is false. They can also be used to force element not to swap directions.
         @param carrierType: Carrier type that drifts to readout
          (ie electrons for pixels and holes for SCT)
          @param readoutSide: Readout side, +ve = positive Depth Side, -ve = negative
             Depth Side */
    SiDetectorDesign(double thickness,
                     bool phiSymmetric,
                     bool etaSymmetric,
                     bool depthSymmetric,
                     InDetDD::CarrierType carrierType,
                     int readoutSide);

    SiDetectorDesign(double thickness,
                     bool phiSymmetric,
                     bool etaSymmetric,
                     bool depthSymmetric,
                     InDetDD::CarrierType carrierType,
                     int readoutSide,
                     SiDetectorDesign::Axis stripDirection,
                     SiDetectorDesign::Axis depthDirection);

    /** Destructor: */
    virtual ~SiDetectorDesign();

    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////
    /** Give strip angle in the reco frame */
    virtual double sinStripAngleReco(double phiCoord, double etaCoord) const;

    /** Give a point in the centre of a sensor. For most designs, this is the origin of the local coordinate system.
        For StripStereoAnnulusDesign, this is not the case (coordinate origin is "on the beamline") */
    virtual HepGeom::Point3D<double> sensorCenter() const;

    /** Test if point is in the active part of the detector with specified tolerances */
    SiIntersect inDetector(const SiLocalPosition &localPosition, double phiTol,
                           double etaTol) const;

    /** Test if near bond gap within tolerances, only relevant for SCT. */
    virtual bool nearBondGap(const SiLocalPosition &localPosition,
                             double etaTol) const = 0;

    /** local axis corresponding to eta direction: */
    Axis etaAxis() const;

    /** local axis corresponding to phi direction: */
    Axis phiAxis() const;

    /** local axis corresponding to depth direction: */
    Axis depthAxis() const;

    bool phiSymmetric() const;
    bool etaSymmetric() const;
    bool depthSymmetric() const;

    /** ReadoutSide. +1 = postive depth side, -1 = negative depth side. */
    int readoutSide() const;

    /** Override default symmetries to prevent swapping of axes.
       NB. Flags can be changed from true to false but not false to true. */
    void setSymmetry(bool phiSymmetric, bool etaSymmetric, bool depthSymmetric);

    ///////////////////////////////////////////////////////////////////
    // Pure virtual methods:
    ///////////////////////////////////////////////////////////////////

    /** Returns distance to nearest detector active edge
       +ve = inside
       -ve = outside */
    virtual void distanceToDetectorEdge(const SiLocalPosition &localPosition,
                                        double &etaDist, double &phiDist) const = 0;

    /** Helper method for stereo angle computation, DEPRECATED */
    virtual HepGeom::Vector3D<double> phiMeasureSegment(const SiLocalPosition &position)
    const = 0;

    /** Special method for SCT to retrieve the two ends of a "strip" */
    virtual std::pair<SiLocalPosition, SiLocalPosition> endsOfStrip(
        const SiLocalPosition &position) const = 0;

    /** Method to calculate length of a module */
    virtual double length() const = 0;

    /** Method to calculate average width of a module */
    virtual double width() const = 0;

    /** Method to calculate minimum width of a module */
    virtual double minWidth() const = 0;

    /** Method to calculate maximum width of a module */
    virtual double maxWidth() const = 0;

    /** Method which returns thickness of the silicon wafer */
    double thickness() const;

    /** Pitch in phi direction */
    virtual double phiPitch() const = 0;

    /** Pitch in phi direction */
    virtual double phiPitch(const SiLocalPosition &localPosition) const = 0;

    // ** Pitch in eta direction */
    virtual double etaPitch() const = 0;

    /** Return carrier type (ie electrons or holes) */
    InDetDD::CarrierType carrierType() const;

    /** Return true if hit local direction is the same as readout direction. */
    virtual bool swapHitPhiReadoutDirection() const = 0;
    virtual bool swapHitEtaReadoutDirection() const = 0;

    /** Shape of element */
    virtual DetectorShape shape() const;

    /**  Element boundary */
    virtual const Trk::SurfaceBounds &bounds() const = 0;

    ///////////////////////////////////////////////////////////
    //
    // The following will replace existing methods but are not all implemented yet
    //

    /** readout or diode id -> position, size */
    virtual SiDiodesParameters parameters(const SiCellId &cellId) const = 0;

    /** readout or diode id -> position. */
    virtual SiLocalPosition localPositionOfCell(const SiCellId &cellId) const = 0;

    /** number of connected cells. Generally 1 except for ganged pixels which will be 2.
     */
    virtual int numberOfConnectedCells(const SiReadoutCellId &readoutId) const = 0;

    /** readout id -> id of connected diodes.
       parameter number = 0 will refer to the primary diode and in general the
       cell number will be the same.
       NB. SiCellId cellId = connectedCell(readoutId, 0) will in general be equivalent to
           SiCellId cellId = readoutId */
    virtual SiCellId connectedCell(const SiReadoutCellId &readoutId,
                                   int number) const = 0;

    /** If cell is ganged return the id of the other cell which shares the readout
        for this cell, otherwise return an invalid id. */
    virtual SiCellId gangedCell(const SiCellId &cellId) const = 0;

    /** diode id -> readout id
        NB assignment of a SiReadoutCellId to a SiCellId is allowed so you are can
        pass SiReadoutCellId variables to functions expecting a SiCellId.  */
    virtual SiReadoutCellId readoutIdOfCell(const SiCellId &cellId) const = 0;

    /** position -> id */
    virtual SiReadoutCellId readoutIdOfPosition(const SiLocalPosition &localPos) const =
        0;

    /** position -> id */
    virtual SiCellId cellIdOfPosition(const SiLocalPosition &localPos) const = 0;

    /** Get the neighbouring diodes of a given diode:
         Cell for which the neighbours must be found
       List of cells which are neighbours of the given one */
    virtual void neighboursOfCell(const SiCellId &cellId,
                                  std::vector<SiCellId> &neighbours) const = 0;

    /** Check if cell is in range. Returns the original cellId if it is in range,
       otherwise it returns an invalid id. */
    virtual SiCellId cellIdInRange(const SiCellId &cellId) const = 0;

    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:
    SiDetectorDesign();

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:
    Axis m_etaAxis; // !< local axis corresponding to eta direction
    Axis m_phiAxis; // !< local axis corresponding to phi direction
    Axis m_depthAxis; // !< local axis corresponding to depth direction
    double m_thickness; // !< thickness of silicon sensor
    InDetDD::CarrierType m_carrierType; // !< Carrier type that drifts towards readout
                                        // !< (ie holes for SCT and electrons for pixel)
    bool m_phiSymmetric;        // !< Unable to find out what these are for. No matter how asymmetric
    bool m_etaSymmetric;        // !< a sensor is, we can still define a coordinate transformation from
    bool m_depthSymmetric;      // !< one system to another.

    bool m_readoutSidePosDepth; // !< Control which side readout is on. true = positive Depth side, 
                                // !< false = negative Depth side.

    // Disallow Copy and assignment;
    SiDetectorDesign(const SiDetectorDesign &design);
    SiDetectorDesign &operator = (const SiDetectorDesign &design);
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline double SiDetectorDesign::sinStripAngleReco(double /* x */, double /* y */) const {
    return 0.0; // pixel and barrel strip sensors always zero 
}

inline SiDetectorDesign::Axis SiDetectorDesign::etaAxis() const {
    return m_etaAxis;
}

inline SiDetectorDesign::Axis SiDetectorDesign::phiAxis() const {
    return m_phiAxis;
}

inline SiDetectorDesign::Axis SiDetectorDesign::depthAxis() const {
    return m_depthAxis;
}

inline double SiDetectorDesign::thickness() const {
    return m_thickness;
}

inline InDetDD::CarrierType SiDetectorDesign::carrierType() const {
    return m_carrierType;
}

inline bool SiDetectorDesign::phiSymmetric() const {
    return m_phiSymmetric;
}

inline bool SiDetectorDesign::etaSymmetric() const {
    return m_etaSymmetric;
}

inline bool SiDetectorDesign::depthSymmetric() const {
    return m_depthSymmetric;
}

inline int SiDetectorDesign::readoutSide() const {
    return (m_readoutSidePosDepth) ? +1 : -1;
}
}  // namespace InDetDD
#endif // INDETREADOUTGEOMETRY_SIDETECTORDESIGN_H
