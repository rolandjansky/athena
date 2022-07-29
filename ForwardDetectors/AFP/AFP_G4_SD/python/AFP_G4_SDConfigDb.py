# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("AFP_G4_SD.AFP_G4_SDConfigLegacy.getAFP_SensitiveDetector"       , "AFP_SensitiveDetector" )
addTool("AFP_G4_SD.AFP_G4_SDConfigLegacy.getAFP_SiDSensitiveDetector"    , "AFP_SiDSensitiveDetector" )
addTool("AFP_G4_SD.AFP_G4_SDConfigLegacy.getAFP_TDSensitiveDetector"     , "AFP_TDSensitiveDetector" )
