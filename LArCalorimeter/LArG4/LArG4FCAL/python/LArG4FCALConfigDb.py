# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService

addService("LArG4FCAL.LArG4FCALConfig.getFCAL1Calculator","FCAL1Calculator")
addService("LArG4FCAL.LArG4FCALConfig.getFCAL2Calculator","FCAL2Calculator")
addService("LArG4FCAL.LArG4FCALConfig.getFCAL3Calculator","FCAL3Calculator")
addService("LArG4FCAL.LArG4FCALConfig.getFCAL1CalibCalculator","FCAL1CalibCalculator")
addService("LArG4FCAL.LArG4FCALConfig.getFCAL2CalibCalculator","FCAL2CalibCalculator")
addService("LArG4FCAL.LArG4FCALConfig.getFCAL3CalibCalculator","FCAL3CalibCalculator")
