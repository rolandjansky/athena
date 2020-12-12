/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiDetectorDesign.h
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_SIDETECTORDESIGN_H
#define INDETREADOUTGEOMETRY_SIDETECTORDESIGN_H

//Base class
#include "ReadoutGeometryBase/DetectorDesign.h"
// Input/output classes
#include "ReadoutGeometryBase/SiDiodesParameters.h"
#include "ReadoutGeometryBase/InDetDD_Defs.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "GeoPrimitives/GeoPrimitives.h"

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
   Objects are owned by SiDetectorManager.

    @author A. Calvet, Grant Gorfine
 */

 class SiDetectorDesign : public DetectorDesign {

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

    /** Test if near bond gap within tolerances, only relevant for SCT. */
    virtual bool nearBondGap(const SiLocalPosition &localPosition,
                             double etaTol) const = 0;

    /** only relevant for SCT. Return strip1Dim(int strip, int row) if SCT; otherwise -1 */
    virtual int strip1Dim(int strip, int row) const;

   
    ///////////////////////////////////////////////////////////////////
    // Pure virtual methods:
    ///////////////////////////////////////////////////////////////////
    /** Helper method for stereo angle computation, DEPRECATED */
    virtual HepGeom::Vector3D<double> phiMeasureSegment(const SiLocalPosition &position)
    const = 0;

    /** Special method for SCT to retrieve the two ends of a "strip" */
    virtual std::pair<SiLocalPosition, SiLocalPosition> endsOfStrip(
        const SiLocalPosition &position) const = 0;

    /** If cell is ganged return the id of the other cell which shares the readout
        for this cell, otherwise return an invalid id. */
    virtual SiCellId gangedCell(const SiCellId &cellId) const = 0;


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
