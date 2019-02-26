# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#from TrigT2CaloEgamma.TrigT2CaloEgammaConf import T2CaloSwCluster
from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaSamp1Fex as _EgammaSamp1Fex 
from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaSamp2Fex as _EgammaSamp2Fex
from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaEmEnFex as _EgammaEmEnFex
from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaHadEnFex as _EgammaHadEnFex
from TrigT2CaloEgamma.TrigT2CaloEgammaConf import RingerFex as _RingerFex
from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaAllFex as _EgammaAllFex
from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaReSamp1Fex
from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaReSamp2Fex
from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaReEmEnFex
from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaReHadEnFex
from TrigT2CaloEgamma.TrigT2CaloEgammaConf import T2CaloEgamma
from TrigT2CaloEgamma.TrigT2CaloEgammaConf import T2CaloEgammaFastAlgo, T2CaloEgammaReFastAlgo

from TrigT2CaloCalibration.EgammaCalibrationConfig import EgammaHitsCalibrationBarrelConfig, EgammaHitsCalibrationEndcapConfig, EgammaGapCalibrationConfig
from TrigT2CaloCalibration.EgammaCalibrationConfig import EgammaTransitionRegionsConfig

from TrigT2CaloCalibration.EgammaCalibrationConfig import EgammaSshapeCalibrationBarrelConfig, EgammaSshapeCalibrationEndcapConfig
from TrigT2CaloEgamma.TrigT2CaloEgammaMonitoring import TrigT2CaloEgammaValMonitoring, TrigT2CaloEgammaCosmicMonitoring, TrigT2CaloEgammaOnMonitoring, TrigT2CaloEgammaSwValMonitoring, TrigT2CaloEgammaSwOnMonitoring, TrigT2CaloEgammaSwCosMonitoring, TrigT2CaloEgammaTimeMonitoring, TrigT2CaloEgammaAllTimeMonitoring, TrigT2CaloEgammaRingerTimeMonitoring
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
#from TrigTimeMonitor.TrigTimeTreeToolConfig import TrigTimeTreeToolConfig

t2catime = TrigTimeHistToolConfig("Time")
#t2catime=TrigTimeTreeToolConfig("T2CaloEgammaTimer")

from AthenaCommon.Constants import VERBOSE,DEBUG

class EgammaAllFex (_EgammaAllFex):
   __slots__ = []
   def __init__ (self, name="EgammaAllFex"):
       super(EgammaAllFex, self).__init__(name)

class EgammaSamp2Fex (_EgammaSamp2Fex):
   __slots__ = []
   def __init__ (self, name="EgammaSamp2Fex"):
       super(EgammaSamp2Fex, self).__init__(name)

class EgammaSamp1Fex (_EgammaSamp1Fex):
   __slots__ = []
   def __init__ (self, name="EgammaSamp1Fex"):
       super(EgammaSamp1Fex, self).__init__(name)

class EgammaEmEnFex (_EgammaEmEnFex):
   __slots__ = []
   def __init__ (self, name="EgammaEmEnFex"):
       super(EgammaEmEnFex, self).__init__(name)

class EgammaHadEnFex (_EgammaHadEnFex):
   __slots__ = []
   def __init__ (self, name="EgammaHadEnFex"):
       super(EgammaHadEnFex, self).__init__(name)

class RingerFex (_RingerFex):
   __slots__ = []
   def __init__ (self, name="RingerFex"):
       super(RingerFex, self).__init__(name)

class EgammaSamp2FexConfig (EgammaSamp2Fex):
   __slots__ = []
   def __init__ (self, name="EgammaSamp2FexConfig"):
       super(EgammaSamp2FexConfig, self).__init__(name)
       self.MaxDetaHotCell=0.15
       self.MaxDphiHotCell=0.15

class EgammaReSamp2FexNoTimerConfig (EgammaReSamp2Fex):
   __slots__ = []
   def __init__ (self, name="EgammaReSamp2FexNoTimerConfig"):
       super(EgammaReSamp2FexNoTimerConfig, self).__init__(name)
       self.MaxDetaHotCell=0.15
       self.MaxDphiHotCell=0.15
       self.TrigTimerSvc=""

