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

// base class
#include "ReadoutGeometryBase/DetectorDesign.h"

// Input/output classes
#include "ReadoutGeometryBase/SiDiodesParameters.h"
#include "ReadoutGeometryBase/InDetDD_Defs.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

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

/** @class SiDetectorDesign

   Base class for the detector design classes for Pixel and SCT.
   These hold the local description of the detector elements which are
   shared by a number of detector elements.

    @author A. Calvet, Grant Gorfine
 */

class SiDetectorDesign: public DetectorDesign {

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
                     DetectorDesign::Axis stripDirection,
                     DetectorDesign::Axis depthDirection);


    /** Destructor: */
    virtual ~SiDetectorDesign();

    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////
    /** Give strip angle in the reco frame */
    virtual double sinStripAngleReco(double phiCoord, double etaCoord) const;

    /** Test if point is in the active part of the detector with specified tolerances */
    SiIntersect inDetector(const SiLocalPosition &localPosition, double phiTol,
                           double etaTol) const;

    /** Test if near bond gap within tolerances, only relevant for SCT. */
    virtual bool nearBondGap(const SiLocalPosition &localPosition,
                             double etaTol) const = 0;

    /** only relevant for SCT. Return strip1Dim(int strip, int row) if SCT; otherwise -1 */
    virtual int strip1Dim(int strip, int row) const;

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

    /** Pitch in phi direction */
    virtual double phiPitch() const = 0;

    /** Pitch in phi direction */
    virtual double phiPitch(const SiLocalPosition &localPosition) const = 0;

    // ** Pitch in eta direction */
    virtual double etaPitch() const = 0;

    /** Return true if hit local direction is the same as readout direction. */
    virtual bool swapHitPhiReadoutDirection() const = 0;
    virtual bool swapHitEtaReadoutDirection() const = 0;

    /**  Element boundary */
    virtual const Trk::SurfaceBounds &bounds() const = 0;

    /** Transform from SiHit to GeoModel frame */
    virtual const HepGeom::Transform3D SiHitToGeoModel() const; 

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

inline int SiDetectorDesign::strip1Dim(int, int) const{
    return -1.0;
}
}  // namespace InDetDD
#endif // INDETREADOUTGEOMETRY_SIDETECTORDESIGN_H
