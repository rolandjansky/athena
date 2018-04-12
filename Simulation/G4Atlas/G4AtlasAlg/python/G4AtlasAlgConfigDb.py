# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm, addTool


addTool("G4AtlasAlg.G4AtlasAlgConfig.getAthenaTrackingActionTool",
        "G4UA::AthenaTrackingActionTool")
addTool("G4AtlasAlg.G4AtlasAlgConfig.getAthenaStackingActionTool",
        "G4UA::AthenaStackingActionTool")

# Main Algorithm for AtlasG4
addAlgorithm("G4AtlasAlg.G4AtlasAlgConfig.getG4AtlasAlg", "G4AtlasAlg")
