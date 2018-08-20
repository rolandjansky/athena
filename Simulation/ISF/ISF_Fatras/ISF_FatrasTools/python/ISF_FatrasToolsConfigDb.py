# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("ISF_FatrasTools.ISF_FatrasToolsConfig.getFatrasPhysicsValidationTool",             "ISF_FatrasPhysicsValidationTool")
addTool("ISF_FatrasTools.ISF_FatrasToolsConfig.getFatrasParametricHadIntProcessor",         "ISF_FatrasParametricHadIntProcessor")
addTool("ISF_FatrasTools.ISF_FatrasToolsConfig.getFatrasEnergyLossUpdator",                 "ISF_FatrasEnergyLossUpdator")
addTool("ISF_FatrasTools.ISF_FatrasToolsConfig.getFatrasEnergyLossSamplerBetheHeitler",     "ISF_FatrasEnergyLossSamplerBetheHeitler")
#addTool("ISF_FatrasTools.ISF_FatrasToolsConfig.getFatrasEnergyLossSamplerBetheBloch",       "ISF_FatrasEnergyLossSamplerBetheBloch")
#addTool("ISF_FatrasTools.ISF_FatrasToolsConfig.getFatrasBetheHeitlerEnergyLossUpdator",     "ISF_FatrasBetheHeitlerEnergyLossUpdator")
addTool("ISF_FatrasTools.ISF_FatrasToolsConfig.getFatrasMultipleScatteringSamplerHighland", "ISF_FatrasMultipleScatteringSamplerHighland")
addTool("ISF_FatrasTools.ISF_FatrasToolsConfig.getFatrasMultipleScatteringSamplerGaussianMixture", "ISF_FatrasMultipleScatteringSamplerGaussianMixture")
addTool("ISF_FatrasTools.ISF_FatrasToolsConfig.getFatrasMultipleScatteringSamplerGeneralMixture", "ISF_FatrasMultipleScatteringSamplerGeneralMixture")
addTool("ISF_FatrasTools.ISF_FatrasToolsConfig.getFatrasConversionCreator",                 "ISF_FatrasConversionCreator")
addTool("ISF_FatrasTools.ISF_FatrasToolsConfig.getFatrasProcessSamplingTool",               "ISF_FatrasProcessSamplingTool")
addTool("ISF_FatrasTools.ISF_FatrasToolsConfig.getFatrasMaterialUpdator" ,                  "ISF_FatrasMaterialUpdator")
addTool("ISF_FatrasTools.ISF_FatrasToolsConfig.getFatrasMaterialEffectsEngine",             "ISF_FatrasMaterialEffectsEngine")
addTool("ISF_FatrasTools.ISF_FatrasToolsConfig.getFatrasSimTool",                           "ISF_FatrasSimTool")
addTool("ISF_FatrasTools.ISF_FatrasToolsConfig.getFatrasPileupSimTool",                     "ISF_FatrasPileupSimTool")
addTool("ISF_FatrasTools.ISF_FatrasToolsConfig.getFatrasSimEngine",                         "ISF_FatrasSimEngine")
