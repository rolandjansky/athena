/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Geo2G4/GenericVolumeBuilder.h"
#include "Geo2G4/ParameterisedVolumeBuilder.h"
#include "Geo2G4/ExtParameterisedVolumeBuilder.h"
#include "Geo2G4/SingleLVCopyBuilder.h"

void InitializeBuilders()
{
  GenericVolumeBuilder *gn __attribute__ ((unused)) = new GenericVolumeBuilder("Generic_Volume_Builder");
  ParameterisedVolumeBuilder *pb __attribute__ ((unused)) = new ParameterisedVolumeBuilder("Parameterised_Volume_Builder");
  ExtParameterisedVolumeBuilder *epb __attribute__ ((unused)) = new ExtParameterisedVolumeBuilder("Extended_Parameterised_Volume_Builder");
  SingleLVCopyBuilder *sb __attribute__ ((unused)) = new SingleLVCopyBuilder("Single_LV_Copy_Builder");
}
