/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoModelDetectorDef_H
#define GeoModelDetectorDef_H
#include "FadsGeometry/DetectorFacilityBaseT.h"
#include "GeoDetectorPlugIns/GeoModelDetectorFacility.h"

#define GEODETECTORDEF(DET_NAME) static FADS::DetectorFacilityBaseT<GeoModelDetectorFacility> detPlugIn(#DET_NAME);
#endif // GeoModelDetectorDef_H
