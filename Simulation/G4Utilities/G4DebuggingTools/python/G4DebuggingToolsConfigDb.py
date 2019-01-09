# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("G4DebuggingTools.G4DebuggingToolsConfig.getVolumeDebuggerTool", "G4UA::VolumeDebuggerTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfig.getG4AtlantisDumperTool", "G4UA::G4AtlantisDumperTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfig.getVerboseSelectorTool", "G4UA::VerboseSelectorTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfig.getStepNtupleTool", "G4UA::StepNtupleTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfig.getStepHistogramTool", "G4UA::StepHistogramTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfig.getEnergyConservationTestTool", "G4UA::EnergyConservationTestTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfig.getHyperspaceCatcherTool", "G4UA::HyperspaceCatcherTool")
addTool("G4DebuggingTools.G4DebuggingToolsConf.G4UA__CheckActivationTool", "G4UA::CheckActivationTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfig.getGeant4SetupCheckerTool","G4UA::Geant4SetupCheckerTool")
