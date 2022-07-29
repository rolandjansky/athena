# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("LArG4FastSimulation.LArG4FastSimulationConfigLegacy.getEMBFastShower",           "EMBFastShower")
addTool("LArG4FastSimulation.LArG4FastSimulationConfigLegacy.getEMECFastShower",          "EMECFastShower")
addTool("LArG4FastSimulation.LArG4FastSimulationConfigLegacy.getFCAL2FastShower",         "FCAL2FastShower")
addTool("LArG4FastSimulation.LArG4FastSimulationConfigLegacy.getFCALFastShower",          "FCALFastShower")
