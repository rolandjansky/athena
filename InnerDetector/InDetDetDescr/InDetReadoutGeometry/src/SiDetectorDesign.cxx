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
#include "ReadoutGeometryBase/SiIntersect.h"

namespace InDetDD {
// Constructor with parameters:
SiDetectorDesign::SiDetectorDesign(double thickness,
                                   bool phiSymmetric,
                                   bool etaSymmetric,
                                   bool depthSymmetric,
                                   InDetDD::CarrierType carrierType,
                                   int readoutSide):
  DetectorDesign(thickness,
		 phiSymmetric,
		 etaSymmetric,
		 depthSymmetric,
		 carrierType,
		 readoutSide) {
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
  DetectorDesign(thickness,
		 phiSymmetric,
		 etaSymmetric,
		 depthSymmetric,
		 carrierType,
		 readoutSide,
		 stripDirection,
		 thicknessDirection){
}
// Destructor:
SiDetectorDesign::~SiDetectorDesign() {
}

} // namespace InDetDD
