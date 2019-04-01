# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getMCTruthUserActionTool"              , "ISFMCTruthUserActionTool")

addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getPhysicsValidationUserActionTool"              , "ISFG4PhysicsValidationUserActionTool")

addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getTrackProcessorUserActionTool"              , "ISFG4TrackProcessorUserActionTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getFullG4TrackProcessorUserActionTool"        , "FullG4TrackProcessorUserActionTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getPassBackG4TrackProcessorUserActionTool"    , "PassBackG4TrackProcessorUserActionTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getAFII_G4TrackProcessorUserActionTool"       , "AFII_G4TrackProcessorUserActionTool")

addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getGeant4Tool",               "ISF_Geant4Tool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getFullGeant4Tool",           "ISF_FullGeant4Tool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getLongLivedGeant4Tool",      "ISF_LongLivedGeant4Tool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getPassBackGeant4Tool",       "ISF_PassBackGeant4Tool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getAFIIGeant4Tool",           "ISF_AFIIGeant4Tool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getAFII_QS_Geant4Tool",       "AFII_QS_Geant4Tool")
