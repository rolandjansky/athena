# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("MCTruthBase.MCTruthBaseConfigLegacy.getMCTruthSteppingActionTool",
        "G4UA::MCTruthSteppingActionTool")
