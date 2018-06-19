# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addService, addAlgorithm
addTool("LArG4SD.LArG4SDConfig.getLArActiveSensitiveDetector","LArActiveSensitiveDetector")
addTool("LArG4SD.LArG4SDConfig.getLArDeadSensitiveDetector","LArDeadSensitiveDetector")
addTool("LArG4SD.LArG4SDConfig.getLArEMBSensitiveDetector","LArEMBSensitiveDetector")
addTool("LArG4SD.LArG4SDConfig.getLArEMECSensitiveDetector","LArEMECSensitiveDetector")
addTool("LArG4SD.LArG4SDConfig.getLArFCALSensitiveDetector","LArFCALSensitiveDetector")
addTool("LArG4SD.LArG4SDConfig.getLArHECSensitiveDetector","LArHECSensitiveDetector")
addTool("LArG4SD.LArG4SDConfig.getLArInactiveSensitiveDetector","LArInactiveSensitiveDetector")
addTool("LArG4SD.LArG4SDConfig.getLArMiniFCALSensitiveDetector","LArMiniFCALSensitiveDetector")

addService("LArG4SD.LArG4SDConfig.getCalibrationDefaultCalculator","CalibrationDefaultCalculator")
addAlgorithm("LArG4SD.LArG4SDConfig.getDeadMaterialCalibrationHitMerger","DeadMaterialCalibrationHitMerger")
