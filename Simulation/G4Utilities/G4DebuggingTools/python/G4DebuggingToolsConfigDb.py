# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("G4DebuggingTools.G4DebuggingToolsConfigLegacy.getVolumeDebuggerTool", "G4UA::VolumeDebuggerTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfigLegacy.getG4AtlantisDumperTool", "G4UA::G4AtlantisDumperTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfigLegacy.getVerboseSelectorTool", "G4UA::VerboseSelectorTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfigLegacy.getStepNtupleTool", "G4UA::StepNtupleTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfigLegacy.getEnergyConservationTestTool", "G4UA::EnergyConservationTestTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfigLegacy.getHyperspaceCatcherTool", "G4UA::HyperspaceCatcherTool")
addTool("G4DebuggingTools.G4DebuggingToolsConf.G4UA__CheckActivationTool", "G4UA::CheckActivationTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfigLegacy.getGeant4SetupCheckerTool","G4UA::Geant4SetupCheckerTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfigLegacy.getStepHistogramTool", "G4UA::StepHistogramTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfigLegacy.getTestBoundariesUserActionTool", "G4UA::TestBoundariesUserActionTool")
