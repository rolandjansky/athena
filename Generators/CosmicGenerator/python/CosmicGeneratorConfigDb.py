# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm

addAlgorithm("CosmicGenerator.CosmicGeneratorConfig.getInput_GenericCosmicGenerator", "GenericCosmicGenerator")
addAlgorithm("CosmicGenerator.CosmicGeneratorConfig.getInput_EvgenCosmicGenerator", "EvgenCosmicGenerator")
