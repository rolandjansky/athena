/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ExtParameterisedVolumeBuilder.h"

void InitializeBuilders()
{
  ExtParameterisedVolumeBuilder *epb __attribute__ ((unused)) = new ExtParameterisedVolumeBuilder("Extended_Parameterised_Volume_Builder");
}
