# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addService
addTool("LArG4H8SD.LArG4H8SDConfig.getLArH8CalibSensitiveDetector","LArH8CalibSensitiveDetector")

addService("LArG4H8SD.LArG4H8SDConfig.getLArTBCryostatCalibrationCalculator", "LArTBCryostatCalibrationCalculator")
addService("LArG4H8SD.LArG4H8SDConfig.getH8CalibrationDefaultCalculator", "H8CalibrationDefaultCalculator")