class EgammaSamp2FexNoTimerConfig (EgammaSamp2Fex):
   __slots__ = []
   def __init__ (self, name="EgammaSamp2FexNoTimerConfig"):
       super(EgammaSamp2FexNoTimerConfig, self).__init__(name)
       self.MaxDetaHotCell=0.15
       self.MaxDphiHotCell=0.15
       self.TrigTimerSvc=""

class EgammaReSamp1FexNoTimerConfig (EgammaReSamp1Fex):
   __slots__ = []
   def __init__ (self, name="EgammaReSamp1Fex"):
       super(EgammaReSamp1FexNoTimerConfig, self).__init__(name)
       self.TrigTimerSvc=""

class EgammaSamp1FexNoTimerConfig (EgammaSamp1Fex):
   __slots__ = []
   def __init__ (self, name="EgammaSamp1Fex"):
       super(EgammaSamp1FexNoTimerConfig, self).__init__(name)
       self.TrigTimerSvc=""

class EgammaReEmEnFexNoTimerConfig (EgammaReEmEnFex):
   __slots__ = []
   def __init__ (self, name="EgammaReEmEnFex"):
       super(EgammaReEmEnFexNoTimerConfig, self).__init__(name)
       self.TrigTimerSvc=""

class EgammaEmEnFexNoTimerConfig (EgammaEmEnFex):
   __slots__ = []
   def __init__ (self, name="EgammaEmEnFex"):
       super(EgammaEmEnFexNoTimerConfig, self).__init__(name)
       self.TrigTimerSvc=""

class EgammaReHadEnFexNoTimerConfig (EgammaReHadEnFex):
   __slots__ = []
   def __init__ (self, name="EgammaReHadEnFex"):
       super(EgammaReHadEnFexNoTimerConfig, self).__init__(name)
       self.TrigTimerSvc=""

class EgammaHadEnFexNoTimerConfig (EgammaHadEnFex):
   __slots__ = []
   def __init__ (self, name="EgammaHadEnFex"):
       super(EgammaHadEnFexNoTimerConfig, self).__init__(name)
       self.TrigTimerSvc=""

