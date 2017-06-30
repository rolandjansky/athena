/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiDetectorDesign.cxx
//   Implementation file for class SiDetectorDesign
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 3.0 05/05/2001 David Calvet
// Modified: Grant Gorfine
///////////////////////////////////////////////////////////////////

#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiIntersect.h"

namespace InDetDD {
// Constructor with parameters:
SiDetectorDesign::SiDetectorDesign(double thickness,
                                   bool phiSymmetric,
                                   bool etaSymmetric,
                                   bool depthSymmetric,
                                   InDetDD::CarrierType carrierType,
                                   int readoutSide):
    m_etaAxis(zAxis),
    m_phiAxis(yAxis),
    m_depthAxis(xAxis),
    m_thickness(thickness),
    m_carrierType(carrierType),
    m_phiSymmetric(phiSymmetric),
    m_etaSymmetric(etaSymmetric),
    m_depthSymmetric(depthSymmetric),
    m_readoutSidePosDepth(readoutSide > 0) {
}

// Constructor with axis parameters:
SiDetectorDesign::SiDetectorDesign(double thickness,
                                   bool phiSymmetric,
                                   bool etaSymmetric,
                                   bool depthSymmetric,
                                   InDetDD::CarrierType carrierType,
                                   int readoutSide,
                                   Axis stripDirection,
                                   Axis thicknessDirection):
    m_etaAxis(stripDirection),
    m_depthAxis(thicknessDirection),
    m_thickness(thickness),
    m_carrierType(carrierType),
    m_phiSymmetric(phiSymmetric),
    m_etaSymmetric(etaSymmetric),
    m_depthSymmetric(depthSymmetric),
    m_readoutSidePosDepth(readoutSide > 0) {
    if (stripDirection == thicknessDirection) {
        throw std::runtime_error(
                  "ERROR: SiDetectorDesign called with phi and thickness directions equal");
    }
    // phiAxis is "the other one"
    m_phiAxis = static_cast<Axis> ((xAxis + yAxis + zAxis) - (stripDirection + thicknessDirection));
}

// Destructor:
SiDetectorDesign::~SiDetectorDesign() {
}

HepGeom::Point3D<double> SiDetectorDesign::sensorCenter() const {
    return HepGeom::Point3D<double> (0., 0., 0.);
}

SiIntersect SiDetectorDesign::inDetector(const SiLocalPosition &localPosition,
                                         double phiTol, double etaTol) const {
    double etaDist = 0;
    double phiDist = 0;

    distanceToDetectorEdge(localPosition, etaDist, phiDist);
    
    SiIntersect state;

    if (phiDist < -phiTol || etaDist < -etaTol) {
        state.setOut();
        return state;
    }

    if (phiDist > phiTol && etaDist > etaTol) {
        state.setIn();
        return state;
    }

    // Near boundary.
    state.setNearBoundary();
    return state;
}

void SiDetectorDesign::setSymmetry(bool phiSymmetric, bool etaSymmetric,
                                   bool depthSymmetric) {
    // Flags can be changed from true to false but not false to true.
    if (m_phiSymmetric) {
        m_phiSymmetric = phiSymmetric;
    }
    else if (phiSymmetric) {
        std::cout <<
            "SiDetectorDesign: WARNING! Attempt to allow swapping of xPhi axis direction ignored."
                  << std::endl;
    }

    if (m_etaSymmetric) {
        m_etaSymmetric = etaSymmetric;
    }
    else if (etaSymmetric) {
        std::cout <<
            "SiDetectorDesign: WARNING! Attempt to allow swapping of xEta axis direction ignored."
                  << std::endl;
    }

    if (m_depthSymmetric) {
        m_depthSymmetric = depthSymmetric;
    }
    else if (depthSymmetric) {
        std::cout <<
            "SiDetectorDesign: WARNING! Attempt to allow swapping of xDepth axis direction ignored."
                  << std::endl;
    }
}

DetectorShape SiDetectorDesign::shape() const {
    // Default is Box.
    return InDetDD::Box;
}
} // namespace InDetDD
