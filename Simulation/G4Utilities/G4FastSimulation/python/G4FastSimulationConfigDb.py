# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("G4FastSimulation.G4FastSimulationConfigLegacy.getSimpleFastKiller",        "SimpleFastKiller")
addTool("G4FastSimulation.G4FastSimulationConfigLegacy.getDeadMaterialShower",      "DeadMaterialShower")
addTool("G4FastSimulation.G4FastSimulationConfigLegacy.getFastCaloSim",             "FastCaloSim")
