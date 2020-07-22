/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <memory>

#include "ExtParameterisedVolumeBuilder.h"
#include "Geo2G4AssemblyFactory.h"

typedef std::unordered_map<std::string, std::unique_ptr<VolumeBuilder>> BuilderMap;

BuilderMap InitializeBuilders(Geo2G4AssemblyFactory *assemblyFactory)
{
    BuilderMap map;
    map.emplace("Extended_Parameterised_Volume_Builder",
                std::make_unique<ExtParameterisedVolumeBuilder>("Extended_Parameterised_Volume_Builder", assemblyFactory));
    return map;
}
