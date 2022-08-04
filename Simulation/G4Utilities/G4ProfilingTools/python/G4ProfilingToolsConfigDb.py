# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("G4ProfilingTools.G4ProfilingToolsConfigLegacy.getTestActionTimerTool", "G4UA::TestActionTimerTool")
addTool("G4ProfilingTools.G4ProfilingToolsConfigLegacy.getTestActionVPTimerTool", "G4UA::TestActionVPTimerTool")
addTool("G4ProfilingTools.G4ProfilingToolsConfigLegacy.getTestActionEHistTool", "G4UA::TestActionEHistTool")
