# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF_Tools
Elmar Ritsch, 31/10/2014
"""

from AthenaCommon.CfgGetter import addTool

# Common tools, services and algorithms used by jobs
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getParticleHelper"                       , "ISF_ParticleHelper"                  )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getMemoryMonitor"                        , "ISF_MemoryMonitor"                   )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getCosmicEventFilterTool"                , "ISF_CosmicEventFilter"               )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getInToOutSubDetOrderingTool"            , "ISF_InToOutSubDetOrderingTool"       )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getParticleOrderingTool"                 , "ISF_ParticleOrderingTool"            )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getEnergyParticleOrderingTool"           , "ISF_EnergyParticleOrderingTool"      )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getMC12EntryLayerFilter"                 , "ISF_MC12EntryLayerFilter"            )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getMC12LLPEntryLayerFilter"              , "ISF_MC12LLPEntryLayerFilter"         )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getMC12PlusEntryLayerFilter"             , "ISF_MC12PlusEntryLayerFilter"        )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getMC15aEntryLayerFilter"                , "ISF_MC15aEntryLayerFilter"           )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getMC15aPlusEntryLayerFilter"            , "ISF_MC15aPlusEntryLayerFilter"       )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getMC15aPlusLLPEntryLayerFilter"         , "ISF_MC15aPlusLLPEntryLayerFilter"    )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getMC15EntryLayerFilter"                 , "ISF_MC15EntryLayerFilter"            )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getMC16EntryLayerFilter"                 , "ISF_MC16EntryLayerFilter"            )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getMC16LLPEntryLayerFilter"              , "ISF_MC16LLPEntryLayerFilter"         )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getMC18EntryLayerFilter"                 , "ISF_MC18EntryLayerFilter"            )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getMC18LLPEntryLayerFilter"              , "ISF_MC18LLPEntryLayerFilter"         )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getValidationEntryLayerFilter"           , "ISF_ValidationEntryLayerFilter"      )
addTool("ISF_Tools.ISF_ToolsConfigLegacy.getStoppedParticleFilter"                , "ISF_StoppedParticleFilter"           )

addTool("ISF_Tools.ISF_ToolsConfigLegacy.getParticleKillerTool"                   , "ISF_ParticleKillerTool"              )
