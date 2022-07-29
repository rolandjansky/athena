# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService

addService("LArG4HEC.LArG4HECConfigLegacy.getLArHECLocalCalculator","LArHECLocalCalculator")
addService("LArG4HEC.LArG4HECConfigLegacy.getLocalCalibrationCalculator","LocalCalibrationCalculator")
addService("LArG4HEC.LArG4HECConfigLegacy.getLocalHECGeometry","LocalHECGeometry")

addService("LArG4HEC.LArG4HECConfigLegacy.getHECWheelCalculator","HECWheelCalculator")
addService("LArG4HEC.LArG4HECConfigLegacy.getHECCalibrationWheelActiveCalculator","HECCalibrationWheelActiveCalculator")
addService("LArG4HEC.LArG4HECConfigLegacy.getHECCalibrationWheelInactiveCalculator","HECCalibrationWheelInactiveCalculator")
addService("LArG4HEC.LArG4HECConfigLegacy.getHECCalibrationWheelDeadCalculator","HECCalibrationWheelDeadCalculator")
addService("LArG4HEC.LArG4HECConfigLegacy.getHECGeometry","HECGeometry")
