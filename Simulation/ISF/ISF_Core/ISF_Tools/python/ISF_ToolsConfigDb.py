# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF_Tools
Elmar Ritsch, 31/10/2014
"""

from AthenaCommon.CfgGetter import addTool

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
import AthenaCommon.SystemOfUnits as Units

# Common tools, services and algorithms used by jobs
addTool("ISF_Tools.ISF_ToolsConfig.getParticleHelper"                       , "ISF_ParticleHelper"                  )
addTool("ISF_Tools.ISF_ToolsConfig.getMemoryMonitor"                        , "ISF_MemoryMonitor"                   )
addTool("ISF_Tools.ISF_ToolsConfig.getCosmicEventFilterTool"                , "ISF_CosmicEventFilter"               )
addTool("ISF_Tools.ISF_ToolsConfig.getInToOutSubDetOrderingTool"            , "ISF_InToOutSubDetOrderingTool"       )
addTool("ISF_Tools.ISF_ToolsConfig.getParticleOrderingTool"                 , "ISF_ParticleOrderingTool"            )
addTool("ISF_Tools.ISF_ToolsConfig.getMC12EntryLayerFilter"                 , "ISF_MC12EntryLayerFilter"            )
addTool("ISF_Tools.ISF_ToolsConfig.getMC12LLPEntryLayerFilter"              , "ISF_MC12LLPEntryLayerFilter"         )
addTool("ISF_Tools.ISF_ToolsConfig.getMC12PlusEntryLayerFilter"             , "ISF_MC12PlusEntryLayerFilter"        )
addTool("ISF_Tools.ISF_ToolsConfig.getMC15aEntryLayerFilter"                , "ISF_MC15aEntryLayerFilter"           )
addTool("ISF_Tools.ISF_ToolsConfig.getMC15aPlusEntryLayerFilter"            , "ISF_MC15aPlusEntryLayerFilter"       )
addTool("ISF_Tools.ISF_ToolsConfig.getMC15aPlusLLPEntryLayerFilter"         , "ISF_MC15aPlusLLPEntryLayerFilter"    )
addTool("ISF_Tools.ISF_ToolsConfig.getMC15EntryLayerFilter"                 , "ISF_MC15EntryLayerFilter"            )
addTool("ISF_Tools.ISF_ToolsConfig.getMC16EntryLayerFilter"                 , "ISF_MC16EntryLayerFilter"            )
addTool("ISF_Tools.ISF_ToolsConfig.getMC16LLPEntryLayerFilter"              , "ISF_MC16LLPEntryLayerFilter"         )
addTool("ISF_Tools.ISF_ToolsConfig.getMC18EntryLayerFilter"                 , "ISF_MC18EntryLayerFilter"            )
addTool("ISF_Tools.ISF_ToolsConfig.getMC18LLPEntryLayerFilter"              , "ISF_MC18LLPEntryLayerFilter"         )
addTool("ISF_Tools.ISF_ToolsConfig.getValidationEntryLayerFilter"           , "ISF_ValidationEntryLayerFilter"      )
