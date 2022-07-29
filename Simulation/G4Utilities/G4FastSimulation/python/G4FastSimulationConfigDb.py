# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("G4FastSimulation.G4FastSimulationConfig.getSimpleFastKiller",        "SimpleFastKiller")
addTool("G4FastSimulation.G4FastSimulationConfig.getDeadMaterialShower",      "DeadMaterialShower")
addTool("G4FastSimulation.G4FastSimulationConfig.getFastCaloSim",             "FastCaloSim")
