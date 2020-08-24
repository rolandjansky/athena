# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__TriggerTowerMaker

class TriggerTowerMakerBase (LVL1__TriggerTowerMaker):
    __slots__ = []
    def __init__(self, name):
        super( TriggerTowerMakerBase, self ).__init__( name )
        from AthenaCommon.DetFlags import DetFlags
        from AthenaCommon.Constants import WARNING
        if DetFlags.digitize.LVL1_on():
            from Digitization.DigitizationFlags import jobproperties
            self.RndmSvc=jobproperties.Digitization.rndmSvc.get_Value()
            self.PedEngine = "%s_Pedestal"%name
            self.DigiEngine =  "%s_Digitization"%name
            jobproperties.Digitization.rndmSeedList.addSeed( str(self.PedEngine), 8594832, 5736213 )
            jobproperties.Digitization.rndmSeedList.addSeed( str(self.DigiEngine), 8631309, 4492432) 
        else:
            self.RndmSvc = 'AtRanluxGenSvc'
            self.PedEngine = "%s_Pedestal"%name
            self.DigiEngine =  "%s_Digitization"%name
          
            from AthenaCommon.AppMgr import ServiceMgr
            if not hasattr( ServiceMgr, 'AtRanluxGenSvc'):
                from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
                ServiceMgr += AtRanluxGenSvc()
               
            # init random number seeds
            ServiceMgr.AtRanluxGenSvc.Seeds += [ str(self.PedEngine) + " 8594832 5736213" ]
            ServiceMgr.AtRanluxGenSvc.Seeds += [ str(self.DigiEngine) + " 8631309 4492432" ]
            ServiceMgr.AtRanluxGenSvc.OutputLevel = WARNING # suppress 1 per event INFO messages.
    
class TriggerTowerMaker_TTL1(TriggerTowerMakerBase) :
  __slots__ = []
  def __init__(self, name = "TriggerTowerMaker_TTL1"):
      super(TriggerTowerMaker_TTL1, self).__init__(name)
      self.CellType = 3
      self.PedestalValue = 32
      self.TowerNoise = False
  
class TriggerTowerMaker_TTL1_Rel13(TriggerTowerMakerBase) :
  __slots__ = []
  def __init__(self, name = "TriggerTowerMaker_TTL1_Rel13"):
      super(TriggerTowerMaker_TTL1_Rel13, self).__init__(name)
      self.CellType = 3
      self.PedestalValue = 32
      self.TowerNoise = False
      self.EmReg1Calib = [1.13,1.1,1.1,1.1,1.1,1.1,1.1,1.13,1.16,1.16,1.14,1.14,1.14,1.14,1.45,1.23,1.23,1.1,1.1,1.16,1.1,1.1,1.15,1.1,1.1]
      self.EmReg2Calib = [ 1.13 , 1.13 , 1.13 ]
      self.EmReg3Calib =[ 1.25 ]
      self.EmFwdCalib =[ 1.2 , 1.13 , 1.13 , 1.13 ]
      self.HadReg1Calib = [1.3,1.3,1.3,1.3,1.3,1.45,1.45,1.45,1.45,1.45,1.45,1.45,1.45,1.45,1.45,1.45,1.45,1.45,1.45,1.45,1.45,1.45,1.45,1.45,1.45]
      self.HadReg2Calib = [1.3,1.3,1.3]
      self.HadReg3Calib = [ 1.3 ]
      self.HadFwdCalib=  [ 1.3 , 1.3 , 1.3 , 1.3 ] 

  
class TriggerTowerMaker_TTL1_Rel11(TriggerTowerMakerBase) :
  __slots__ = []
  def __init__(self, name = "TriggerTowerMaker_TTL1_Rel11"):
      super(TriggerTowerMaker_TTL1_Rel11, self).__init__(name)
      self.CellType = 3
      self.PedestalValue = 32
      self.TowerNoise = False
      self.EmReg1Calib = [1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1]
      self.EmReg2Calib = [ 1.1 , 1.1 , 1.1 ]
      self.EmReg3Calib =[ 1.1 ]
      self.EmFwdCalib =[ 1.8 , 1.8 , 1.5 , 0.8 ]
      self.HadReg1Calib = [1.2,1.2,1.2,1.2,1.2,1.2,1.2,1.2,1.2,1.7,1.7,1.7,1.7,1.7,1.7,0.7,0.7,0.7,0.7,0.7,0.7,0.7,0.7,0.7,0.7]
      self.HadReg2Calib = [0.6,0.6,0.6]
      self.HadReg3Calib = [ 0.6 ]
      self.HadFwdCalib=  [ 1. , 1. , 1. , 1. ] 