class T2CaloEgamma_eGamma (T2CaloEgamma):
   __slots__ = []
   def __init__ (self, name="T2CaloEgamma_eGamma"):
       super(T2CaloEgamma_eGamma, self).__init__(name)
       # here put your customizations
       samp2 = EgammaSamp2FexConfig()
       samp1 = EgammaSamp1Fex()
       sampe = EgammaEmEnFex()
       samph = EgammaHadEnFex()
       self.IAlgToolList= [ samp2,
			    samp1,
			    sampe,
			    samph]
       self.EtaWidth = 0.2
       self.PhiWidth = 0.2
       self.EtaWidthForID = 0.1
       self.PhiWidthForID = 0.1
       #self.TimerNtuple="T2CaloEgamma.T2CaEgtTot"
       self.TrigEMClusterKey="TrigT2CaloEgamma"
       #self.Monitoring=False
       #self.OutputLevel = 1
	     # detailed timing
       #t2catime=TrigTimeTreeToolConfig("T2CaloEgammaTimer")
       #t2catime=TrigTimeHistToolConfig("T2CaloEgammaTimer")
       #t2catime.Key = ["ESamp2.ESamp2Total","ESamp2.ESamp2RegSel","ESamp2.ESamp2BSCnv","ESamp2.ESamp2Algor","ESamp2.ESamp2SaveEM","ESamp2.ESamp2Eta","ESamp2.ESamp2Total:ESamp2Eta"]
       #t2catime.TimerHistLimits = [0,20]
       self.AthenaMonTools += [ TrigT2CaloEgammaValMonitoring(),
                         TrigT2CaloEgammaCosmicMonitoring(),
			 TrigT2CaloEgammaOnMonitoring(name="TrigT2CaloEgammaOnMonitoring_doTimeHist",doTimeHist=True),
 			 TrigT2CaloEgammaTimeMonitoring()]
       self.CalibListEndcap=[EgammaSshapeCalibrationEndcapConfig()]
       self.CalibListBarrel=[EgammaSshapeCalibrationBarrelConfig()]
       self.CalibListBarrel+=[EgammaHitsCalibrationBarrelConfig()]
       self.CalibListBarrel+=[EgammaGapCalibrationConfig()]
       self.CalibListBarrel+=[EgammaTransitionRegionsConfig()]
       self.CalibListEndcap+=[EgammaHitsCalibrationEndcapConfig()]
       self.CalibListEndcap+=[EgammaGapCalibrationConfig()]
       self.doTiming=True
       # code to suport Z of the vertex calculation
       self.RhoFirstLayer = [1542.22, 1543.34, 1542.56, 1543.39, 1543.10, # Eta range: 0.00 ~ 0.05 ~ 0.10 ~ 0.15 ~ 0.20 ~ 0.25
                             1543.91, 1543.19, 1541.05, 1540.18, 1538.67, # Eta range: 0.25 ~ 0.30 ~ 0.35 ~ 0.40 ~ 0.45 ~ 0.50
                             1538.55, 1536.49, 1535.31, 1535.93, 1536.14, # Eta range: 0.50 ~ 0.55 ~ 0.60 ~ 0.65 ~ 0.70 ~ 0.75
                             1535.93, 1524.32, 1524.82, 1525.86, 1523.97, # Eta range: 0.75 ~ 0.80 ~ 0.85 ~ 0.90 ~ 0.95 ~ 1.00
                             1526.66, 1524.73, 1524.00, 1522.18, 1520.65, # Eta range: 1.00 ~ 1.05 ~ 1.10 ~ 1.15 ~ 1.20 ~ 1.25
                             1519.67, 1517.90];                           # Eta range: 1.25 ~ 1.30 ~ 1.37

       self.RhoMiddleLayer= [1756.75, 1757.91, 1757.06, 1758.25, 1757.40, # Eta range: 0.00 ~ 0.05 ~ 0.10 ~ 0.15 ~ 0.20 ~ 0.25
                             1758.75, 1757.90, 1756.01, 1754.76, 1748.07, # Eta range: 0.25 ~ 0.30 ~ 0.35 ~ 0.40 ~ 0.45 ~ 0.50
                             1740.84, 1735.52, 1732.03, 1721.71, 1716.65, # Eta range: 0.50 ~ 0.55 ~ 0.60 ~ 0.65 ~ 0.70 ~ 0.75
                             1710.82, 1739.15, 1728.36, 1722.92, 1716.45, # Eta range: 0.75 ~ 0.80 ~ 0.85 ~ 0.90 ~ 0.95 ~ 1.00 
                             1707.56, 1697.96, 1689.75, 1684.23, 1671.07, # Eta range: 1.00 ~ 1.05 ~ 1.10 ~ 1.15 ~ 1.20 ~ 1.25
                             1663.98, 1662.04];                           # Eta range: 1.25 ~ 1.30 ~ 1.37

       self.RhoEta = [0,    0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40, 0.45, 0.50, 0.55, # eta point
                      0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.90, 0.95, 1.00, 1.05, 1.10, 1.15, # eta point
                      1.20, 1.25, 1.30, 1.37];

       self.ZFirstLayer = [3790.00, 3790.00, 3790.00, 3790.00, 3790.00,           # 1.52 - 1.55 ~ 1.60 - 1.65 ~ 1.70 - 1.75
                           3790.00, 3790.00, 3790.00, 3790.00, 3790.00,           # 1.75 - 1.80 ~ 1.85 - 1.90 ~ 1.95 - 2.00
                           3790.00, 3790.00, 3790.00, 3790.00, 3790.00,           # 2.00 - 2.05 ~ 2.10 - 2.15 ~ 2.20 - 2.25
                           3790.00, 3781.74, 3778.00, 3778.00, 3778.00, 3778.00]; # 2.25 - 2.30 ~ 2.35 - 2.40 ~ 2.45 - 2.50 ~ 2.55

       self.ZMiddleLayer= [4006.00, 4006.00, 4002.00, 4002.00, 4002.00,           # 1.52 - 1.55 ~ 1.60 - 1.65 ~ 1.70 - 1.75
                           3990.00, 3990.00, 3990.00, 3990.00, 3990.00,           # 1.75 - 1.80 ~ 1.85 - 1.90 ~ 1.95 - 2.00
                           3986.21, 3979.21, 3978.00, 3974.47, 3963.42,           # 2.00 - 2.05 ~ 2.10 - 2.15 ~ 2.20 - 2.25
                           3962.00, 3952.39, 3940.42, 3934.71, 3934.00, 3934.00]; # 2.25 - 2.30 ~ 2.35 - 2.40 ~ 2.45 - 2.50 ~ 2.55

       self.ZEta = [1.50, 1.55, 1.60, 1.65, 1.70, 1.75, 1.80, 1.85, 1.90, 1.95, 2.00,
                    2.05, 2.10, 2.15, 2.20, 2.25, 2.30, 2.35, 2.40, 2.45, 2.50, 2.55];

