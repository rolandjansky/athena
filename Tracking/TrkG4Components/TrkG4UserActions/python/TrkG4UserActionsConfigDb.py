# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("TrkG4UserActions.TrkG4UserActionsConfigLegacy.getMaterialStepRecorderTool", "G4UA::MaterialStepRecorderTool")
addTool("TrkG4UserActions.TrkG4UserActionsConfigLegacy.getEnergyLossRecorderTool", "G4UA::EnergyLossRecorderTool")

