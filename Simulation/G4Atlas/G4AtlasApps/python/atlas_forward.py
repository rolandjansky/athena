# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS forward detectors description
"""

__author__  = 'A. Dell`Acqua'

import PyG4Atlas, AtlasG4Eng
from PyG4Atlas import DetConfigurator
from SimFlags import simFlags


class ForwardRegion(object):
    """Describes the whole forward region (outside the ATLAS boundaries)"""

    def __init__(self,doDetFac=True):
        """Describes the geometry."""
        if doDetFac:
            self.atlas_ForwardRegion = PyG4Atlas.DetFacility("GeoDetector", "ForDetEnvelope:ForDetEnvelope")

class ZDC(object):
    """Describes the ATLAS ZDC detector."""

    def __init__(self):
        """Describes the geometry."""
        AtlasG4Eng.G4Eng.log.debug("ZDC Simulation :: ZDC atlas_ZDC.py :: __init__  about to make DetFacility")
        self.atlas_ZDC = PyG4Atlas.DetFacility("GeoDetector", "ZDC:ZDC")
        AtlasG4Eng.G4Eng.log.debug("ZDC Simulation :: ZDC atlas_ZDC.py :: __init__  DetFacility Done")

    def _initSD(self):
        """Describes the sensitive detector."""
        pass

    def _initPR(self):
        """Describes the physics regions."""
        pass


class ALFA(object):
     """Describes the ATLAS ALFA detector."""

     def __init__(self):
         """Describes the geometry."""
         self.alfa = PyG4Atlas.DetFacility("GeoDetector", "ALFA:ALFA")

     def _initSD(self):
         """Describes the sensitive detector."""
         pass


class Lucid(object):
     """Describes Lucid."""

     def __init__(self):
         """Describes the geometry."""
         self.lucid = PyG4Atlas.DetFacility("GeoDetector", "LUCID:LUCID")

     def _initSD(self):
         """Describes the sensitive detector."""
         pass

     def _initOpProcess(self):
         """Activates the optical process."""
         AtlasG4Eng.G4Eng.load_Lib("LUCID_OpProcess")


class AFP(object):
     """Describes AFP."""

     def __init__(self):
         """Describes the geometry."""
         self.AFP = PyG4Atlas.DetFacility("GeoDetector", "AFP:AFP_GeoModel")

     def _initSD(self):
         """Describes the SI sensitive detector."""
         pass

     def _initOpProcess(self):
         """Activates the optical process."""
         AtlasG4Eng.G4Eng.load_Lib("LUCID_OpProcess")


class FwdRegion(object):
     """Describes beam pipe in the Forward Region"""

     def __init__(self):
         """Describes the geometry."""
         self.atlas_FwdRegion = PyG4Atlas.DetFacility("GeoDetector", "FwdRegion:ForwardRegionGeoModel")

     def _initPR(self):
         """Describes the physics regions."""
         self.atlas_FwdRegion_PhysReg=PyG4Atlas.PhysicsReg('FwdRegion')
         self.atlas_FwdRegion_PhysReg.add_Volumes('FwdRegion::ForwardRegionGeoModel')
         self.atlas_FwdRegion_PhysReg.add_Cuts('e-',200.)
         self.atlas_FwdRegion_PhysReg.add_Cuts('e+',200.)
         self.atlas_FwdRegion_PhysReg.add_Cuts('gamma',200.)
         self.atlas_FwdRegion.add_PhysicsReg(self.atlas_FwdRegion_PhysReg)
