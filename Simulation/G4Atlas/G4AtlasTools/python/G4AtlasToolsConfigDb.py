# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("G4AtlasTools.G4AtlasToolsConfig.getFastSimulationMasterTool","FastSimulationMasterTool")
addTool("G4AtlasTools.G4AtlasToolsConfig.getSensitiveDetectorMasterTool","SensitiveDetectorMasterTool")
addTool("G4AtlasTools.G4AtlasToolsConfig.getPhysicsListToolBase","PhysicsListToolBase")