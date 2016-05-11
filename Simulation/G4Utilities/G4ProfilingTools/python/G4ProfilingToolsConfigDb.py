# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("G4ProfilingTools.G4ProfilingToolsConf.TestActionTimer", "TestActionTimer")
addTool("G4ProfilingTools.G4ProfilingToolsConf.TestActionVPTimer", "TestActionVPTimer")
addTool("G4ProfilingTools.G4ProfilingToolsConf.TestActionEHist", "TestActionEHist")

# new MT user actions

addTool("G4ProfilingTools.G4ProfilingToolsConfig.getTestActionTimerTool", "G4UA::TestActionTimerTool")
addTool("G4ProfilingTools.G4ProfilingToolsConfig.getTestActionVPTimerTool", "G4UA::TestActionVPTimerTool")
addTool("G4ProfilingTools.G4ProfilingToolsConfig.getTestActionEHistTool", "G4UA::TestActionEHistTool")
