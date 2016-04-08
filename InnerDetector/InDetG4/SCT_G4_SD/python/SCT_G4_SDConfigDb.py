# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("SCT_G4_SD.SCT_G4_SDConfig.getSctSensorSD"              , "SctSensorSD" )
addTool("SCT_G4_SD.SCT_G4_SDConfig.getSLHC_SctSensorSD"         , "SLHC_SctSensorSD" )
addTool("SCT_G4_SD.SCT_G4_SDConfig.getSLHC_SctSensorSD_Gmx"     , "SLHC_SctSensorSD_Gmx" )
addTool("SCT_G4_SD.SCT_G4_SDConfig.getSctSensor_CTB"            , "SctSensor_CTB")
