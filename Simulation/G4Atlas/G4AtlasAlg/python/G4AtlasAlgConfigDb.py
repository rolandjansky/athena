# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm

# Main Algorithm for AtlasG4
addAlgorithm("G4AtlasAlg.G4AtlasAlgConfig.getG4AtlasAlg", "G4AtlasAlg")