class EgammaAllFexHadConfig (EgammaAllFex):
   __slots__ = []
   def __init__ (self, name="EgammaAllFexHadConfig"):
       super(EgammaAllFexHadConfig, self).__init__(name)
       self.IncludeHad=True


class T2CaloEgamma_All (T2CaloEgamma):
   __slots__ = []
   def __init__ (self, name="T2CaloEgamma_All"):
       super(T2CaloEgamma_All, self).__init__(name)
       self.IAlgToolList= [EgammaAllFexHadConfig()]
       self.EtaWidth = 0.1
       self.PhiWidth = 0.1
       self.EtaWidthForID = 0.1
       self.PhiWidthForID = 0.1
       self.TrigEMClusterKey="TrigT2CaloAll"
       self.AthenaMonTools += [ TrigT2CaloEgammaValMonitoring(),
                         TrigT2CaloEgammaCosmicMonitoring(),
                         TrigT2CaloEgammaOnMonitoring(),
                        TrigT2CaloEgammaAllTimeMonitoring()]
       self.doTiming=True

class T2CaloEgamma_AllEm (T2CaloEgamma):
   __slots__ = []
   def __init__ (self, name="T2CaloEgamma_AllEm"):
       super(T2CaloEgamma_AllEm, self).__init__(name)
       self.IAlgToolList= [EgammaAllFex()]
       self.EtaWidth = 0.1
       self.PhiWidth = 0.1
       self.EtaWidthForID = 0.1
       self.PhiWidthForID = 0.1
       self.TrigEMClusterKey="TrigT2CaloAll"
       self.AthenaMonTools += [ TrigT2CaloEgammaValMonitoring(),
                         TrigT2CaloEgammaCosmicMonitoring(),
                         TrigT2CaloEgammaOnMonitoring(),
                        TrigT2CaloEgammaAllTimeMonitoring()]
       self.doTiming=True

class T2CaloEgamma_cells (T2CaloEgamma):
   __slots__ = []
   def __init__ (self, name="T2CaloEgamma_cells"):
       super(T2CaloEgamma_cells, self).__init__(name)
       # here put your customizations
       self.IAlgToolList= [EgammaSamp2FexConfig('EgammaSamp2FexConfigDebug'),
                           EgammaSamp1Fex('EgammaSamp1FexDebug'),
                           EgammaEmEnFex('EgammaEmEnFexDebug') ,
                           EgammaHadEnFex('EgammaHadEnFexDebug')]
       # Save cells
       for item in self.IAlgToolList:
         item.SaveCellsInContainer=True
         item.ThresholdKeepCells=400.0
         if item.name() == 'EgammaHadEnFexDebug': # HadEn should have a different threshold
           item.ThresholdKeepCells=150.0
       self.StoreCells=True
       self.EtaWidth = 0.2
       self.PhiWidth = 0.2
       self.EtaWidthForID = 0.1
       self.PhiWidthForID = 0.1
       #self.TimerNtuple="T2CaloEgamma.T2CaEgtTot"
       self.TrigEMClusterKey="TrigT2CaloEgamma"
       #t2catime.TimerHistLimits = [0,20]
       #self.AthenaMonTools += [t2catime]
       self.AthenaMonTools += [ TrigT2CaloEgammaValMonitoring(),
                         TrigT2CaloEgammaCosmicMonitoring(),
                         TrigT2CaloEgammaOnMonitoring(),
			 TrigT2CaloEgammaTimeMonitoring()]
       self.CalibListEndcap=[EgammaSshapeCalibrationEndcapConfig()]
       self.CalibListBarrel=[EgammaSshapeCalibrationBarrelConfig()]
       self.CalibListBarrel+=[EgammaHitsCalibrationBarrelConfig()]
       self.CalibListBarrel+=[EgammaGapCalibrationConfig()]
       self.CalibListBarrel+=[EgammaTransitionRegionsConfig()]
       self.CalibListEndcap+=[EgammaHitsCalibrationEndcapConfig()]
       self.CalibListEndcap+=[EgammaGapCalibrationConfig()]
       self.doTiming=True


