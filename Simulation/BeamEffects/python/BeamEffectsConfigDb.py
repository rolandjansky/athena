# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addAlgorithm
## Lorentz Vector Generators
addTool("BeamEffects.BeamEffectsConfigLegacy.getVertexPositionFromFile",            "VertexPositionFromFile")
addTool("BeamEffects.BeamEffectsConfigLegacy.getVertexBeamCondPositioner",          "VertexBeamCondPositioner")
addTool("BeamEffects.BeamEffectsConfigLegacy.getLongBeamspotVertexPositioner",      "LongBeamspotVertexPositioner")
addTool("BeamEffects.BeamEffectsConfigLegacy.getCrabKissingVertexPositioner",       "CrabKissingVertexPositioner")
## GenEvent Manipulators
addTool("BeamEffects.BeamEffectsConfigLegacy.getGenEventValidityChecker",           "GenEventValidityChecker")
addTool("BeamEffects.BeamEffectsConfigLegacy.getZeroLifetimePositioner",            "ZeroLifetimePositioner")
addTool("BeamEffects.BeamEffectsConfigLegacy.getGenEventVertexPositioner",          "GenEventVertexPositioner")
addTool("BeamEffects.BeamEffectsConfigLegacy.getGenEventBeamEffectBooster",         "GenEventBeamEffectBooster")
addTool("BeamEffects.BeamEffectsConfigLegacy.getGenEventRotator",                   "GenEventRotator")
## Algorithms
addAlgorithm("BeamEffects.BeamEffectsConfigLegacy.getBeamEffectsAlg",               "BeamEffectsAlg")
addAlgorithm("BeamEffects.BeamEffectsConfigLegacy.getBeamSpotFixerAlg",             "BeamSpotFixerAlg")
addAlgorithm("BeamEffects.BeamEffectsConfigLegacy.getBeamSpotReweightingAlg",       "BeamSpotReweightingAlg")

