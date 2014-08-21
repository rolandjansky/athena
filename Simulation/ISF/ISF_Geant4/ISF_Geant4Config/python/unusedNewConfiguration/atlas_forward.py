# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS forward detectors description
"""

__author__  = 'A. Dell`Acqua'

import PyG4Atlas, AtlasG4Eng
from PyG4Atlas import DetConfigurator
from SimFlags import SimFlags


class ForwardRegion(object):
   """Describes the whole forward region (outside the ATLAS boundaries)"""

   def __init__(self):
       """Describes the geometry."""
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
       AtlasG4Eng.G4Eng.log.debug("ZDC Simulation :: atlas_ZDC.py :: _initSD  about to make SenDetector")
       self.atlas_ZDC.add_SenDetector('ZDC_SD', 'ZDCPixelSD', 'ZDCPixelSD', 'ZDC::Pixel_Logical')
       self.atlas_ZDC.add_SenDetector('ZDC_SD', 'ZDCStripSD', 'ZDCStripSD', 'ZDC::Strip_Logical')
       AtlasG4Eng.G4Eng.log.debug("ZDC Simulation :: atlas_ZDC.py :: _initSD  SenDetector Done")

   def _initPR(self):
       """Describes the physics regions."""



class ALFA(object):
    """Describes the ATLAS ALFA detector."""

    def __init__(self):
        """Describes the geometry."""
        self.alfa = PyG4Atlas.DetFacility("GeoDetector", "ALFA:ALFA")

    def _initSD(self):
        """Describes the sensitive detector."""
        self.alfa.add_SenDetector('ALFA_G4_SD','ALFA_SensitiveDetector', 'ALFA_SensitiveDetector','ALFA::logALFA_FiberU')
        self.alfa.add_SenDetector('ALFA_G4_SD','ALFA_SensitiveDetector', 'ALFA_SensitiveDetector','ALFA::logALFA_FiberV')
        for i_alfa_fiber in xrange(64):
            self.alfa.add_SenDetector('ALFA_G4_SD', 'ALFA_SensitiveDetector',
                                      'ALFA_SensitiveDetector', 'ALFA::logALFA_FiberU[%d]' % i_alfa_fiber)
            self.alfa.add_SenDetector('ALFA_G4_SD', 'ALFA_SensitiveDetector',
                                      'ALFA_SensitiveDetector', 'ALFA::logALFA_FiberV[%d]' % i_alfa_fiber)
        self.alfa.add_SenDetector('ALFA_G4_SD','ALFA_SensitiveDetector', 'ALFA_SensitiveDetector', 'ALFA::logOD_FiberActive')
        #print 'fiber %d' % i_alfa_fiber


class Lucid(object):
    """Describes Lucid."""

    def __init__(self):
        """Describes the geometry."""
        self.lucid = PyG4Atlas.DetFacility("GeoDetector", "LUCID:LUCID")

    def _initSD(self):
        """Describes the sensitive detector."""
        self.lucid.add_SenDetector('LUCID_G4_SD', 'LUCID_SensitiveDetector', 'LUCID_SensitiveDetector', 'LUCID::lvPmt')

    def _initOpProcess(self):
        """Activates the optical process."""
        AtlasG4Eng.G4Eng.load_Lib("LUCID_OpProcess")