class T2CaloEgamma_eGamma_NoHad (T2CaloEgamma):
   __slots__ = []
   def __init__ (self, name="T2CaloEgamma_eGamma_NoHad"):
       super(T2CaloEgamma_eGamma_NoHad, self).__init__(name)
       # here put your customizations
       self.IAlgToolList= [EgammaSamp2Fex(),
			   EgammaSamp1Fex(),
			   EgammaEmEnFex()]
       self.EtaWidth = 0.2
       self.PhiWidth = 0.2
       self.EtaWidthForID = 0.1
       self.PhiWidthForID = 0.1
       #self.TimerNtuple="T2CaloEgamma.T2CaEgtTot"
       self.TrigEMClusterKey="TrigT2CaloEgamma"
       #self.Monitoring=False
       #self.OutputLevel = DEBUG
       #self.AthenaMonTools += [t2catime]
       self.AthenaMonTools = [ TrigT2CaloEgammaValMonitoring(),
                         TrigT2CaloEgammaCosmicMonitoring(),
			 TrigT2CaloEgammaOnMonitoring() ]
       #self.CalibList=[]
       self.CalibListEndcap=[EgammaSshapeCalibrationEndcapConfig()]
       self.CalibListBarrel=[EgammaSshapeCalibrationBarrelConfig()]
       self.CalibListBarrel+=[EgammaHitsCalibrationBarrelConfig()]
       self.CalibListBarrel+=[EgammaGapCalibrationConfig()]
       self.CalibListBarrel+=[EgammaTransitionRegionsConfig()]
       self.CalibListEndcap+=[EgammaHitsCalibrationEndcapConfig()]
       self.CalibListEndcap+=[EgammaGapCalibrationConfig()]

# for single-beam chains starting from MBTS+BPTX
class T2CaloEgamma_MBTSA (T2CaloEgamma):
   __slots__ = []
   def __init__ (self, name="T2CaloEgamma_MBTSA"):
       super(T2CaloEgamma_MBTSA, self).__init__(name)
       # here put your customizations
       self.IAlgToolList= [EgammaSamp2FexConfig(),
			   EgammaSamp1Fex(),
			   EgammaEmEnFex() ,
			   EgammaHadEnFex()]
       self.L1ForceEta =-1.6
       self.L1ForcePhi =-1.5
       self.EtaWidth = 0.2
       self.PhiWidth = 1.6
       self.EtaWidthForID = 0.1
       self.PhiWidthForID = 0.1
       #self.TimerNtuple="T2CaloEgamma.T2CaEgtTot"
       self.TrigEMClusterKey="TrigT2CaloEgamma"
       #self.AthenaMonTools += [t2catime]
       self.AthenaMonTools += [ TrigT2CaloEgammaValMonitoring(),
                         TrigT2CaloEgammaCosmicMonitoring(),
			 TrigT2CaloEgammaOnMonitoring()]

class T2CaloEgamma_MBTSC (T2CaloEgamma):
   __slots__ = []
   def __init__ (self, name="T2CaloEgamma_MBTSC"):
       super(T2CaloEgamma_MBTSC, self).__init__(name)
       # here put your customizations
       self.IAlgToolList= [EgammaSamp2FexConfig(),
			   EgammaSamp1Fex(),
			   EgammaEmEnFex() ,
			   EgammaHadEnFex()]
       self.L1ForceEta = 1.6
       self.L1ForcePhi =-1.5
       self.EtaWidth = 0.2
       self.PhiWidth = 1.6
       self.EtaWidthForID = 0.1
       self.PhiWidthForID = 0.1
       #self.TimerNtuple="T2CaloEgamma.T2CaEgtTot"
       self.TrigEMClusterKey="TrigT2CaloEgamma"
       #self.AthenaMonTools += [t2catime]
       self.AthenaMonTools += [ TrigT2CaloEgammaValMonitoring(),
                         TrigT2CaloEgammaCosmicMonitoring(),
			 TrigT2CaloEgammaOnMonitoring()]


