# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# flake8: noqa
# ... because this file is hopeless ...

#############################
# EgammaLWCalibration
#############################
from AthenaConfiguration.ComponentFactory import CompFactory
from CaloClusterCorrection.CaloSwLongWeights_v5 import *

class EgammaLWCalibrationBarrelConfig(CompFactory.EgammaLWCalibration):
    __slots__=[]
    def __init__(self,name="EgammaLWCalibrationBarrel"):
      super(EgammaLWCalibrationBarrelConfig,self).__init__(name)
      self.UseInterpolation=True
      self.correction=str(CaloSwLongWeights_v5_ele37)
      self.eta_start_crack=1.425
      self.eta_end_crack=1.55
      self.etamax = 2.5
      self.use_raw_eta=True
      self.preserve_offset=False
      self.degree=3
      #self.region = CALOCORR_COMBINED2

class EgammaLWCalibrationEndcapConfig(CompFactory.EgammaLWCalibration):
    __slots__=[]
    def __init__(self,name="EgammaLWCalibrationEndcap"):
      super(EgammaLWCalibrationEndcapConfig,self).__init__(name)
      self.correction=str(CaloSwLongWeights_v5_ele55)
      self.eta_start_crack=1.425
      self.eta_end_crack=1.55
      self.etamax = 2.5
      self.use_raw_eta=True
      self.preserve_offset=False
      #self.region = CALOCORR_COMBINED2


#############################
# EgammaHitsCalibration
#############################
from CaloClusterCorrection.CaloSwCalibHitsCalibration_v9 import *
from CaloClusterCorrection.CaloSwCalibHitsCalibration_v9leakdata import *

class EgammaHitsCalibrationBarrelConfig(CompFactory.EgammaHitsCalibration):
    __slots__=[]
    def __init__(self,name="EgammaHitsCalibrationBarrel"):
      super(EgammaHitsCalibrationBarrelConfig,self).__init__(name)
      self.correction=str(CaloSwCalibHitsCalibration_v9leakdata_ele37)
      self.fix_v6_pathologies=False
      self.sampling_depth = str(samplings_depths)
      self.eta_start_crack=1.425
      self.eta_end_crack=1.55
      self.etamax = 2.5
      self.use_raw_eta=True
      self.preserve_offset=False
      self.OutputLevel=0

class EgammaHitsCalibrationEndcapConfig(CompFactory.EgammaHitsCalibration):
    __slots__=[]
    def __init__(self,name="EgammaHitsCalibrationEndcap"):
      super(EgammaHitsCalibrationEndcapConfig,self).__init__(name)
      self.correction=str(CaloSwCalibHitsCalibration_v9leakdata_ele55)
      self.fix_v6_pathologies=False
      self.sampling_depth = str(samplings_depths)
      self.eta_start_crack=1.425
      self.eta_end_crack=1.55
      self.etamax = 2.5
      self.use_raw_eta=True
      self.preserve_offset=False
      self.OutputLevel=0


#############################
# EgammaGapCalibration
#############################
from CaloClusterCorrection.CaloSwGap_v4 import *

class EgammaGapCalibrationConfig(CompFactory.EgammaGapCalibration):
    __slots__=[]
    def __init__(self,name="EgammaGapCalibration"):
      super(EgammaGapCalibrationConfig,self).__init__(name)
      self.UseInterpolation=True
      self.correction=str(CaloSwGap_v4_ele37)
      self.eta_start_crack=1.425
      self.eta_end_crack=1.55
      self.degree=3
      self.use_raw_eta=False

#############################
# EgammaSshapeCalibration
#############################
from CaloClusterCorrection.CaloSwEtaoff_v4_1 import *

class EgammaSshapeCalibrationBarrelConfig(CompFactory.EgammaSshapeCalibration):
    __slots__=[]
    def __init__(self,name="EgammaSshapeCalibrationBarrel"):
        super(EgammaSshapeCalibrationBarrelConfig,self).__init__(name)
        self.correction=str(ele_layer2_etaoffv4_1_37_b)
        self.regions=str(layer2_etaoffv4_1_regions_b)
        self.energies=str(ele_etaoffv4_1_energies)
        self.forms="[[]]"
        self.energy_degree=3
        self.rangeBarrel=True
    
class EgammaSshapeCalibrationEndcapConfig(CompFactory.EgammaSshapeCalibration):
    __slots__=[]
    def __init__(self,name="EgammaSshapeCalibrationEndcap"):
        super(EgammaSshapeCalibrationEndcapConfig,self).__init__(name)
        self.correction=str(ele_layer2_etaoffv4_1_55_e)
        self.regions=str(layer2_etaoffv4_1_regions_e)
        self.energies=str(ele_etaoffv4_1_energies)
        self.forms="[[]]"
        self.energy_degree=3
        self.rangeBarrel=False


#############################
# EgammaTransitionRegions
#############################
from CaloClusterCorrection.CaloSwTransitionRegionsCorr_v5_1 import *

class EgammaTransitionRegionsConfig(CompFactory.EgammaTransitionRegions):
    __slots__=[]
    def __init__(self,name="EgammaTransitionRegionsConfig"):
      super(EgammaTransitionRegionsConfig,self).__init__(name)
      self.correction=str(CaloSwTransitionRegionsCorr_v5_1_ele)
      self.etamin_TR00 =0.001
      self.etamax_TR00 =0.015
      self.etamin_TR08 =0.78
      self.etamax_TR08 =0.82
      self.use_raw_eta=True
      self.OutputLevel=0
