# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Muon detector description for the ATLAS CTB(2004) Geant4 Simulation.

 contacts:
       Daniela Rebuzzi  <rebuzzi@pv.infn.it>

"""

__author__ = 'M. Gallas'

import PyG4Atlas, AtlasG4Eng

# -- Muon System --
__doc__+="""\n ctb_muon      = Muon:Muon GeoModel detector
            to be added to the MUON envelope from ctb_common.muon."""
# envelope: MUON from ctb_common.muon
# detector geometry:
ctb_muon=PyG4Atlas.DetFacility("GeoDetector","Muon:Muon")
ctb_muon_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-34900.0,0.,0.)
ctb_muon.df.MoveTo(ctb_muon_position)
# physics-regions:
ctb_muon_PhysReg=PyG4Atlas.PhysicsReg('DriftWall')
ctb_muon_PhysReg.add_Volumes('Muon::MDTDriftWall')
ctb_muon_PhysReg.add_Cuts('e-',0.005)
ctb_muon_PhysReg.add_Cuts('e+',0.005)
ctb_muon.add_PhysicsReg(ctb_muon_PhysReg)
ctb_muon_PhysReg1=PyG4Atlas.PhysicsReg('DriftWall1')
ctb_muon_PhysReg1.add_Volumes('Muon::Endplug')
ctb_muon_PhysReg1.add_Cuts('e-',1.)
ctb_muon_PhysReg1.add_Cuts('e+',1.)
ctb_muon.add_PhysicsReg(ctb_muon_PhysReg1)
ctb_muon_PhysReg2=PyG4Atlas.PhysicsReg('DriftWall2')
ctb_muon_PhysReg2.add_Volumes('Muon::SensitiveGas')
ctb_muon_PhysReg2.add_Cuts('e-',1.)
ctb_muon_PhysReg2.add_Cuts('e+',1.)
ctb_muon.add_PhysicsReg(ctb_muon_PhysReg2)

# Muon selector at the entrance of the Muon region
# (to be added to the Muon:Muon detector facility
muonsel  = PyG4Atlas.DetFacility("ResizeableBox","MSEL")
muonsel.df.SetDx(0.1)
muonsel.df.SetDy(4050.)
muonsel.df.SetDz(2750.)
muonsel_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(12250.,0.,0.)
muonsel.df.MoveTo(muonsel_position)

#=======================================================================
