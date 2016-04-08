# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("G4FastSimulation.G4FastSimulationConfig.getSimpleFastKiller",        "SimpleFastKiller")
addTool("G4FastSimulation.G4FastSimulationConfig.getDeadMaterialShower",      "DeadMaterialShower")
