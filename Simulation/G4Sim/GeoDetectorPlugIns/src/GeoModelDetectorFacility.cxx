/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoDetectorPlugIns/GeoModelDetectorFacility.h"
#include "FadsGeometry/DetectorFacilityBaseT.h"
#include "G4LogicalVolume.hh"
#include "Geo2G4/Geo2G4Builder.h"

void GeoModelDetectorFacility::BuildGeometry()
{
  Geo2G4Builder Builder(GeoDetectorName);
  VolumeBuilder *b=Builder.GetVolumeBuilder(builderName);
  b->SetParam(blParamOn);
  G4LogicalVolume *temp=Builder.BuildTree();
  SetDetectorEnvelope(temp);

  if(blGetTopTransform)
    SetInitialTransformation(Builder.GetDetectorTransform());
}





