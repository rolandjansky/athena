# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
- This module adds materials to the G4AtlasSimulation engine. Within the
AtlasG4Eng.py the "standard materials" are already defined by default.
- The materials already defined can be found at:
     AtlasG4Eng.G4Eng.Dict_Materials
- The list of elements is:
     AtlasG4Eng.G4Eng.List_Elements
- To add the materials here defined, import this module from your
python simulation description (ex: AtlasSim.py) after the instance of
the G4AtlasSimulation engine. You can also import one of the materials
here defined.
"""

__author__ = 'M. Gallas'

from AthenaCommon.SystemOfUnits import *
#import PyG4Atlas,AtlasG4Eng
from ISF_Geant4Config.iGeant4 import G4Eng

#=======================================================================
# - ATLAS materials  -
# - "CompositeMaterial" definitions
__doc__+="""\n material_concrete  =  material Concrete"""
G4Eng.add_CompositeMaterial("Concrete",2.3*g/cm3)
material_concrete=G4Eng.Dict_Materials.get("Concrete")
material_concrete.AddMaterial("Silicon",0.227915)
material_concrete.AddMaterial("Oxygen",0.605410)
material_concrete.AddMaterial("Hydrogen",0.099720)
material_concrete.AddMaterial("Calcium",0.049860)
material_concrete.AddMaterial("Aluminum",0.014245)
material_concrete.AddMaterial("Iron",0.002850)

__doc__+="""\n material_scintillator  =  material Scintillator"""
G4Eng.add_CompositeMaterial("Scintillator",1.032*g/cm3)
material_scintillator=G4Eng.Dict_Materials.get("Scintillator")
material_scintillator.AddElement("C",1)
material_scintillator.AddElement("H",1)

__doc__+="""\n material_mylar  =  material Mylar"""
G4Eng.add_CompositeMaterial("Mylar",1.39*g/cm3)
material_mylar=G4Eng.Dict_Materials.get("Mylar")
material_mylar.AddElement("H",4)
material_mylar.AddElement("C",5)
material_mylar.AddElement("O",2)

__doc__+="""\n material_rock  =  material Rock"""
G4Eng.add_CompositeMaterial("Rock",2.33*g/cm3)
material_rock=G4Eng.Dict_Materials.get("Rock")
material_rock.AddElement("Si",1)


#=======================================================================
