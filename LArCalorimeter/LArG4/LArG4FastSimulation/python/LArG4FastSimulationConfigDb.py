# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("LArG4FastSimulation.LArG4FastSimulationConfig.getBarrelFastSimDedicatedSD","BarrelFastSimDedicatedSD")
addTool("LArG4FastSimulation.LArG4FastSimulationConfig.getEndcapFastSimDedicatedSD","EndcapFastSimDedicatedSD")
addTool("LArG4FastSimulation.LArG4FastSimulationConfig.getFCALFastSimDedicatedSD",  "FCALFastSimDedicatedSD")
addTool("LArG4FastSimulation.LArG4FastSimulationConfig.getFCAL2FastSimDedicatedSD", "FCAL2FastSimDedicatedSD")

#addTool("LArG4FastSimulation.LArG4FastSimulationConfig.getDeadMaterialShower",      "DeadMaterialShower")
addTool("LArG4FastSimulation.LArG4FastSimulationConfig.getEMBFastShower",           "EMBFastShower")
addTool("LArG4FastSimulation.LArG4FastSimulationConfig.getEMECFastShower",          "EMECFastShower")
addTool("LArG4FastSimulation.LArG4FastSimulationConfig.getFCAL2FastShower",         "FCAL2FastShower")
addTool("LArG4FastSimulation.LArG4FastSimulationConfig.getFCALFastShower",          "FCALFastShower")
