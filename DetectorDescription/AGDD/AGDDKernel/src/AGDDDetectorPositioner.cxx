/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AGDDDetectorPositioner.h"
#include "AGDDKernel/AGDDPositionerStore.h"
#include "AGDDKernel/AGDDVolume.h"
#include "AGDDKernel/AGDDVolumeStore.h"

AGDDDetectorPositioner::AGDDDetectorPositioner(const std::string& n, GeoTrf::Transform3D t) :
  AGDDPositioner(n,t),
  theDetector(nullptr),
  theVolume(nullptr) {
}