#class T2CaloEgamma_SwSeed (T2CaloSwSeed):
#   __slots__ = []
#   def __init__ (self, name="T2CaloEgamma_SwSeed"):
#       super(T2CaloEgamma_SwSeed, self).__init__(name)
#       self.NoCut = False
#       self.EtaWidth = 0.2
#       self.PhiWidth = 0.2
#       self.EtaWidthForID = 0.1
#       self.PhiWidthForID = 0.1
#       self.NoiseThreshold = 0
#       self.WindowThreshold = 1500
#       self.OverlapDistance = 0.1
#       self.TrigEMClusterKey="T2CaloTrigEMSwCluster"
#       #self.AthenaMonTools += [t2catime]
#       self.AthenaMonTools += [
#         TrigT2CaloEgammaSwValMonitoring(),
#         TrigT2CaloEgammaSwOnMonitoring(),
#         TrigT2CaloEgammaSwCosMonitoring()]
#       self.CalibList=[EgammaSshapeCalibrationEndcapConfig(),
#                       EgammaSshapeCalibrationBarrelConfig()]
#
#class T2CaloEgamma_SwSeed_NoCut (T2CaloEgamma_SwSeed):
#    __slots__ = []
#    def __init__(self, name = "T2CaloEgamma_SwSeed_NoCut"):
#        super( T2CaloEgamma_SwSeed_NoCut, self ).__init__( name )
#        self.NoCut = True
#

#class T2CaloEgamma_SwCluster (T2CaloSwCluster):
#   __slots__ = []
#   def __init__ (self, name="T2CaloEgamma_SwCluster"):
#       super(T2CaloEgamma_SwCluster, self).__init__(name)
#       self.IAlgToolList= [
#			   EgammaSamp1Fex(),
#			   EgammaEmEnFex(),
#			   EgammaHadEnFex()]
#       self.EtaWidth = 0.2
#       self.PhiWidth = 0.2
#       self.EtaWidthForID = 0.1
#       self.PhiWidthForID = 0.1
#       #self.TimerNtuple="T2CaloEgamma.T2CaSwCluster"
#       self.TrigEMClusterKey="T2CaloTrigEMSwCluster"
#       #self.AthenaMonTools += [t2catime]
#       self.AthenaMonTools += [
#         TrigT2CaloEgammaValMonitoring(),
#         TrigT2CaloEgammaCosmicMonitoring(),
#			   TrigT2CaloEgammaOnMonitoring()]
#       self.CalibList = [EgammaHitsCalibrationBarrelConfig(),
#                         EgammaGapCalibrationConfig()]
#       self.doTiming=True

class RingerFexConfig( RingerFex ):
  __slots__ = []
  def __init__(self, name = "RingerFexConfig"):
    super(RingerFexConfig, self).__init__(name)
    #self.TrigTimerSvc=""
    self.HltFeature = "HLT_TrigT2CaloEgamma"
    self.Feature = "TrigT2CaloEgamma"
    self.EtaBins = [0.0000, 2.5000] # bin pairs: min < eta <= max, PS,barrel,crack,endcap
    self.GlobalCenter = False;
    self.EtaSearchWindowSize = 0.1;
    self.PhiSearchWindowSize = 0.1;

    from TrigT2CaloEgamma.RingerConstants import Layer

    self.DEtaRings = [0.025, 0.003125, 0.025, 0.05, 0.1, 0.1, 0.1];
    self.DPhiRings = [0.098174770424681, 0.098174770424681, 0.024543692606170, 0.024543692606170, 0.098174770424681, 0.098174770424681, 0.098174770424681];
    self.NRings = [8, 64, 8, 8, 4, 4, 4];
    self.LayersRings = [
                        Layer.PreSamplerB,Layer.PreSamplerE,
                        Layer.EMB1,       Layer.EME1,
                        Layer.EMB2,       Layer.EME2,
                        Layer.EMB3,       Layer.EME3,
                        Layer.HEC0,       Layer.TileBar0, Layer.TileGap2, Layer.TileExt0,
                        Layer.HEC1,       Layer.HEC2,     Layer.TileBar1, Layer.TileGap0, Layer.TileExt1,
                        Layer.HEC3,       Layer.TileBar2, Layer.TileGap1, Layer.TileExt2
                        ];
    self.NLayersRings = [2, 2, 2, 2, 4, 5, 4];
    self.NMaxCells = [320, 512, 272, 128, 128, 128, 128];


