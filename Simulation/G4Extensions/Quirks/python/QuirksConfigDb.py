# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("Quirks.QuirksConfig.getQuirkPhysicsTool"         , "QuirkPhysicsTool" )
addTool("Quirks.QuirksConfig.getDebugSteppingActionTool"  , "G4UA::DebugSteppingActionTool" )
