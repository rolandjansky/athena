# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("G4ExtraProcesses.G4ExtraProcessesConfigLegacy.getG4EMProcessesPhysicsTool",
        "G4EMProcessesPhysicsTool")
