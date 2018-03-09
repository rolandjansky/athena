# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService

addService("LArG4Barrel.LArG4BarrelConfig.getBarrelCryostatCalibrationCalculator","BarrelCryostatCalibrationCalculator")
addService("LArG4Barrel.LArG4BarrelConfig.getBarrelCryostatCalibrationLArCalculator","BarrelCryostatCalibrationLArCalculator")
addService("LArG4Barrel.LArG4BarrelConfig.getBarrelCryostatCalibrationMixedCalculator","BarrelCryostatCalibrationMixedCalculator")
addService("LArG4Barrel.LArG4BarrelConfig.getDMCalibrationCalculator","DMCalibrationCalculator")
addService("LArG4Barrel.LArG4BarrelConfig.getBarrelCalibrationCalculator","BarrelCalibrationCalculator")
addService("LArG4Barrel.LArG4BarrelConfig.getBarrelPresamplerCalibrationCalculator","BarrelPresamplerCalibrationCalculator")

addService("LArG4Barrel.LArG4BarrelConfig.getEMBCalculator","EMBCalculator")
addService("LArG4Barrel.LArG4BarrelConfig.getEMBPresamplerCalculator","EMBPresamplerCalculator")
addService("LArG4Barrel.LArG4BarrelConfig.getLArBarrelGeometry", "LArBarrelGeometry")
addService("LArG4Barrel.LArG4BarrelConfig.getLArBarrelPresamplerGeometry", "LArBarrelPresamplerGeometry")
