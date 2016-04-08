# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("G4DebuggingTools.G4DebuggingToolsToolsConf.VolumeDebugger", "VolumeDebugger")
addTool("G4DebuggingTools.G4DebuggingToolsToolsConf.G4AtlantisDumper", "G4AtlantisDumper")
addTool("G4DebuggingTools.G4DebuggingToolsToolsConf.VerboseSelector", "VerboseSelector")
addTool("G4DebuggingTools.G4DebuggingToolsToolsConf.StepNtuple", "StepNtuple")
addTool("G4DebuggingTools.G4DebuggingToolsToolsConf.EnergyConservationTest", "EnergyConservationTest")
addTool("G4DebuggingTools.G4DebuggingToolsToolsConf.HyperspaceCatcher", "HyperspaceCatcher")

