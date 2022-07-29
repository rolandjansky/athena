# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addService, addAlgorithm
addTool("LArG4SD.LArG4SDConfigLegacy.getLArActiveSensitiveDetector","LArActiveSensitiveDetector")
addTool("LArG4SD.LArG4SDConfigLegacy.getLArDeadSensitiveDetector","LArDeadSensitiveDetector")
addTool("LArG4SD.LArG4SDConfigLegacy.getLArEMBSensitiveDetector","LArEMBSensitiveDetector")
addTool("LArG4SD.LArG4SDConfigLegacy.getLArEMECSensitiveDetector","LArEMECSensitiveDetector")
addTool("LArG4SD.LArG4SDConfigLegacy.getLArFCALSensitiveDetector","LArFCALSensitiveDetector")
addTool("LArG4SD.LArG4SDConfigLegacy.getLArHECSensitiveDetector","LArHECSensitiveDetector")
addTool("LArG4SD.LArG4SDConfigLegacy.getLArInactiveSensitiveDetector","LArInactiveSensitiveDetector")
addTool("LArG4SD.LArG4SDConfigLegacy.getLArMiniFCALSensitiveDetector","LArMiniFCALSensitiveDetector")

addService("LArG4SD.LArG4SDConfigLegacy.getCalibrationDefaultCalculator","CalibrationDefaultCalculator")
addAlgorithm("LArG4SD.LArG4SDConfigLegacy.getDeadMaterialCalibrationHitMerger","DeadMaterialCalibrationHitMerger")
