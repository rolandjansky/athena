/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DetectorDesign.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef READOUTGEOMETRYBASE_DETECTORDESIGN_H
#define READOUTGEOMETRYBASE_DETECTORDESIGN_H

// base class
#include "GeoModelKernel/RCBase.h"

#include "ReadoutGeometryBase/SiDiodesParameters.h"
#include "ReadoutGeometryBase/InDetDD_Defs.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include <list>
#include <vector>

namespace InDetDD {
class SiReadoutCellId;
class SiCellId;
class SiDiode;
class SiReadoutCell;
class SiLocalPosition;
class SiIntersect;

enum DetectorShape {
  Box=0, Trapezoid, Annulus,Other
};

/** @class DetectorDesign

   Base class for the detector design classes for ITk and HGTD.
   These hold the local description of the detector elements which are
   shared by a number of detector elements.

 */

class DetectorDesign: public RCBase {
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
    DetectorDesign(double thickness,
                   bool phiSymmetric,
                   bool etaSymmetric,
                   bool depthSymmetric,
                   InDetDD::CarrierType carrierType,
                   int readoutSide);

    DetectorDesign(double thickness,
                   bool phiSymmetric,
                   bool etaSymmetric,
                   bool depthSymmetric,
                   InDetDD::CarrierType carrierType,
                   int readoutSide,
                   Axis stripDirection,
                   Axis depthDirection);

    // Destructor
    ~DetectorDesign();

    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////
    /** Return the centre of a sensor in the local reference frame.
        For most designs, this is the origin of the local reference frame.
        For StripStereoAnnulusDesign, this is not the case (coordinate origin is "on the beamline") */
    virtual HepGeom::Point3D<double> sensorCenter() const;

    /** Override default symmetries to prevent swapping of axes.
       NB. Flags can be changed from true to false but not false to true. */
    void setSymmetry(bool phiSymmetric, bool etaSymmetric, bool depthSymmetric);

    /** local axis corresponding to eta direction: */
    Axis etaAxis() const;

    /** local axis corresponding to phi direction: */
    Axis phiAxis() const;

    /** local axis corresponding to depth direction: */
    Axis depthAxis() const;

    /** Method which returns thickness of the silicon wafer */
    double thickness() const;

    /** Return carrier type (ie electrons or holes) */
    InDetDD::CarrierType carrierType() const;

    bool phiSymmetric() const;
    bool etaSymmetric() const;
    bool depthSymmetric() const;

    /** ReadoutSide. +1 = postive depth side, -1 = negative depth side. */
    int readoutSide() const;

    ///////////////////////////////////////////////////////////////////
    // Pure virtual methods:
    ///////////////////////////////////////////////////////////////////

    /** Shape of element */
    virtual DetectorShape shape() const;

    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:
    DetectorDesign();

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:
    Axis m_etaAxis; // !< local axis corresponding to eta direction
    Axis m_phiAxis; // !< local axis corresponding to phi direction
    Axis m_depthAxis; // !< local axis corresponding to depth direction
    double m_thickness; // !< thickness of silicon sensor
    InDetDD::CarrierType m_carrierType; // !< carrier type that drifts towards readout
    // !< (ie holes fro SCT and electrons for pixel)
    bool m_phiSymmetric;
    bool m_etaSymmetric;
    bool m_depthSymmetric;

    bool m_readoutSidePosDepth; // !< Control which side readout is on.
                                // !< true = positive Depth Side, false = negative Depth
                                // Side

    // Disallow Copy and assignment;
    DetectorDesign(const DetectorDesign &design);
    DetectorDesign &operator = (const DetectorDesign &design);
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline DetectorDesign::Axis DetectorDesign::etaAxis() const {
    return m_etaAxis;
}

inline DetectorDesign::Axis DetectorDesign::phiAxis() const {
    return m_phiAxis;
}

inline DetectorDesign::Axis DetectorDesign::depthAxis() const {
    return m_depthAxis;
}

inline double DetectorDesign::thickness() const {
    return m_thickness;
}

inline InDetDD::CarrierType DetectorDesign::carrierType() const {
    return m_carrierType;
}

inline bool DetectorDesign::phiSymmetric() const {
    return m_phiSymmetric;
}

inline bool DetectorDesign::etaSymmetric() const {
    return m_etaSymmetric;
}

inline bool DetectorDesign::depthSymmetric() const {
    return m_depthSymmetric;
}

inline int DetectorDesign::readoutSide() const {
    return (m_readoutSidePosDepth) ? +1 : -1;
}

}  // namespace InDetDD

#endif // READOUTGEOMETRYBASE_DETECTORDESIGN_H
