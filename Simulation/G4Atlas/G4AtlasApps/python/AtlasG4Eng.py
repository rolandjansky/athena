# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Geant4 simulation Python facilities for the ATLAS experiment.

"G4Eng", the instance of PyG4Atlas.G4AtlasEngine, is defined here and it will be
accessed from any other module.

The basic elements and materials are also defined here. If custom materials are
needed, they can be produced using the template atlas_materials.py.
"""

__author__ = "A. Dell`Acqua, M. Gallas"

from AthenaCommon.SystemOfUnits import *

## Create the instance we will use later in other modules.
import PyG4Atlas
G4Eng = PyG4Atlas.G4AtlasEngine()