class RingerFexEndCapConfig( RingerFex ):
  __slots__ = []
  def __init__(self, name = "RingerFexEndCapConfig"):
    super(RingerFexEndCapConfig, self).__init__(name)

    self.HltFeature = "HLT_TrigT2CaloEgamma"
    self.Feature = "TrigT2CaloEgamma"
    self.EtaBins = [1.54, 2.5] # bin pairs: min < eta <= max
    self.GlobalCenter = False;
    self.EtaSearchWindowSize = 0.1;
    self.PhiSearchWindowSize = 0.1;
    self.HistLabel = 'EndCap';

    from TrigT2CaloEgamma.RingerConstants import Layer

    # FIXME The EM1 from 1.8->2.5 granularity is 0.025/4=0.006250. However, from 1.57 to
    # 1.8 it should be 0.025/6
    # FIXME The HAD3 has segmentation of 0.1 x 0.1 from 1.8->2.5, whereas this is also not true from 1.57 to 1.8, being from 0.2 x 0.1
    self.DEtaRings = [0.0, 0.006250, 0.025, 0.05, 0.1, 0.1, 0.1];
    self.DPhiRings = [0.0,  0.098174770424681, 0.024543692606170, 0.024543692606170, 0.098174770424681, 0.098174770424681, 0.098174770424681];
    self.NRings = [0, 32, 8, 8, 4, 4, 4];
    self.LayersRings = [
                        #Layer.PreSamplerB,Layer.PreSamplerE,
                        Layer.EMB1,       Layer.EME1,
                        Layer.EMB2,       Layer.EME2,
                        Layer.EMB3,       Layer.EME3,
                        Layer.HEC0,       Layer.TileBar0, Layer.TileGap2, Layer.TileExt0,
                        Layer.HEC1,       Layer.HEC2,     Layer.TileBar1, Layer.TileGap0, Layer.TileExt1,
                        Layer.HEC3,       Layer.TileBar2, Layer.TileGap1, Layer.TileExt2
                        ];
    self.NLayersRings = [0, 2, 2, 2, 4, 5, 4];
    self.NMaxCells = [0, 272, 128, 128, 128, 128];


class T2CaloEgamma_Ringer (T2CaloEgamma_eGamma):
   __slots__ = []
   def __init__ (self, name="T2CaloEgamma_Ringer"):
       super(T2CaloEgamma_Ringer, self).__init__(name)
       # here put your customizations
       self.IAlgToolList += [RingerFexConfig()]
       self.TimerNtuple="T2CaloEgamma.T2CaEgtTotRinger"
       self.AthenaMonTools += [TrigT2CaloEgammaRingerTimeMonitoring()]


class T2CaloEgamma_FastAlgo (T2CaloEgammaFastAlgo):
   __slots__ = []
   def __init__ (self, name="T2CaloEgamma_FastAlgo"):
       super(T2CaloEgamma_FastAlgo, self).__init__(name)
       # here put your customizations
       from AthenaCommon.AppMgr import ToolSvc

       samp2 = EgammaSamp2FexNoTimerConfig(name="FaAlgoSamp2FexConfig")
       ToolSvc+=samp2
       samp1 = EgammaSamp1FexNoTimerConfig("FaAlgoSamp1FexConfig")
       ToolSvc+=samp1
       sampe = EgammaEmEnFexNoTimerConfig("FaAlgoEmEnFexConfig")
       ToolSvc+=sampe
       samph = EgammaHadEnFexNoTimerConfig("FaAlgoHadEnFexConfig")
       ToolSvc+=samph
       ring = RingerFexConfig("RingsMaker")
       ToolSvc+=ring

       ring.OutputLevel=DEBUG
       ring.RingsKey="CaloRings"
       self.IAlgToolList = [ samp2 ]
       self.IAlgToolList+= [ samp1 ]
       self.IAlgToolList+= [ sampe ]
       self.IAlgToolList+= [ samph ]
       self.IAlgToolList+= [ ring ] 

       self.EtaWidth = 0.2
       self.PhiWidth = 0.2
       #self.EtaWidthForID = 0.1
       #self.PhiWidthForID = 0.1
       #self.TrigEMClusterKey="TrigT2CaloEgamma"
       #t2catime.TimerHistLimits = [0,20]
       self.CalibListEndcap=[EgammaSshapeCalibrationEndcapConfig()]
       self.CalibListBarrel=[EgammaSshapeCalibrationBarrelConfig()]
       self.CalibListBarrel+=[EgammaHitsCalibrationBarrelConfig()]
       self.CalibListBarrel+=[EgammaGapCalibrationConfig()]
       self.CalibListBarrel+=[EgammaTransitionRegionsConfig()]
       self.CalibListEndcap+=[EgammaHitsCalibrationEndcapConfig()]
       self.CalibListEndcap+=[EgammaGapCalibrationConfig()]

