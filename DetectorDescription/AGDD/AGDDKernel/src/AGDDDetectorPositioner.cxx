/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AGDDDetectorPositioner.h"
#include "AGDDKernel/AGDDPositionerStore.h"
#include "AGDDKernel/AGDDVolume.h"
#include "AGDDKernel/AGDDVolumeStore.h"

AGDDDetectorPositioner::AGDDDetectorPositioner(AGDDPositionerStore& ps,
                                               AGDDVolumeStore& vs,
                                               const std::string& n,
                                               const GeoTrf::Transform3D& t) :
  AGDDPositioner(ps,vs,n,t),
  theDetector(nullptr),
  theVolume(nullptr) {
}