class TriggerTowerMaker_Cell(TriggerTowerMakerBase) :
  __slots__ = []
  def __init__(self, name = "TriggerTowerMaker_Cell"):
      super(TriggerTowerMaker_Cell, self).__init__(name)
      self.CellType = 1
      self.PedestalValue = 32
      self.EMTowerThreshold = 3072
      self.HadTowerThreshold = 3072
      self.TowerNoise = True
      self.NumberOfEmNoiseRegions = 4
      self.EmNoiseEtaLower = [0.,1.,1.5,3.2]
      self.EmNoiseRMS = [90.,550.,650.,6500.]
      self.NumberOfHadNoiseRegions = 3
      self.HadNoiseEtaLower = [0.,1.5,3.2]
      self.HadNoiseRMS = [350.,50.,6500.]
      self.EmReg1Calib = [1.132 , 1.124 , 1.119 , 1.107 , 1.115 , 1.109 , 1.072 , 1.091 , 1.133 , 1.132 , 1.127, 1.111 , 1.107 , 1.114 , 1.285 , 1.004 , 1.153 , 1.112 , 1.116 , 1.17 , 1.136 , 1.119 , 1.157 , 1.134 , 1.151 ]
      self.EmReg2Calib = [1.146 , 1.167 , 1.154 ]
      self.EmReg3Calib = [1.202]
      self.EmFwdCalib = [0.25, 1.55, 2.85, 3.35]
      self.HadReg1Calib = [1.408 , 1.422 , 1.389 , 1.377 , 1.428 , 1.627 , 1.792 , 1.482 , 1.682 , 1.612 , 1.674, 1.674 , 1.812 , 1.682 , 1.792 , 1.529 , 1.000 , 1.000 , 1.383 , 1.608 , 1.430 , 1.282 , 1.464 , 1.383 , 1.324 ]
      self.HadReg2Calib = [1.184 , 1.023 , 0.713 ]
      self.HadReg3Calib = [1.57]
      self.HadFwdCalib = [1.57 , 1.618 , 1.618 , 1.330]


class TriggerTowerMaker_Reprocess(TriggerTowerMakerBase) :
  __slots__ = []
  def __init__(self, name = "TriggerTowerMaker_Reprocess"):
      super(TriggerTowerMaker_Reprocess, self).__init__(name)
      self.CellType = 2
      self.TowerNoise = False
      self.TriggerTowerLocation="ReprocessedTriggerTowers"

class TriggerTowerMaker_TTL1_Gain1(TriggerTowerMakerBase) :
  __slots__ = []
  def __init__(self, name = "TriggerTowerMaker_TTL1_Gain1"):
      super(TriggerTowerMaker_TTL1_Gain1, self).__init__(name)
      self.CellType = 3
      self.PedestalValue = 32
      self.TowerNoise = False
      self.EmReg1Calib = [1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.]
      self.EmReg2Calib = [ 1. , 1. , 1. ]
      self.EmReg3Calib =[ 1. ]
      self.EmFwdCalib =[ 1. , 1. , 1. , 1. ]
      self.HadReg1Calib = [1.001,1.011,1.031,1.062,1.103,1.155,1.219,1.295,1.384,1.486,1.604,1.737,1.888,2.058,2.249,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.]
      self.HadReg2Calib = [1.,1.,1.]
      self.HadReg3Calib = [ 1. ]
      self.HadFwdCalib=  [ 1. , 1. , 1. , 1. ]

