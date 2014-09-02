# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS inner detector description
"""

__author__  = 'A. Dell`Acqua, M. Gallas, A. DiSimone'


import PyG4Atlas, AtlasG4Eng
from SimFlags import SimFlags


## -- PIXEL --
class Pixel(object):
    """
    Describes the ATLAS Pixel detector.

    Sets the max step size to 50 micron for consistency with G3 in:
      Pixel::siBLayLog and Pixel::siLog
    """

    def __init__(self, allowMods=False):
        """ Describes the geometry.
        """
        self.atlas_pixel = PyG4Atlas.DetFacility("GeoDetector","Pixel:Pixel",allowMods)
        # If we are using the old multiple scattering model, include step limitation
        if SimFlags.PhysicsList.get_Value().endswith("_EMV"):
          self.atlas_pixel.add_MaxStepInVolumes('Pixel::siBLayLog', 0.050)
          self.atlas_pixel.add_MaxStepInVolumes('Pixel::siLog', 0.050)

    def _initSD(self):
        """ Describes the sensitive detector, including BCM and BLM.
        """
        self.atlas_pixel.add_SenDetector('PixelG4_SD','PixelSensorSD', 'PSD','Pixel::siBLayLog')
        self.atlas_pixel.add_SenDetector('PixelG4_SD','PixelSensorSD', 'PSD','Pixel::siLog')
        self.atlas_pixel.add_SenDetector('PixelG4_SD','PixelSensorSD', 'PSD','Pixel::dbmDiamondLog')

        self.atlas_pixel.add_SenDetector('BCM_G4_SD','BCMSensorSD', 'BCMSD','Pixel::bcmDiamondLog')
        self.atlas_pixel.add_SenDetector('BLM_G4_SD','BLMSensorSD', 'BLMSD','Pixel::blmDiamondLog')

    def _initPR(self):
        """
            Here are defined the physics regions, used for production cuts
        """
        self.atlas_pixel_PhysReg=PyG4Atlas.PhysicsReg('Pixel')
        self.atlas_pixel_PhysReg.add_Volumes('Pixel::siLog')
        self.atlas_pixel_PhysReg.add_Volumes('Pixel::siBLayLog')
        self.atlas_pixel_PhysReg.add_Cuts('e-',0.05)
        self.atlas_pixel_PhysReg.add_Cuts('e+',0.05)
        self.atlas_pixel_PhysReg.add_Cuts('gamma',0.05)
        self.atlas_pixel.add_PhysicsReg(self.atlas_pixel_PhysReg)


## -- SCT --
class SCT(object):
    """
    Describes the ATLAS SCT detector.
    """
    def __init__(self, allowMods=False):
        """
        Describes the geometry.

        Sets the max step size to 50 micron for consistency with G3 in:
          SCT::BRLSensor, SCT::BRLSensorSS, SCT::BRLSensorMS, 
          SCT::ECSensor0, SCT::ECSensor1, SCT::ECSensor2, 
          SCT::ECSensor3, SCT::ECSensor4, SCT::ECSensor5
        """
        self.atlas_sct = PyG4Atlas.DetFacility("GeoDetector", "SCT:SCT",allowMods)
        # If we are using the old multiple scattering model, include step limitation
        if SimFlags.PhysicsList.get_Value().endswith('_EMV'):
          self.atlas_sct.add_MaxStepInVolumes('SCT::BRLSensor', 0.050)
          self.atlas_sct.add_MaxStepInVolumes('SCT::BRLSensorSS', 0.050)
          self.atlas_sct.add_MaxStepInVolumes('SCT::BRLSensorMS', 0.050)
          self.atlas_sct.add_MaxStepInVolumes('SCT::ECSensor0', 0.050)
          self.atlas_sct.add_MaxStepInVolumes('SCT::ECSensor1', 0.050)
          self.atlas_sct.add_MaxStepInVolumes('SCT::ECSensor2', 0.050)
          self.atlas_sct.add_MaxStepInVolumes('SCT::ECSensor3', 0.050)
          self.atlas_sct.add_MaxStepInVolumes('SCT::ECSensor4', 0.050)
          self.atlas_sct.add_MaxStepInVolumes('SCT::ECSensor5', 0.050)

    def _initSD(self):
        """
        Describes the sensitive detector.
        """
        self.atlas_sct.add_SenDetector('SCT_G4_SD','SctSensorSD', 'SCTSD','SCT::BRLSensor')
        self.atlas_sct.add_SenDetector('SCT_G4_SD','SctSensorSD', 'SCTSD','SCT::BRLSensorSS')
        self.atlas_sct.add_SenDetector('SCT_G4_SD','SctSensorSD', 'SCTSD','SCT::BRLSensorMS')
        self.atlas_sct.add_SenDetector('SCT_G4_SD','SctSensorSD', 'SCTSD','SCT::ECSensor0')
        self.atlas_sct.add_SenDetector('SCT_G4_SD','SctSensorSD', 'SCTSD','SCT::ECSensor1')
        self.atlas_sct.add_SenDetector('SCT_G4_SD','SctSensorSD', 'SCTSD','SCT::ECSensor2')
        self.atlas_sct.add_SenDetector('SCT_G4_SD','SctSensorSD', 'SCTSD','SCT::ECSensor3')
        self.atlas_sct.add_SenDetector('SCT_G4_SD','SctSensorSD', 'SCTSD','SCT::ECSensor4')
        self.atlas_sct.add_SenDetector('SCT_G4_SD','SctSensorSD', 'SCTSD','SCT::ECSensor5')

    def _initPR(self):
        """
        Here are defined the physics regions, used for production cuts
        """
        self.atlas_sct_PhysReg=PyG4Atlas.PhysicsReg('SCT')
        self.atlas_sct_PhysReg.add_Volumes('SCT::BRLSensor')
        self.atlas_sct_PhysReg.add_Volumes('SCT::BRLSensorSS')
        self.atlas_sct_PhysReg.add_Volumes('SCT::BRLSensorMS')
        self.atlas_sct_PhysReg.add_Volumes('SCT::ECSensor0')
        self.atlas_sct_PhysReg.add_Volumes('SCT::ECSensor1')
        self.atlas_sct_PhysReg.add_Volumes('SCT::ECSensor2')
        self.atlas_sct_PhysReg.add_Volumes('SCT::ECSensor3')
        self.atlas_sct_PhysReg.add_Volumes('SCT::ECSensor4')
        self.atlas_sct_PhysReg.add_Volumes('SCT::ECSensor5')
        self.atlas_sct_PhysReg.add_Cuts('e-',0.05)
        self.atlas_sct_PhysReg.add_Cuts('e+',0.05)
        self.atlas_sct_PhysReg.add_Cuts('gamma',0.05)
        self.atlas_sct.add_PhysicsReg(self.atlas_sct_PhysReg)


## -- TRT --
class TRT(object):
    """
    Describes the ATLAS TRT detector.
    """

    def __init__(self,allowMods=False):
        """
        Describes the geometry.
        """
        self.atlas_trt=PyG4Atlas.DetFacility("GeoDetector","TRT:TRT",allowMods)

    def _initSD(self):
        """
        Describes the sensitive detector.
        """
        self.atlas_trt.add_SenDetector('TRT_G4Utilities:TRT_G4_SD',
                                       'TRTSensitiveDetector','TRTSD','TRT::Gas')
        self.atlas_trt.add_SenDetector('TRT_G4Utilities:TRT_G4_SD',
                                       'TRTSensitiveDetector','TRTSD','TRT::GasMA')
        self.atlas_trt.add_SenDetector('TRT_G4Utilities:TRT_G4_SD',
                                       'TRTSensitiveDetector','TRTSD','TRT::Gas_Ar')
        self.atlas_trt.add_SenDetector('TRT_G4Utilities:TRT_G4_SD',
                                       'TRTSensitiveDetector','TRTSD','TRT::GasMA_Ar')

    def _initTRTProcess(self):
        """
        Activates the TRT process.
        """
        AtlasG4Eng.G4Eng.load_Lib("TRT_TR_Process")
        AtlasG4Eng.G4Eng.read_XML("TRgeomodelgeometry.xml")

    def _initPR(self):
        """
        Here are defined the physics regions, used for production cuts
        """
        self.atlas_trt_PhysReg=PyG4Atlas.PhysicsReg('TRT')
        self.atlas_trt_PhysReg.add_Volumes('TRT::Gas')
        self.atlas_trt_PhysReg.add_Volumes('TRT::GasMA')
        self.atlas_trt_PhysReg.add_Cuts('e-',0.05)
        self.atlas_trt_PhysReg.add_Cuts('e+',0.05)
        self.atlas_trt_PhysReg.add_Cuts('gamma',0.05)
        self.atlas_trt.add_PhysicsReg(self.atlas_trt_PhysReg)
        self.atlas_trt_PhysReg_Ar=PyG4Atlas.PhysicsReg('TRT_Ar')
        self.atlas_trt_PhysReg_Ar.add_Volumes('TRT::Gas_Ar')
        self.atlas_trt_PhysReg_Ar.add_Volumes('TRT::GasMA_Ar')
        self.atlas_trt_PhysReg_Ar.add_Cuts('e-',30.0)
        self.atlas_trt_PhysReg_Ar.add_Cuts('e+',30.0)
        self.atlas_trt_PhysReg_Ar.add_Cuts('gamma',0.05)
        self.atlas_trt.add_PhysicsReg(self.atlas_trt_PhysReg_Ar)


# -- IDET material services --
class IDetServicesMat(object):
    """
    Describes the IDET services material.
    """

    def __init__(self):
        """
        Describes the geometry.
        """
        self.atlas_idetServMat=PyG4Atlas.DetFacility("GeoDetector", "InDetServMat:InDetServMat")

