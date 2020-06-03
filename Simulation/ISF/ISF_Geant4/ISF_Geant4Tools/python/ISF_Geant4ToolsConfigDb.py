# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getMCTruthUserActionTool"              , "ISFMCTruthUserActionTool")

addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getPhysicsValidationUserActionTool"              , "ISFG4PhysicsValidationUserActionTool")

addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getTrackProcessorUserActionTool"              , "ISFG4TrackProcessorUserActionTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getFullG4TrackProcessorUserActionTool"        , "FullG4TrackProcessorUserActionTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getPassBackG4TrackProcessorUserActionTool"    , "PassBackG4TrackProcessorUserActionTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getAFII_G4TrackProcessorUserActionTool"       , "AFII_G4TrackProcessorUserActionTool")


addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getG4TransportTool"                       , "ISFG4TransportTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getFullG4TransportTool"                   , "FullG4TransportTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getPassBackG4TransportTool"               , "PassBackG4TransportTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getAFII_G4TransportTool"                  , "AFII_G4TransportTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getQuasiStableG4TransportTool"            , "QuasiStableG4TransportTool")
addTool("ISF_Geant4Tools.ISF_Geant4ToolsConfig.getAFII_QS_G4TransportTool"               , "AFII_QS_G4TransportTool")
