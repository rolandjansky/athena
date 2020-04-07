/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ReadoutGeometryBase/DetectorDesign.h"

namespace InDetDD {

// Constructor with parameters:
DetectorDesign::DetectorDesign(double thickness,
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
DetectorDesign::DetectorDesign(double thickness,
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
                  "ERROR: DetectorDesign called with phi and thickness directions equal");
    }
    // phiAxis is "the other one"
    m_phiAxis = static_cast<Axis> ((xAxis + yAxis + zAxis) - (stripDirection + thicknessDirection));
}

DetectorDesign::~DetectorDesign(){}

HepGeom::Point3D<double> DetectorDesign::sensorCenter() const {
    return HepGeom::Point3D<double> (0., 0., 0.);
}

void DetectorDesign::setSymmetry(bool phiSymmetric, bool etaSymmetric,
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

DetectorShape DetectorDesign::shape() const {
    // Default is Box.
    return InDetDD::Box;
}

} // namespace InDetDD