def configureTriggerTowerMaker(useAutoCorr,
                               usePedestalCorrection,
                               useNoiseCuts,
                               usePedestalCorrectionProviderROOT,
                               inputFileRoot = 'l1calofir-mu60.root',
                               inputFileTxtEM = 'DynamicPedestalCorrectionEM.txt',
                               inputFileTxtHAD = 'DynamicPedestalCorrectionHAD.txt',
                               useNoiseCuts2012 = False):
    """ This function configures the TriggerTowerMaker Algorithm.

useAutoCorr: True = use autocorrelation filters; False = use matched filters
usePedestalCorrection: ...
useNoiseCuts: ...
usePedestalCorrectionProviderROOT: obtain pedestal correction data from ROOT file (True)
                                   or text file (False)
inputFile*: name of the input files for the pedestal correction provider
useNoiseCuts2012: use 2012 values of noise cuts (only available for matched filters)
"""
    from AthenaCommon.Logging import logging  # loads logger
    log = logging.getLogger( "TrigT1CaloSimConfig.py" )
    log.info("configureTriggerTowerMaker will override the TriggerTowerMaker configuration")
    log.info("AutoCorrelation = %s", useAutoCorr)
    log.info("PedestalCorrection = %s", usePedestalCorrection)
    log.info("NoiseCuts = %s", useNoiseCuts)
    log.info("PedestalCorrection from ROOT-File = %s", usePedestalCorrectionProviderROOT)
    if usePedestalCorrection:
        if usePedestalCorrectionProviderROOT:
            log.info("ROOT input file = %s", inputFileRoot)
        else:
            log.info("EM input file = %s", inputFileTxtEM)
            log.info("HAD input file = %s", inputFileTxtHAD)
    log.info("NoiseCuts2012 = %s", useNoiseCuts2012)
    if useAutoCorr and useNoiseCuts2012:
        log.warning("useNoiseCuts2012: 2012 NoiseCuts only available for matched filters - setting will have not effect")
    if (not useAutoCorr) and useNoiseCuts and useNoiseCuts2012:
        log.warning("both useNoiseCuts and useNoiseCuts2012 set - the latter will override the former")

    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

    if not hasattr(job, 'TriggerTowerMaker'):
        log.warning('configureTriggerTowerMaker called without TriggerTowerMaker in the AlgSequence! Not configuring anything.')
        return
    
    if useAutoCorr: # auto-correlation filter
        job.TriggerTowerMaker.PulseElements = True
        # Em 
        job.TriggerTowerMaker.FilterCoeffsEmElement0=[-2,0,14,3,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement1=[-2,0,14,3,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement2=[-2,-1,15,3,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement3=[-2,-1,15,3,-4]
        job.TriggerTowerMaker.FilterCoeffsEmElement4=[-2,-1,15,3,-4]
        job.TriggerTowerMaker.FilterCoeffsEmElement5=[-2,-1,15,3,-4]
        job.TriggerTowerMaker.FilterCoeffsEmElement6=[-2,-1,15,3,-4]
        job.TriggerTowerMaker.FilterCoeffsEmElement7=[-2,0,14,3,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement8=[-1,0,7,2,-2]
        job.TriggerTowerMaker.FilterCoeffsEmElement9=[-1,0,7,2,-2]
        job.TriggerTowerMaker.FilterCoeffsEmElement10=[-2,0,14,3,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement11=[-2,0,14,3,-4]
        job.TriggerTowerMaker.FilterCoeffsEmElement12=[-3,-1,15,3,-4]
        job.TriggerTowerMaker.FilterCoeffsEmElement13=[-3,-1,15,3,-5]
        job.TriggerTowerMaker.FilterCoeffsEmElement14=[-1,0,7,2,-2] #15
        job.TriggerTowerMaker.FilterCoeffsEmElement15=[-2,-1,8,1,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement16=[-2,-1,8,1,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement17=[-2,-1,8,1,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement18=[-2,-1,8,1,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement19=[-2,-1,8,1,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement20=[-2,-1,8,1,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement21=[-2,-1,8,1,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement22=[-2,-1,8,1,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement23=[-2,-1,8,1,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement24=[-2,-1,8,1,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement25=[-1,-5,12,-2,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement26=[-1,-5,12,-2,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement27=[-1,-5,12,-2,-3]
        job.TriggerTowerMaker.FilterCoeffsEmElement28=[-2,-3,10,0,-4] # 14
        job.TriggerTowerMaker.FilterCoeffsEmElement29=[1,-2,8,1,0]
        job.TriggerTowerMaker.FilterCoeffsEmElement30=[3,-6,9,-2,0]
        job.TriggerTowerMaker.FilterCoeffsEmElement31=[3,-7,10,-3,0]
        job.TriggerTowerMaker.FilterCoeffsEmElement32=[3,-7,10,-3,0] # 4  
        # Had
        job.TriggerTowerMaker.FilterCoeffsHadElement0=[-2,4,10,5,0]
        job.TriggerTowerMaker.FilterCoeffsHadElement1=[-2,4,10,5,1]
        job.TriggerTowerMaker.FilterCoeffsHadElement2=[-2,4,10,5,0]
        job.TriggerTowerMaker.FilterCoeffsHadElement3=[-2,4,10,5,0]
        job.TriggerTowerMaker.FilterCoeffsHadElement4=[-2,4,10,5,0]
        job.TriggerTowerMaker.FilterCoeffsHadElement5=[-2,4,10,5,0]
        job.TriggerTowerMaker.FilterCoeffsHadElement6=[-3,4,10,5,0]
        job.TriggerTowerMaker.FilterCoeffsHadElement7=[-3,4,10,5,0]
        job.TriggerTowerMaker.FilterCoeffsHadElement8=[-3,4,10,5,0]
        job.TriggerTowerMaker.FilterCoeffsHadElement9=[-3,4,10,5,0]
        job.TriggerTowerMaker.FilterCoeffsHadElement10=[-3,4,10,5,0]
        job.TriggerTowerMaker.FilterCoeffsHadElement11=[-3,4,10,5,-1]
        job.TriggerTowerMaker.FilterCoeffsHadElement12=[-2,2,5,2,0]
        job.TriggerTowerMaker.FilterCoeffsHadElement13=[-2,2,5,2,0]
        job.TriggerTowerMaker.FilterCoeffsHadElement14=[-2,2,5,2,0] # 15
        job.TriggerTowerMaker.FilterCoeffsHadElement15=[-7,3,10,5,-1]
        job.TriggerTowerMaker.FilterCoeffsHadElement16=[-2,1,3,1,-1]
        job.TriggerTowerMaker.FilterCoeffsHadElement17=[-2,1,3,1,-1]
        job.TriggerTowerMaker.FilterCoeffsHadElement18=[-2,1,3,1,-1]
        job.TriggerTowerMaker.FilterCoeffsHadElement19=[-2,1,3,1,-1]
        job.TriggerTowerMaker.FilterCoeffsHadElement20=[-2,1,3,1,-1]
        job.TriggerTowerMaker.FilterCoeffsHadElement21=[-2,1,3,1,-1]
        job.TriggerTowerMaker.FilterCoeffsHadElement22=[-2,1,3,1,-1]
        job.TriggerTowerMaker.FilterCoeffsHadElement23=[-2,1,3,1,-1]
        job.TriggerTowerMaker.FilterCoeffsHadElement24=[-2,1,3,1,-1]
        job.TriggerTowerMaker.FilterCoeffsHadElement25=[-10,4,13,2,-4]
        job.TriggerTowerMaker.FilterCoeffsHadElement26=[-11,4,14,2,-5]
        job.TriggerTowerMaker.FilterCoeffsHadElement27=[-2,1,3,1,-1]
        job.TriggerTowerMaker.FilterCoeffsHadElement28=[-2,1,3,1,-1] # 14
        job.TriggerTowerMaker.FilterCoeffsHadElement29=[1,-4,8,1,0]      # FCal2-1
        job.TriggerTowerMaker.FilterCoeffsHadElement30=[3,-10,12,-3,0]   # FCal2-2
        job.TriggerTowerMaker.FilterCoeffsHadElement31=[-2,-1,13,5,0]     # FCal3-1
        job.TriggerTowerMaker.FilterCoeffsHadElement32=[0,-6,9,1,0] #4   # FCal3-2

        if useNoiseCuts:
            # Em
            job.TriggerTowerMaker.EmReg1Thresh=[6182,6246,6506,6515,6536,6495,6449,5844,5959,5780,5476,5526,5677,5348,4807,5307,5248,5383,5316,5230,5219,5228,5122,4933,4633]
            job.TriggerTowerMaker.EmReg2Thresh=[9316,9034,8981]
            job.TriggerTowerMaker.EmReg3Thresh=[6087]
            job.TriggerTowerMaker.FCAL1Thresh=[4116,11645,28641,26263]
            # Had
            job.TriggerTowerMaker.HadReg1Thresh=[4929,4859,4944,4569,4566,4394,4203,4067,4534,3633,3497,3517,3744,3369,3126,2684,3515,3680,3726,3607,3511,3526,3491,3387,3281]
            job.TriggerTowerMaker.HadReg2Thresh=[5617,5527,5206]
            job.TriggerTowerMaker.HadReg3Thresh=[2941]
            job.TriggerTowerMaker.FCAL23Thresh=[5274,10229,3044,5329]

    else: # matched filter
        job.TriggerTowerMaker.PulseElements = True
        # Em
        job.TriggerTowerMaker.FilterCoeffsEmElement0=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement1=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement2=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement3=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement4=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement5=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement6=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement7=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement8=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement9=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement10=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement11=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement12=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement13=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement14=[1,4,9,5,1] #15
        job.TriggerTowerMaker.FilterCoeffsEmElement15=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement16=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement17=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement18=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement19=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement20=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement21=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement22=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement23=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement24=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement25=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement26=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement27=[1,4,9,5,1]
        job.TriggerTowerMaker.FilterCoeffsEmElement28=[1,4,9,5,1] # 14
        job.TriggerTowerMaker.FilterCoeffsEmElement29=[0,3,14,4,0]
        job.TriggerTowerMaker.FilterCoeffsEmElement30=[0,3,14,4,0]
        job.TriggerTowerMaker.FilterCoeffsEmElement31=[0,3,14,4,0]
        job.TriggerTowerMaker.FilterCoeffsEmElement32=[0,3,14,4,0] # 4
        # Had
        job.TriggerTowerMaker.FilterCoeffsHadElement0=[1,8,15,10,4]
        job.TriggerTowerMaker.FilterCoeffsHadElement1=[1,8,15,10,4]
        job.TriggerTowerMaker.FilterCoeffsHadElement2=[1,8,15,10,4]
        job.TriggerTowerMaker.FilterCoeffsHadElement3=[1,8,15,10,4]
        job.TriggerTowerMaker.FilterCoeffsHadElement4=[1,8,15,10,4]
        job.TriggerTowerMaker.FilterCoeffsHadElement5=[1,8,15,10,4]
        job.TriggerTowerMaker.FilterCoeffsHadElement6=[1,8,15,10,4]
        job.TriggerTowerMaker.FilterCoeffsHadElement7=[1,8,15,10,4]
        job.TriggerTowerMaker.FilterCoeffsHadElement8=[1,8,15,10,4]
        job.TriggerTowerMaker.FilterCoeffsHadElement9=[1,8,15,10,4]
        job.TriggerTowerMaker.FilterCoeffsHadElement10=[1,8,15,10,4]
        job.TriggerTowerMaker.FilterCoeffsHadElement11=[1,8,15,10,4]
        job.TriggerTowerMaker.FilterCoeffsHadElement12=[1,8,15,10,4]
        job.TriggerTowerMaker.FilterCoeffsHadElement13=[1,8,15,10,4]
        job.TriggerTowerMaker.FilterCoeffsHadElement14=[1,8,15,10,4] # 15
        job.TriggerTowerMaker.FilterCoeffsHadElement15=[0,9,15,11,6]
        job.TriggerTowerMaker.FilterCoeffsHadElement16=[0,9,15,11,6]
        job.TriggerTowerMaker.FilterCoeffsHadElement17=[0,9,15,11,6]
        job.TriggerTowerMaker.FilterCoeffsHadElement18=[0,9,15,11,6]
        job.TriggerTowerMaker.FilterCoeffsHadElement19=[0,9,15,11,6]
        job.TriggerTowerMaker.FilterCoeffsHadElement20=[0,9,15,11,6]
        job.TriggerTowerMaker.FilterCoeffsHadElement21=[0,9,15,11,6]
        job.TriggerTowerMaker.FilterCoeffsHadElement22=[0,9,15,11,6]
        job.TriggerTowerMaker.FilterCoeffsHadElement23=[0,9,15,11,6]
        job.TriggerTowerMaker.FilterCoeffsHadElement24=[0,9,15,11,6]
        job.TriggerTowerMaker.FilterCoeffsHadElement25=[0,9,15,11,6]
        job.TriggerTowerMaker.FilterCoeffsHadElement26=[0,9,15,11,6]
        job.TriggerTowerMaker.FilterCoeffsHadElement27=[0,9,15,11,6]
        job.TriggerTowerMaker.FilterCoeffsHadElement28=[0,9,15,11,6] # 14
        job.TriggerTowerMaker.FilterCoeffsHadElement29=[0,3,11,6,0]
        job.TriggerTowerMaker.FilterCoeffsHadElement30=[0,3,11,6,0]
        job.TriggerTowerMaker.FilterCoeffsHadElement31=[0,3,11,6,0]
        job.TriggerTowerMaker.FilterCoeffsHadElement32=[0,3,11,6,0] #4 

        if useNoiseCuts:
            job.TriggerTowerMaker.EmReg1Thresh=[5986,6080,6140,6095,6083,6127,6120,5696,5321,5197,5134,5208,5232,4645,4270,4322,4395,4805,4373,4414,4452,4636,4517,4335,4046]
            job.TriggerTowerMaker.EmReg2Thresh=[9767,9609,9837]
            job.TriggerTowerMaker.EmReg3Thresh=[5858]
            job.TriggerTowerMaker.FCAL1Thresh=[4379,14842,42753,40780]
            job.TriggerTowerMaker.HadReg1Thresh=[4611,4482,4653,4193,4199,4013,3738,3601,4266,3052,2927,3063,3481,2873,2616,2416,2942,3155,3272,3131,3017,3112,3106,2943,2826]
            job.TriggerTowerMaker.HadReg2Thresh=[6601,6960,6610]
            job.TriggerTowerMaker.HadReg3Thresh=[2688]
            job.TriggerTowerMaker.FCAL23Thresh=[5467,22738,2760,9010]
        
        if useNoiseCuts2012: # only available for matched filters
            job.TriggerTowerMaker.EmReg1Thresh=[4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000] 
            job.TriggerTowerMaker.EmReg2Thresh=[11400,11700,10800] 
            job.TriggerTowerMaker.EmReg3Thresh=[6100]
            job.TriggerTowerMaker.FCAL1Thresh=[4500,13500,36500,44600]
            job.TriggerTowerMaker.HadReg1Thresh=[4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000]
            job.TriggerTowerMaker.HadReg2Thresh=[4000,4000,4000]
            job.TriggerTowerMaker.HadReg3Thresh=[4000]
            job.TriggerTowerMaker.FCAL23Thresh=[10900,28700,4100,6600]

    if usePedestalCorrection:
        if not hasattr(ToolSvc, 'L1TriggerTowerTool'):
            log.warning('configureTriggerTowerMaker called without LVL1::L1TriggerTowerTool in ToolSvc! Not configuring dynamic pedestal correction.')
            return

        if usePedestalCorrectionProviderROOT:
            if not hasattr(ToolSvc, 'L1DynamicPedestalProviderRoot'):
                from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1DynamicPedestalProviderRoot
                pedestalProvider = LVL1__L1DynamicPedestalProviderRoot('L1DynamicPedestalProviderRoot',
                                                                       FirRefFile=inputFileRoot)
                ToolSvc += pedestalProvider
            else:
                pedestalProvider = ToolSvc.L1DynamicPedestalProviderRoot
        else:

            if 'MetaReaderPeeker' not in dir():
                try:
                    from PyUtils.MetaReaderPeekerFull import metadata
                except ImportError:
                    log.warning('configureTriggerTowerMaker could not retrieve input file summary. Will not configure dynamic pedestal correction from text.')
                    return

            if not hasattr(ToolSvc, 'L1DynamicPedestalProviderTxt'):
                from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1DynamicPedestalProviderTxt
                pedestalProvider = LVL1__L1DynamicPedestalProviderTxt('L1DynamicPedestalProviderTxt',
                                                                      InputFileEM=inputFileTxtEM,
                                                                      InputFileHAD=inputFileTxtHAD)
                pedestalProvider.BeamIntensityPattern = metadata['/Digitization/Parameters']['BeamIntensityPattern']
                ToolSvc += pedestalProvider
            else:
                pedestalProvider = ToolSvc.L1DynamicPedestalProviderTxt

        job.TriggerTowerMaker.BaselineCorrection = True
        ToolSvc.L1TriggerTowerTool.BaselineCorrection = True
        ToolSvc.L1TriggerTowerTool.L1DynamicPedestalProvider=pedestalProvider
        if not hasattr(ToolSvc, "LumiBlockMuTool"):
            from LumiBlockComps.LumiBlockCompsConf import LumiBlockMuTool
            ToolSvc += LumiBlockMuTool("LumiBlockMuTool")
            ToolSvc.L1TriggerTowerTool.LumiBlockMuTool = ToolSvc.LumiBlockMuTool
