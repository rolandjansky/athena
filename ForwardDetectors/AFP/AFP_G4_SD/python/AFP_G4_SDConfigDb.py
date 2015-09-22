# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("AFP_G4_SD.AFP_G4_SDConfig.getAFP_SensitiveDetector"       , "AFP_SensitiveDetector" )
addTool("AFP_G4_SD.AFP_G4_SDConfig.getAFP_SiDSensitiveDetector"    , "AFP_SiDSensitiveDetector" )
addTool("AFP_G4_SD.AFP_G4_SDConfig.getAFP_TDSensitiveDetector"     , "AFP_TDSensitiveDetector" )
