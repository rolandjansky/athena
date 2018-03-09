# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService

addService("LArG4MiniFCAL.LArG4MiniFCALConfig.getMiniFCALCalculator","MiniFCALCalculator")
addService("LArG4MiniFCAL.LArG4MiniFCALConfig.getMiniFCALActiveCalibrationCalculator","MiniFCALActiveCalibrationCalculator")
addService("LArG4MiniFCAL.LArG4MiniFCALConfig.getMiniFCALInactiveCalibrationCalculator","MiniFCALInactiveCalibrationCalculator")
addService("LArG4MiniFCAL.LArG4MiniFCALConfig.getMiniFCALDeadCalibrationCalculator","MiniFCALDeadCalibrationCalculator")

