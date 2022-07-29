# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService

addService("LArG4FCAL.LArG4FCALConfigLegacy.getFCAL1Calculator","FCAL1Calculator")
addService("LArG4FCAL.LArG4FCALConfigLegacy.getFCAL2Calculator","FCAL2Calculator")
addService("LArG4FCAL.LArG4FCALConfigLegacy.getFCAL3Calculator","FCAL3Calculator")
addService("LArG4FCAL.LArG4FCALConfigLegacy.getFCAL1CalibCalculator","FCAL1CalibCalculator")
addService("LArG4FCAL.LArG4FCALConfigLegacy.getFCAL2CalibCalculator","FCAL2CalibCalculator")
addService("LArG4FCAL.LArG4FCALConfigLegacy.getFCAL3CalibCalculator","FCAL3CalibCalculator")
