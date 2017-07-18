# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService

addService("LArG4HEC.LArG4HECConfig.getLArHECLocalCalculator","LArHECLocalCalculator")
addService("LArG4HEC.LArG4HECConfig.getLocalCalibrationCalculator","LocalCalibrationCalculator")
addService("LArG4HEC.LArG4HECConfig.getLocalHECGeometry","LocalHECGeometry")

addService("LArG4HEC.LArG4HECConfig.getHECWheelCalculator","HECWheelCalculator")
addService("LArG4HEC.LArG4HECConfig.getHECCalibrationWheelActiveCalculator","HECCalibrationWheelActiveCalculator")
addService("LArG4HEC.LArG4HECConfig.getHECCalibrationWheelInactiveCalculator","HECCalibrationWheelInactiveCalculator")
addService("LArG4HEC.LArG4HECConfig.getHECCalibrationWheelDeadCalculator","HECCalibrationWheelDeadCalculator")
addService("LArG4HEC.LArG4HECConfig.getHECGeometry","HECGeometry")
