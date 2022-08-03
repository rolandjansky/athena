# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("RHadrons.RHadronsConfigLegacy.getSG_StepNtupleTool", "G4UA::SG_StepNtupleTool")
addTool("RHadrons.RHadronsConfigLegacy.getRHadronsPhysicsTool", "RHadronsPhysicsTool")
