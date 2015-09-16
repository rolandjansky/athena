# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS MUON system description.
"""

__author__  = 'A. Dell`Acqua, M. Gallas, A. Disimone'

import math
import PyG4Atlas, AtlasG4Eng

from SimFlags import jobproperties
from AthenaCommon.BeamFlags import jobproperties


# -- MUON --
class Muon(object):
    """ Describe the ATLAS Muon detector."""

    def __init__(self, allowMods=False):
        """ Describes the geometry.  Disables caching set up for reconstruction.
        """
        self.atlas_muon=PyG4Atlas.DetFacility("GeoDetector","Muon:Muon",allowMods)


    def _initSD(self):
        """ Describes the sensitive detector.
        """
        pass


    def _initPR(self):
        """ Describes the physics regions.
        """
        self.atlas_muon_PhysReg=PyG4Atlas.PhysicsReg('DriftWall')
        self.atlas_muon_PhysReg.add_Volumes('Muon::MDTDriftWall')
        self.atlas_muon_PhysReg.add_Cuts('e-',0.05)
        self.atlas_muon_PhysReg.add_Cuts('e+',0.05)
        self.atlas_muon.add_PhysicsReg(self.atlas_muon_PhysReg)
        self.atlas_muon_PhysReg1=PyG4Atlas.PhysicsReg('DriftWall1')
        self.atlas_muon_PhysReg1.add_Volumes('Muon::Endplug')
        self.atlas_muon_PhysReg1.add_Cuts('e-',1.)
        self.atlas_muon_PhysReg1.add_Cuts('e+',1.)
        self.atlas_muon.add_PhysicsReg(self.atlas_muon_PhysReg1)
        self.atlas_muon_PhysReg2=PyG4Atlas.PhysicsReg('DriftWall2')
        self.atlas_muon_PhysReg2.add_Volumes('Muon::SensitiveGas')
        self.atlas_muon_PhysReg2.add_Cuts('e-',1.)
        self.atlas_muon_PhysReg2.add_Cuts('e+',1.)
        self.atlas_muon.add_PhysicsReg(self.atlas_muon_PhysReg2)

        # Add the cavern fast sim model if necessary
        if jobproperties.SimFlags.CavernBG.statusOn and jobproperties.SimFlags.CavernBG.get_Value() != 'Read' and\
           not (jobproperties.SimFlags.RecordFlux.statusOn and jobproperties.SimFlags.RecordFlux()):
            # add region to the FastSimModel obj
            self.atlas_muonFastRegion=PyG4Atlas.PhysicsReg('MuonSystemFastRegion')
            if 'World' in jobproperties.SimFlags.CavernBG.get_Value():
                self.atlas_muonFastRegion.add_Volumes('BeamPipe::BeamPipe')
                self.atlas_muonFastRegion.add_Volumes('IDET::IDET')
            self.atlas_muonFastRegion.add_Volumes('Muon::MuonSys')
            self.atlas_muon.add_PhysicsReg(self.atlas_muonFastRegion)

#=======================================================================
