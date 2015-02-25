# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS MUON system description.
"""

__author__  = 'A. Dell`Acqua, M. Gallas, A. Disimone'

import math
from ISF_Geant4Config.iGeant4 import G4Eng
from ISF_Geant4Config.DetFacility import DetFacility
from ISF_Geant4Config.PhysicsReg import PhysicsReg
from ISF_Geant4Config.ISF_SimFlags import simFlags
#from SimFlags import jobproperties
from AthenaCommon.BeamFlags import jobproperties


# -- MUON --
class Muon(object):
    """ Describe the ATLAS Muon detector."""

    def __init__(self, allowMods=False):
        """ Describes the geometry.  Disables caching set up for reconstruction.
        """
        self.atlas_muon=DetFacility("GeoDetector","Muon:Muon",allowMods)


    def _initSD(self):
        """ Describes the sensitive detector.
        """
        sdsuffix = ""
        if jobproperties.Beam.beamType() == 'cosmics':
            sdsuffix = "_Cosmics"
        self.atlas_muon.add_SenDetector('MuonG4SD', 'MDTSens'+sdsuffix, 'AMS', 'Muon::SensitiveGas')
        self.atlas_muon.add_SenDetector('MuonG4SD', 'RPCSens'+sdsuffix, 'RMS', 'Muon::gazGap')
        self.atlas_muon.add_SenDetector('MuonG4SD', 'TGCSens'+sdsuffix, 'TMS', 'Muon::muo::TGCGas')
        self.atlas_muon.add_SenDetector('MuonG4SD', 'CSCSens'+sdsuffix, 'CMS', 'Muon::CscArCO2')


    def _initPR(self):
        """ Describes the physics regions.
        """
        self.atlas_muon_PhysReg=PhysicsReg('DriftWall')
        self.atlas_muon_PhysReg.add_Volumes('Muon::MDTDriftWall')
        self.atlas_muon_PhysReg.add_Cuts('e-',0.05)
        self.atlas_muon_PhysReg.add_Cuts('e+',0.05)
        self.atlas_muon.add_PhysicsReg(self.atlas_muon_PhysReg)
        self.atlas_muon_PhysReg1=PhysicsReg('DriftWall1')
        self.atlas_muon_PhysReg1.add_Volumes('Muon::Endplug')
        self.atlas_muon_PhysReg1.add_Cuts('e-',1.)
        self.atlas_muon_PhysReg1.add_Cuts('e+',1.)
        self.atlas_muon.add_PhysicsReg(self.atlas_muon_PhysReg1)
        self.atlas_muon_PhysReg2=PhysicsReg('DriftWall2')
        self.atlas_muon_PhysReg2.add_Volumes('Muon::SensitiveGas')
        self.atlas_muon_PhysReg2.add_Cuts('e-',1.)
        self.atlas_muon_PhysReg2.add_Cuts('e+',1.)
        self.atlas_muon.add_PhysicsReg(self.atlas_muon_PhysReg2)

        # Add the cavern fast sim model if necessary
        if jobproperties.SimFlags.CavernBG.statusOn and jobproperties.SimFlags.CavernBG.get_Value() != 'Read':
            # create the FastSimModel obj
            self.atlas_NeutronFastSimModel=FastSimModel('TrackWriteFastSim','NeutronFastSim')

            # add region to the FastSimModel obj
            self.atlas_muonFastRegion=PhysicsReg('MuonSystemFastRegion')
            if 'World' in jobproperties.SimFlags.CavernBG.get_Value():
                self.atlas_muonFastRegion.add_Volumes('BeamPipe::BeamPipe')
                self.atlas_muonFastRegion.add_Volumes('IDET::IDET')
            self.atlas_muonFastRegion.add_Volumes('Muon::MuonSys')
            self.atlas_muon.add_PhysicsReg(self.atlas_muonFastRegion)
            self.atlas_NeutronFastSimModel.add_Region(self.atlas_muonFastRegion)


            # add the FastSimModel to the G4Eng
            G4Eng.add_FastSimModel(self.atlas_NeutronFastSimModel)

            # if we need it, add the SD to a dummy volume
            if 'Write' in jobproperties.SimFlags.CavernBG.get_Value():
                self.atlas_muon.add_SenDetector('TrackWriteFastSim','TrackFastSimSD','TrackFastSimSD',\
                                                'Muon::DummyVolumeName')

#=======================================================================
