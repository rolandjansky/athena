/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ExtParameterisedVolumeBuilder.h"
#include "Geo2G4AssemblyFactory.h"

void InitializeBuilders(Geo2G4AssemblyFactory* assemblyFactory)
{
  ExtParameterisedVolumeBuilder *epb __attribute__ ((unused)) =
    new ExtParameterisedVolumeBuilder("Extended_Parameterised_Volume_Builder", assemblyFactory);
}
