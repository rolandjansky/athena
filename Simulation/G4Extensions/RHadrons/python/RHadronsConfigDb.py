# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("RHadrons.RHadronsConfig.getSG_StepNtupleTool", "G4UA::SG_StepNtupleTool")
addTool("RHadrons.RHadronsConfig.getRHadronsPhysicsTool", "RHadronsPhysicsTool")
