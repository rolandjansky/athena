# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm

addAlgorithm("CosmicGenerator.CosmicGeneratorConfigLegacy.getInput_GenericCosmicGenerator", "GenericCosmicGenerator")
addAlgorithm("CosmicGenerator.CosmicGeneratorConfigLegacy.getInput_EvgenCosmicGenerator", "EvgenCosmicGenerator")
