# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addAlgorithm
## Lorentz Vector Generators
addTool("BeamEffects.BeamEffectsConfig.getVertexPositionFromFile",            "VertexPositionFromFile")
addTool("BeamEffects.BeamEffectsConfig.getVertexBeamCondPositioner",          "VertexBeamCondPositioner")
addTool("BeamEffects.BeamEffectsConfig.getLongBeamspotVertexPositioner",      "LongBeamspotVertexPositioner")
addTool("BeamEffects.BeamEffectsConfig.getCrabKissingVertexPositioner",       "CrabKissingVertexPositioner")
## GenEvent Manipulators
addTool("BeamEffects.BeamEffectsConfig.getGenEventValidityChecker",           "GenEventValidityChecker")
addTool("BeamEffects.BeamEffectsConfig.getZeroLifetimePositioner",            "ZeroLifetimePositioner")
addTool("BeamEffects.BeamEffectsConfig.getGenEventVertexPositioner",          "GenEventVertexPositioner")
addTool("BeamEffects.BeamEffectsConfig.getGenEventBeamEffectBooster",         "GenEventBeamEffectBooster")
addTool("BeamEffects.BeamEffectsConfig.getGenEventRotator",                   "GenEventRotator")
## Algorithms
addAlgorithm("BeamEffects.BeamEffectsConfig.getBeamEffectsAlg",               "BeamEffectsAlg")

