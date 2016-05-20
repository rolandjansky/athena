# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getMCTruthUserAction"              , "ISFMCTruthUserAction")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getMCTruthUserActionTool"              , "ISFMCTruthUserActionTool")

addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getPhysicsValidationUserAction"              , "ISFG4PhysicsValidationUserAction")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getG4OnlyPhysicsValidationUserAction"        , "G4OnlyPhysicsValidationUserAction")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getAFII_G4PhysicsValidationUserAction"       , "AFII_G4PhysicsValidationUserAction")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getQuasiStableG4PhysicsValidationUserAction" , "QuasiStableG4PhysicsValidationUserAction")

addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getTrackProcessorUserAction"              , "ISFG4TrackProcessorUserAction")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getFullG4TrackProcessorUserAction"        , "FullG4TrackProcessorUserAction")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getPassBackG4TrackProcessorUserAction"    , "PassBackG4TrackProcessorUserAction")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getAFII_G4TrackProcessorUserAction"       , "AFII_G4TrackProcessorUserAction")

addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getTrackProcessorUserActionTool"              , "ISFG4TrackProcessorUserActionTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getFullG4TrackProcessorUserActionTool"        , "FullG4TrackProcessorUserActionTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getPassBackG4TrackProcessorUserActionTool"    , "PassBackG4TrackProcessorUserActionTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getAFII_G4TrackProcessorUserActionTool"       , "AFII_G4TrackProcessorUserActionTool")


addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getG4TransportTool"                       , "ISFG4TransportTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getFullG4TransportTool"                   , "FullG4TransportTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getPassBackG4TransportTool"               , "PassBackG4TransportTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getAFII_G4TransportTool"                  , "AFII_G4TransportTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getQuasiStableG4TransportTool"            , "QuasiStableG4TransportTool")