class T2CaloEgamma_ReFastAlgo (T2CaloEgammaReFastAlgo):
   __slots__ = []
   def __init__ (self, name="T2CaloEgamma_ReFastAlgo"):
       super(T2CaloEgamma_ReFastAlgo, self).__init__(name)
       # here put your customizations
       from AthenaCommon.AppMgr import ToolSvc
       from AthenaCommon.AppMgr import ServiceMgr as svcMgr
       from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigCaloDataAccessSvc
       svcMgr += TrigCaloDataAccessSvc()
       samp2 = EgammaReSamp2FexNoTimerConfig(name="ReFaAlgoSamp2FexConfig")
       samp2.trigDataAccessMT=svcMgr.TrigCaloDataAccessSvc
       samp2.ExtraInputs+=[( 'LArOnOffIdMapping' , 'ConditionStore+LArOnOffIdMap' )]
       ToolSvc+=samp2
       samp1 = EgammaReSamp1FexNoTimerConfig("ReFaAlgoSamp1FexConfig")
       samp1.trigDataAccessMT=svcMgr.TrigCaloDataAccessSvc
       samp1.ExtraInputs+=[( 'LArOnOffIdMapping' , 'ConditionStore+LArOnOffIdMap' )]
       ToolSvc+=samp1
       sampe = EgammaReEmEnFexNoTimerConfig("ReFaAlgoEmEnFexConfig")
       sampe.trigDataAccessMT=svcMgr.TrigCaloDataAccessSvc
       sampe.ExtraInputs+=[( 'LArOnOffIdMapping' , 'ConditionStore+LArOnOffIdMap' )]
       ToolSvc+=sampe
       samph = EgammaReHadEnFexNoTimerConfig("ReFaAlgoHadEnFexConfig")
       # temporary fix for Tile
       samph.ExtraInputs=[('TileEMScale','ConditionStore+TileEMScale'),('TileBadChannels','ConditionStore+TileBadChannels')]
       samph.ExtraInputs+=[( 'LArOnOffIdMapping' , 'ConditionStore+LArOnOffIdMap' )]
       samph.trigDataAccessMT=svcMgr.TrigCaloDataAccessSvc
       ToolSvc+=samph
       #ToolSvc+=RingerFexConfig("RingsMaker") 
       #ToolSvc.RingsMaker.OutputLevel=DEBUG
       #ToolSvc.RingsMaker.RingsKey="CaloRings"
       self.IReAlgToolList = [ samp2 ]
       self.IReAlgToolList+= [ samp1 ]
       self.IReAlgToolList+= [ sampe ]
       self.IReAlgToolList+= [ samph ]
       #self.IReAlgToolList+= [ ToolSvc.RingsMaker ] 

       self.EtaWidth = 0.2
       self.PhiWidth = 0.2
       #self.EtaWidthForID = 0.1
       #self.PhiWidthForID = 0.1
       #self.TrigEMClusterKey="TrigT2CaloEgamma"
       #t2catime.TimerHistLimits = [0,20]
       self.CalibListEndcap=[EgammaSshapeCalibrationEndcapConfig()]
       self.CalibListBarrel=[EgammaSshapeCalibrationBarrelConfig()]
       self.CalibListBarrel+=[EgammaHitsCalibrationBarrelConfig()]
       self.CalibListBarrel+=[EgammaGapCalibrationConfig()]
       self.CalibListBarrel+=[EgammaTransitionRegionsConfig()]
       self.CalibListEndcap+=[EgammaHitsCalibrationEndcapConfig()]
       self.CalibListEndcap+=[EgammaGapCalibrationConfig()]
