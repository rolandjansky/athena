# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("MCTruthBase.MCTruthBaseConfig.getMCTruthSteppingAction", "MCTruthSteppingAction")

# V2 user actions
addTool("MCTruthBase.MCTruthBaseConfig.getMCTruthSteppingActionTool",
        "G4UA::MCTruthSteppingActionTool")
