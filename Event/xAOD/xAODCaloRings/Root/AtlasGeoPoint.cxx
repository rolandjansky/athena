/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AtlasGeoPoint.cxx 670601 2015-05-28 14:22:49Z wsfreund $ 

// Athena framework includes:
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

// Local includes:
#include "xAODCaloRings/tools/AtlasGeoPoint.h"

const float AtlasGeoPoint::PHI_MIN = CaloPhiRange::phi_min();
const float AtlasGeoPoint::PHI_MAX = CaloPhiRange::phi_max();

#if !(RINGER_USE_NEW_CPP_FEATURES)
const float AtlasGeoPoint::ETA_MIN = -5;
const float AtlasGeoPoint::ETA_MAX = 5;
#else
constexpr float AtlasGeoPoint::ETA_MIN;
constexpr float AtlasGeoPoint::ETA_MAX;
#endif

