# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("G4ExtraProcesses.G4ExtraProcessesConfig.getG4EMProcessesPhysicsTool",
        "G4EMProcessesPhysicsTool")
