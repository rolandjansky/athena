# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS inner detector description
"""

__author__  = 'A. Dell`Acqua, M. Gallas, A. DiSimone'


from ISF_Geant4Config.iGeant4 import G4Eng
from ISF_Geant4Config.DetFacility import DetFacility
from ISF_Geant4Config.ISF_SimFlags import simFlags
from ISF_Geant4Config.PhysicsReg import PhysicsReg


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
        self.atlas_pixel = DetFacility("GeoDetector","Pixel:Pixel",allowMods)
        # If we are using the old multiple scattering model, include step limitation
        if simFlags.PhysicsList.get_Value().endswith("_EMV"):
          self.atlas_pixel.add_MaxStepInVolumes('Pixel::siBLayLog', 0.050)
          self.atlas_pixel.add_MaxStepInVolumes('Pixel::siLog', 0.050)

    def _initSD(self):
        """ Describes the sensitive detector, including BCM and BLM.
        """
        self.atlas_pixel.add_SenDetector('PixelG4_SD','PixelSensorSD', 'PSD','Pixel::siBLayLog')
        self.atlas_pixel.add_SenDetector('PixelG4_SD','PixelSensorSD', 'PSD','Pixel::siLog')

        self.atlas_pixel.add_SenDetector('BCM_G4_SD','BCMSensorSD', 'BCMSD','Pixel::bcmDiamondLog')
        self.atlas_pixel.add_SenDetector('BLM_G4_SD','BLMSensorSD', 'BLMSD','Pixel::blmDiamondLog')

    def _initPR(self):
        """
            Here are defined the physics regions, used for production cuts
        """
        self.atlas_pixel_PhysReg=PhysicsReg('Pixel')
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
          SCT::BRLSensor, SCT::ECSensor0, SCT::ECSensor1,
          SCT::ECSensor2, SCT::ECSensor3
        """
        self.atlas_sct = DetFacility("GeoDetector", "SCT:SCT",allowMods)
        # If we are using the old multiple scattering model, include step limitation
        if simFlags.PhysicsList.get_Value().endswith('_EMV'):
          self.atlas_sct.add_MaxStepInVolumes('SCT::BRLSensor', 0.050)
          self.atlas_sct.add_MaxStepInVolumes('SCT::ECSensor0', 0.050)
          self.atlas_sct.add_MaxStepInVolumes('SCT::ECSensor1', 0.050)
          self.atlas_sct.add_MaxStepInVolumes('SCT::ECSensor2', 0.050)
          self.atlas_sct.add_MaxStepInVolumes('SCT::ECSensor3', 0.050)

    def _initSD(self):
        """
        Describes the sensitive detector.
        """
        self.atlas_sct.add_SenDetector('SCT_G4_SD','SctSensorSD', 'SCTSD','SCT::BRLSensor')
        self.atlas_sct.add_SenDetector('SCT_G4_SD','SctSensorSD', 'SCTSD','SCT::ECSensor0')
        self.atlas_sct.add_SenDetector('SCT_G4_SD','SctSensorSD', 'SCTSD','SCT::ECSensor1')
        self.atlas_sct.add_SenDetector('SCT_G4_SD','SctSensorSD', 'SCTSD','SCT::ECSensor2')
        self.atlas_sct.add_SenDetector('SCT_G4_SD','SctSensorSD', 'SCTSD','SCT::ECSensor3')

    def _initPR(self):
        """
        Here are defined the physics regions, used for production cuts
        """
        self.atlas_sct_PhysReg=PhysicsReg('SCT')
        self.atlas_sct_PhysReg.add_Volumes('SCT::BRLSensor')
        self.atlas_sct_PhysReg.add_Volumes('SCT::ECSensor0')
        self.atlas_sct_PhysReg.add_Volumes('SCT::ECSensor1')
        self.atlas_sct_PhysReg.add_Volumes('SCT::ECSensor2')
        self.atlas_sct_PhysReg.add_Volumes('SCT::ECSensor3')
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
        self.atlas_trt=DetFacility("GeoDetector","TRT:TRT",allowMods)

    def _initSD(self):
        """
        Describes the sensitive detector.
        """
        self.atlas_trt.add_SenDetector('TRT_G4Utilities:TRT_G4_SD',
                                       'TRTSensitiveDetector','TRTSD','TRT::Gas')
        self.atlas_trt.add_SenDetector('TRT_G4Utilities:TRT_G4_SD',
                                       'TRTSensitiveDetector','TRTSD','TRT::GasMA')

    def _initTRTProcess(self):
        """
        Activates the TRT process.
        """
        G4Eng.load_Lib("TRT_TR_Process")
        G4Eng.read_XML("TRgeomodelgeometry.xml")

    def _initPR(self):
        """
        Here are defined the physics regions, used for production cuts
        """
        self.atlas_trt_PhysReg=PhysicsReg('TRT')
        self.atlas_trt_PhysReg.add_Volumes('TRT::Gas')
        self.atlas_trt_PhysReg.add_Volumes('TRT::GasMA')
        self.atlas_trt_PhysReg.add_Cuts('e-',0.05)
        self.atlas_trt_PhysReg.add_Cuts('e+',0.05)
        self.atlas_trt_PhysReg.add_Cuts('gamma',0.05)
        self.atlas_trt.add_PhysicsReg(self.atlas_trt_PhysReg)


# -- IDET material services --
class IDetServicesMat(object):
    """
    Describes the IDET services material.
    """

    def __init__(self):
        """
        Describes the geometry.
        """
        self.atlas_idetServMat=DetFacility("GeoDetector", "InDetServMat:InDetServMat")
