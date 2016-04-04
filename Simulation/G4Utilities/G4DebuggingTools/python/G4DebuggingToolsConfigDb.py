# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("G4DebuggingTools.G4DebuggingToolsConf.VolumeDebugger", "VolumeDebugger")
addTool("G4DebuggingTools.G4DebuggingToolsConf.G4AtlantisDumper", "G4AtlantisDumper")
addTool("G4DebuggingTools.G4DebuggingToolsConf.VerboseSelector", "VerboseSelector")
addTool("G4DebuggingTools.G4DebuggingToolsConf.StepNtuple", "StepNtuple")
addTool("G4DebuggingTools.G4DebuggingToolsConf.EnergyConservationTest", "EnergyConservationTest")
addTool("G4DebuggingTools.G4DebuggingToolsConf.HyperspaceCatcher", "HyperspaceCatcher")


#addTool("G4DebuggingTools.G4DebuggingToolsConfig.getVolumeDebuggerTool", "G4UA::VolumeDebuggerTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfig.getG4AtlantisDumperTool", "G4UA::G4AtlantisDumperTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfig.getVerboseSelectorTool", "G4UA::VerboseSelectorTool")
#addTool("G4DebuggingTools.G4DebuggingToolsConfig.getStepNtupleTool", "G4UA::StepNtupleTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfig.getEnergyConservationTestTool", "G4UA::EnergyConservationTestTool")
addTool("G4DebuggingTools.G4DebuggingToolsConfig.getHyperspaceCatcherTool", "G4UA::HyperspaceCatcherTool")
addTool("G4DebuggingTools.G4DebuggingToolsConf.G4UA__CheckActivationTool", "G4UA::CheckActivationTool")
