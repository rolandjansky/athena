# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("Quirks.QuirksConfigLegacy.getQuirkPhysicsTool"         , "QuirkPhysicsTool" )
addTool("Quirks.QuirksConfigLegacy.getDebugSteppingActionTool"  , "G4UA::DebugSteppingActionTool" )
