# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Monitoring classes are loaded for each algorithm

from AthenaCommon.Logging import logging
from TrigT2CaloJet.TrigT2CaloJetConf import T2CaloJetGridFromCells
from TrigT2CaloJet.TrigT2CaloJetConf import T2CaloJetGridFromFEBHeader
from TrigT2CaloJet.TrigT2CaloJetConf import T2CaloJetConeTool
from TrigT2CaloJet.TrigT2CaloJetConf import T2CaloJetCalibTool
from TrigT2CaloJet.TrigT2CaloJetConf import T2L1CaloJet
from TrigT2CaloJet.TrigT2CaloJetConf import T2L1CaloFullScanJet
from TrigT2CaloJet.TrigT2CaloJetConf import T2L1CaloJetFullScanFastJetTool
from TrigT2CaloJet.TrigT2CaloJetConf import T2L1Unpacking
from TrigT2CaloJet.TrigT2CaloJetConf import T2AllRoiUnpacking
from TrigT2CaloJet.TrigT2CaloJetConf import T2CaloFastJet
from TrigT2CaloJet.TrigT2CaloJetConf import T2CaloJet
from TrigT2CaloJet.TrigT2CaloJetConf import T2L1TowerCalibTool
from TrigT2CaloJet.TrigT2CaloJetConf import T2L1CaloJetCalibTool

import TrigT2CaloJet.CalibrationFactorsL15 as CalibrationFactorsL15

from TrigT2CaloCalibration.TrigT2CaloCalibrationConf import T2SampCalibTool
from TrigT2CaloCalibration.TrigT2CaloCalibrationConf import T2JESCalibTool
from TrigT2CaloCalibration.TrigT2CaloCalibrationConf import T2GSCalibTool

#import JetCalibTools.EtaMassEnergyFactors        as EtaMassEnergyFactors
#import JetCalibTools.Tile1FractionConstants      as Tile1FractionConstants
#import JetCalibTools.HEC0FractionConstants       as HEC0FractionConstants
#import JetCalibTools.EM3FractionConstants        as EM3FractionConstants
#import JetCalibTools.FCAL1FractionConstants      as FCAL1FractionConstants
#import JetCalibTools.PresamplerFractionConstants as PresamplerFractionConstants 

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO
from AthenaPython import PyAthena # for L1.5 enums

class  T2AllRoiUnpacking_test(T2AllRoiUnpacking):
    __slots__ = []
    def __init__(self, name="T2AllRoiUnpacking_test", noisecut=2.):
        super( T2AllRoiUnpacking_test, self ).__init__(name)        
        from TrigT2CaloJet.TrigT2CaloJetMonitoring import T2AllRoiUnpackingTimeOnlineMonitoring, T2AllRoiUnpackingTimeValidationMonitoring
        online = T2AllRoiUnpackingTimeOnlineMonitoring()
        validation = T2AllRoiUnpackingTimeValidationMonitoring()
        self.AthenaMonTools = [online, validation, ]
        self.doTiming=True
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += T2CaloJetGridFromCells()
        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theNoiseTool = CaloNoiseToolDefault()
        ToolSvc += theNoiseTool
        # add the tool used to get & fill the calorimeter cells
        t2calojetgridfromcellsTool = T2CaloJetGridFromCells()
        t2calojetgridfromcellsTool.doTiming = True
        t2calojetgridfromcellsTool.doHECcellQualityCleaning = False
        t2calojetgridfromcellsTool.applyNoiseCut = True  # gerbaudo@cern.ch: should I set also the threshold? (default is 2.0)
        t2calojetgridfromcellsTool.noiseTool = theNoiseTool
        t2calojetgridfromcellsTool.applyNoiseCut = True
        t2calojetgridfromcellsTool.noiseCutValue = noisecut

        self.GridFromCellsTool = t2calojetgridfromcellsTool
        # here put your customizations
        self.L2RoiEtaHalfWidth = 0.5
        self.L2RoiPhiHalfWidth = 0.5
        #self.MergeEmCells=False #default True

## L1.5 unpackers
class T2L1Unpacking_Base (T2L1Unpacking):
    __slots__ = []
    def __init__(self, name):
        super( T2L1Unpacking_Base, self ).__init__(name)
        
        from TrigT2CaloJet.TrigT2CaloJetMonitoring import TrigT2L1UnpackingValidationMonitoring, TrigT2L1UnpackingOnlineMonitoring, TrigT2L1UnpackingCosmicMonitoring
        validation = TrigT2L1UnpackingValidationMonitoring()
        online = TrigT2L1UnpackingOnlineMonitoring()
        cosmic = TrigT2L1UnpackingCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("T2L1Unpacking_Time")
        time.TimerHistLimits = [0,20]
        self.doTiming=True

        self.AthenaMonTools = [ validation, online, time, cosmic]
        
class T2L1Unpacking_TT (T2L1Unpacking_Base):
    __slots__ = []
    def __init__(self, name="T2L1Unpacking_TT"):
        super( T2L1Unpacking_TT, self ).__init__(name)
        # here put your customizations
        self.doTriggerTowers = True

class T2L1Unpacking_TT_TowerCalib (T2L1Unpacking_Base):
    __slots__ = []
    def __init__(self, name="T2L1Unpacking_TT_TowerCalib"):
        super( T2L1Unpacking_TT_TowerCalib, self ).__init__(name)      
        # here put your customizations
        self.doTriggerTowers = True        
        # add in the calibration tool
        t2l1towercalibtool = T2L1TowerCalibTool()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += T2L1TowerCalibTool()
        energyCorr = [ [ ] ]
        energyCorr = CalibrationFactorsL15.energyCorrDict['TT_factors']
        t2l1towercalibtool.Calib_Factors = str(energyCorr)
        self.T2JetTools = [t2l1towercalibtool]        

class T2L1Unpacking_JE (T2L1Unpacking_Base):
    __slots__ = []
    def __init__(self, name="T2L1Unpacking_JE"):
        super( T2L1Unpacking_JE, self ).__init__(name)
        # here put your customizations
        self.doTriggerTowers = False
        
class T2L1Unpacking_JE_TowerCalib (T2L1Unpacking_Base):
    __slots__ = []
    def __init__(self, name="T2L1Unpacking_JE_TowerCalib"):
        super( T2L1Unpacking_JE_TowerCalib, self ).__init__(name)
        # here put your customizations
        self.doTriggerTowers = False  
        # add in the calibration tool
        t2l1towercalibtool = T2L1TowerCalibTool()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += T2L1TowerCalibTool()
        energyCorr = [ [ ] ]
        energyCorr = CalibrationFactorsL15.energyCorrDict['JE_factors']
        t2l1towercalibtool.Calib_Factors = str(energyCorr)
        self.T2JetTools = [t2l1towercalibtool]        

class T2CaloFastJet_CC (T2CaloFastJet):
    "T2CaloFastJet FEX to be used with calorimeter cells"
    __slots__ = []
    def __init__(self, name="T2CaloFastJet_CC"):
        super( T2CaloFastJet_CC, self ).__init__(name)
        from TrigT2CaloJet.TrigT2CaloJetMonitoring import TrigT2CaloFastJetValidationMonitoring, TrigT2CaloFastJetOnlineMonitoring, TrigT2CaloFastJetCosmicMonitoring
        validation = TrigT2CaloFastJetValidationMonitoring()
        online = TrigT2CaloFastJetOnlineMonitoring()
        cosmic = TrigT2CaloFastJetCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("T2CaloFastJet_CC_Time")
        time.TimerHistLimits = [0,20]
        self.doTiming=True

        self.AthenaMonTools = [ validation, online, time, cosmic]

        # here put your customizations
        self.doCleaning = True
        self.doT2L1Cleaning = False # do the regular cell-based cleaning
        self.outputType = PyAthena.T2L1Tools.A4CC

class T2CaloFastJet_CC_JESCalib (T2CaloFastJet_CC):
    __slots__ = []
    def __init__(self, name="T2CaloFastJet_CC_JESCalib"):
        super( T2CaloFastJet_CC_JESCalib, self ).__init__(name)
        # here put your customizations
        self.outputType = PyAthena.T2L1Tools.A4CC_JES
        # add in the calibration tool
        t2jescalibtool = T2JESCalibTool("T2JESCalibTool_Jet")
        t2jescalibtool.MinEtCut = 1 
        # Jet calibration factors
        energyCorr = [ [ ] ]
#        energyCorr = EtaMassEnergyFactors.energyCorrDict['AntiKt4TopoJets_EM'] # as a test, eventually we'll have our own constants
        t2jescalibtool.JES_Factors = str(energyCorr)
        t2calojetcalibtool = T2CaloJetCalibTool()
        t2calojetcalibtool.T2JetCalibTool = t2jescalibtool
        t2calojetcalibtool.doJetCalib = True
        t2calojetcalibtool.T2JetCorrToolTile1  = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolFcal1  = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolEm3Bar = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolHec0   = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolEm3Fwd = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolPsBar  = t2jescalibtool #dummy config        
        self.T2JetTools = [t2calojetcalibtool]

class T2CaloFastJet_CC_JESCalib_L15 (T2CaloFastJet_CC_JESCalib):
    __slots__ = []
    def __init__(self, name="T2CaloFastJet_CC_JESCalib_L15"):
        super( T2CaloFastJet_CC_JESCalib_L15, self ).__init__(name)
        # here put your customizations
        self.inputType = PyAthena.T2L1Tools.A4TT
class T2CaloFastJet_CC_JESCalib_L15had (T2CaloFastJet_CC_JESCalib):
    __slots__ = []
    def __init__(self, name="T2CaloFastJet_CC_JESCalib_L15had"):
        super( T2CaloFastJet_CC_JESCalib_L15had, self ).__init__(name)
        # here put your customizations
        self.inputType = PyAthena.T2L1Tools.A4TT_JES

        
## L1.5 FastJet FEXes
class T2CaloFastJet_Base (T2CaloFastJet):
    __slots__ = []
    def __init__(self, name):
        super( T2CaloFastJet_Base, self ).__init__(name)
        from TrigT2CaloJet.TrigT2CaloJetMonitoring import TrigT2CaloFastJetValidationMonitoring, TrigT2CaloFastJetOnlineMonitoring, TrigT2CaloFastJetCosmicMonitoring
        validation = TrigT2CaloFastJetValidationMonitoring()
        online = TrigT2CaloFastJetOnlineMonitoring()
        cosmic = TrigT2CaloFastJetCosmicMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("T2CaloFastJet_Time")
        time.TimerHistLimits = [0,20]
        self.doTiming=True
        
        self.AthenaMonTools = [ validation, online, time, cosmic]

###############
# All non-multiple output TE algorithms should not be used with current L1.5 L2 interface
###############
#class T2CaloFastJet_a4JE (T2CaloFastJet_Base):
    #__slots__ = []
    #def __init__(self, name="T2CaloFastJet_a4JE"):
        #super( T2CaloFastJet_a4JE, self ).__init__(name)
        ## here put your customizations
        #self.doCleaning = True
        #self.doT2L1Cleaning = True
        #self.outputType = PyAthena.T2L1Tools.A4JE
        
#class T2CaloFastJet_a4JE_TowerCalib (T2CaloFastJet_Base):
    #__slots__ = []
    #def __init__(self, name="T2CaloFastJet_a4JE_TowerCalib"):
        #super( T2CaloFastJet_a4JE_TowerCalib, self ).__init__(name)
        ## here put your customizations
        #self.doCleaning = True
        #self.doT2L1Cleaning = True
        #self.inputType  = PyAthena.T2L1Tools.CALIBRATED
        #self.outputType = PyAthena.T2L1Tools.A4JE_TC
        
        
#class T2CaloFastJet_a4JE_JESCalib (T2CaloFastJet_Base):
    #__slots__ = []
    #def __init__(self, name="T2CaloFastJet_a4JE_JESCalib"):
        #super( T2CaloFastJet_a4JE_JESCalib, self ).__init__(name)
        ## here put your customizations
        #self.doCleaning = True
        #self.doT2L1Cleaning = True
        #self.outputType = PyAthena.T2L1Tools.A4JE_JES
        
        ## add in the calibration tool
        #t2jescalibtool = T2JESCalibTool("T2JESCalibTool_Jet")
        #t2jescalibtool.MinEtCut = 1 
        ## Jet calibration factors
        #energyCorr = [ [ ] ]
        #energyCorr = EtaMassEnergyFactors.energyCorrDict['AntiKt4TopoJets_EM'] # as a test, eventually we'll have our own constants
        #t2jescalibtool.JES_Factors = str(energyCorr)

        #t2calojetcalibtool = T2CaloJetCalibTool()
        #t2calojetcalibtool.T2JetCalibTool = t2jescalibtool
        #t2calojetcalibtool.doJetCalib = True
        #t2calojetcalibtool.T2JetCorrToolTile1  = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolFcal1  = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolEm3Bar = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolHec0   = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolEm3Fwd = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolPsBar  = t2jescalibtool #dummy config
        
        #self.T2JetTools = [t2calojetcalibtool]
        
        
#class T2CaloFastJet_a4JE_TowerCalib_JESCalib (T2CaloFastJet_Base):
    #__slots__ = []
    #def __init__(self, name="T2CaloFastJet_a4JE_TowerCalib_JESCalib"):
        #super( T2CaloFastJet_a4JE_TowerCalib_JESCalib, self ).__init__(name)
        ## here put your customizations
        #self.doCleaning = True
        #self.doT2L1Cleaning = True
        #self.inputType  = PyAthena.T2L1Tools.CALIBRATED
        #self.outputType = PyAthena.T2L1Tools.A4JE_TC_JES
        
        ## add in the calibration tool
        #t2jescalibtool = T2JESCalibTool("T2JESCalibTool_Jet")
        #t2jescalibtool.MinEtCut = 1 
        ## Jet calibration factors
        #energyCorr = [ [ ] ]
        #energyCorr = EtaMassEnergyFactors.energyCorrDict['AntiKt4TopoJets_EM'] # as a test, eventually we'll have our own constants
        #t2jescalibtool.JES_Factors = str(energyCorr)

        #t2calojetcalibtool = T2CaloJetCalibTool()
        #t2calojetcalibtool.T2JetCalibTool = t2jescalibtool
        #t2calojetcalibtool.doJetCalib = True
        #t2calojetcalibtool.T2JetCorrToolTile1  = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolFcal1  = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolEm3Bar = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolHec0   = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolEm3Fwd = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolPsBar  = t2jescalibtool #dummy config
        
        #self.T2JetTools = [t2calojetcalibtool]
        
        
#class T2CaloFastJet_a10JE (T2CaloFastJet_Base):
    #__slots__ = []
    #def __init__(self, name="T2CaloFastJet_a10JE"):
        #super( T2CaloFastJet_a10JE, self ).__init__(name)
        ## here put your customizations
        #self.doCleaning = True
        #self.doT2L1Cleaning = True
        #self.distanceParameter = 1.0
        #self.outputType = PyAthena.T2L1Tools.A10JE
        
class T2CaloFastJet_a4JE (T2CaloFastJet_Base):
    __slots__ = []
    def __init__(self, name="T2CaloFastJet_a4JE"):
        super( T2CaloFastJet_a4JE, self ).__init__(name)
        # here put your customizations
        self.doCleaning = True
        self.doT2L1Cleaning = True
        self.outputType = PyAthena.T2L1Tools.A4JE
        self.writeMultipleOutputTEs = True
        self.secondOutputType = PyAthena.T2L1Tools.L2CONE
class T2CaloFastJet_a10JE (T2CaloFastJet_Base):
    __slots__ = []
    def __init__(self, name="T2CaloFastJet_a10JE"):
        super( T2CaloFastJet_a10JE, self ).__init__(name)
        # here put your customizations
        self.doCleaning = True
        self.doT2L1Cleaning = True
        self.outputType = PyAthena.T2L1Tools.A10JE
        self.distanceParameter = 1.0
        self.writeMultipleOutputTEs = True
        self.secondOutputType = PyAthena.T2L1Tools.L2CONE
###############
# All non-multiple output TE algorithms should not be used with current L1.5 L2 interface
###############
#class T2CaloFastJet_a4JE_PileupSubtraction (T2CaloFastJet_Base):
    #__slots__ = []
    #def __init__(self, name="T2CaloFastJet_a4JE_PileupSubtraction"):
        #super( T2CaloFastJet_a4JE_PileupSubtraction, self ).__init__(name)
        ## here put your customizations
        #self.doCleaning = True
        #self.doT2L1Cleaning = True
        #self.outputType = PyAthena.T2L1Tools.A4JE_PU_SUB
        #self.doPileupSubtraction = True

#class T2CaloFastJet_a4TT (T2CaloFastJet_Base):
    #__slots__ = []
    #def __init__(self, name="T2CaloFastJet_a4TT"):
        #super( T2CaloFastJet_a4TT, self ).__init__(name)
        ## here put your customizations
        #self.doCleaning = True
        #self.doT2L1Cleaning = True
        #self.outputType = PyAthena.T2L1Tools.A4TT

#class T2CaloFastJet_a4TT_TowerCalib (T2CaloFastJet_Base):
    #__slots__ = []
    #def __init__(self, name="T2CaloFastJet_a4TT_TowerCalib"):
        #super( T2CaloFastJet_a4TT_TowerCalib, self ).__init__(name)
        ## here put your customizations
        #self.doCleaning = True
        #self.doT2L1Cleaning = True
        #self.inputType  = PyAthena.T2L1Tools.CALIBRATED
        #self.outputType = PyAthena.T2L1Tools.A4TT_TC
        
        
#class T2CaloFastJet_a4TT_JESCalib (T2CaloFastJet_Base):
    #__slots__ = []
    #def __init__(self, name="T2CaloFastJet_a4TT_JESCalib"):
        #super( T2CaloFastJet_a4TT_JESCalib, self ).__init__(name)
        ## here put your customizations
        #self.doCleaning = True
        #self.doT2L1Cleaning = True
        #self.outputType = PyAthena.T2L1Tools.A4TT_JES
        
        ## add in the calibration tool
        #t2jescalibtool = T2JESCalibTool("T2JESCalibTool_Jet")
        #t2jescalibtool.MinEtCut = 1 
        ## Jet calibration factors
        #energyCorr = [ [ ] ]
        #energyCorr = EtaMassEnergyFactors.energyCorrDict['AntiKt4TopoJets_EM'] # as a test, eventually we'll have our own constants
        #t2jescalibtool.JES_Factors = str(energyCorr)

        #t2calojetcalibtool = T2CaloJetCalibTool()
        #t2calojetcalibtool.T2JetCalibTool = t2jescalibtool
        #t2calojetcalibtool.doJetCalib = True
        #t2calojetcalibtool.T2JetCorrToolTile1  = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolFcal1  = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolEm3Bar = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolHec0   = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolEm3Fwd = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolPsBar  = t2jescalibtool #dummy config
        
        #self.T2JetTools = [t2calojetcalibtool]
        
        
#class T2CaloFastJet_a4TT_TowerCalib_JESCalib (T2CaloFastJet_Base):
    #__slots__ = []
    #def __init__(self, name="T2CaloFastJet_a4TT_TowerCalib_JESCalib"):
        #super( T2CaloFastJet_a4TT_TowerCalib_JESCalib, self ).__init__(name)
        ## here put your customizations
        #self.doCleaning = True
        #self.doT2L1Cleaning = True
        #self.inputType  = PyAthena.T2L1Tools.CALIBRATED
        #self.outputType = PyAthena.T2L1Tools.A4TT_TC_JES
        
        ## add in the calibration tool
        #t2jescalibtool = T2JESCalibTool("T2JESCalibTool_Jet")
        #t2jescalibtool.MinEtCut = 1 
        ## Jet calibration factors
        #energyCorr = [ [ ] ]
        #energyCorr = EtaMassEnergyFactors.energyCorrDict['AntiKt4TopoJets_EM'] # as a test, eventually we'll have our own constants
        #t2jescalibtool.JES_Factors = str(energyCorr)

        #t2calojetcalibtool = T2CaloJetCalibTool()
        #t2calojetcalibtool.T2JetCalibTool = t2jescalibtool
        #t2calojetcalibtool.doJetCalib = True
        #t2calojetcalibtool.T2JetCorrToolTile1  = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolFcal1  = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolEm3Bar = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolHec0   = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolEm3Fwd = t2jescalibtool #dummy config
        #t2calojetcalibtool.T2JetCorrToolPsBar  = t2jescalibtool #dummy config
        
        #self.T2JetTools = [t2calojetcalibtool]
        

#class T2CaloFastJet_a10TT (T2CaloFastJet_Base):
    #__slots__ = []
    #def __init__(self, name="T2CaloFastJet_a10TT"):
        #super( T2CaloFastJet_a10TT, self ).__init__(name)
        ## here put your customizations
        #self.doCleaning = True
        #self.doT2L1Cleaning = True
        #self.distanceParameter = 1.0
        #self.outputType = PyAthena.T2L1Tools.A10TT
        
        
class T2CaloFastJet_a4TT (T2CaloFastJet_Base):
    __slots__ = []
    def __init__(self, name="T2CaloFastJet_a4TT"):
        super( T2CaloFastJet_a4TT, self ).__init__(name)
        # here put your customizations
        self.doCleaning = True
        self.doT2L1Cleaning = True
        self.outputType = PyAthena.T2L1Tools.A4TT
        self.writeMultipleOutputTEs = True
        self.secondOutputType = PyAthena.T2L1Tools.L2CONE
        
class T2CaloFastJet_a10TT (T2CaloFastJet_Base):
    __slots__ = []
    def __init__(self, name="T2CaloFastJet_a10TT"):
        super( T2CaloFastJet_a10TT, self ).__init__(name)
        # here put your customizations
        self.doCleaning = True
        self.doT2L1Cleaning = True
        self.outputType = PyAthena.T2L1Tools.A10TT
        self.distanceParameter = 1.0
        self.writeMultipleOutputTEs = True
        self.secondOutputType = PyAthena.T2L1Tools.L2CONE

class T2CaloFastJet_a4TT_JESCalib (T2CaloFastJet_Base):
    __slots__ = []
    def __init__(self, name="T2CaloFastJet_a4TT_JESCalib"):
        super( T2CaloFastJet_a4TT_JESCalib, self ).__init__(name)
        # here put your customizations
        self.doCleaning = True
        self.doT2L1Cleaning = True
        self.outputType = PyAthena.T2L1Tools.A4TT_JES
        self.pTmin = 10000. # here we could lower the pTmin cut so we can calibrate jets up.
        
        # add in the calibration tool
        t2l1calojetcalibtool = T2L1CaloJetCalibTool()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += T2L1CaloJetCalibTool()
        # Jet calibration factors
        energyCorr = [ [ ] ]
        energyCorr = CalibrationFactorsL15.energyCorrDict['L2FS_em_residual_factors'] ### DEBUG FACTORS replace with real ones
        Eta_bins   = CalibrationFactorsL15.binEdgeDict['L2FS_em_residual_factors_eta'] 
        t2l1calojetcalibtool.Calib_Factors = str(energyCorr)
        t2l1calojetcalibtool.Eta_bin_edges = Eta_bins
        assert len(Eta_bins)-1 == len(energyCorr), "T2CaloFastJet_a4TT_JESCalib eta bin length %i-1 != number of x bins in energy correction matrix %i"%(len(Eta_bins),len(energyCorr))
        
        # add in the calibration tool
        t2jescalibtool = T2JESCalibTool("T2JESCalibTool_Jet")
        t2jescalibtool.MinEtCut = 1 
        # Jet calibration factors
        energyCorr = [ [ ] ]
        #energyCorr = EtaMassEnergyFactors.energyCorrDict['AntiKt4TopoJets_EM'] # as a test, eventually we'll have our own constants
        t2jescalibtool.JES_Factors = str(energyCorr)
        t2calojetcalibtool = T2CaloJetCalibTool()
        t2calojetcalibtool.T2JetCalibTool = t2jescalibtool
        t2calojetcalibtool.doJetCalib = True
        t2calojetcalibtool.T2JetCorrToolTile1  = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolFcal1  = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolEm3Bar = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolHec0   = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolEm3Fwd = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolPsBar  = t2jescalibtool #dummy config 
         
        self.T2JetTools = [t2l1calojetcalibtool,t2calojetcalibtool]      
        
        self.writeMultipleOutputTEs = True
        self.secondOutputType = PyAthena.T2L1Tools.L2CONE
        
#class T2CaloFastJet_a4TT_PileupSubtraction (T2CaloFastJet_Base):
    #__slots__ = []
    #def __init__(self, name="T2CaloFastJet_a4TT_PileupSubtraction"):
        #super( T2CaloFastJet_a4TT_PileupSubtraction, self ).__init__(name)
        ## here put your customizations
        #self.doCleaning = True
        #self.doT2L1Cleaning = True
        #self.outputType = PyAthena.T2L1Tools.A4TT_PU_SUB
        #self.doPileupSubtraction = True
        
## depreciated L1.5 FEXes
class T2L1CaloFullScanFastJet_JE_Jet (T2L1CaloFullScanJet):
    __slots__ = []
    def __init__(self, name="T2L1CaloFullScanFastJet_JE_Jet"):
        super( T2L1CaloFullScanFastJet_JE_Jet, self ).__init__(name)

        from TrigT2CaloJet.TrigT2CaloJetMonitoring import TrigT2L1CaloJetValidationMonitoring, TrigT2L1CaloJetOnlineMonitoring, TrigT2L1CaloJetCosmicMonitoring
        validation = TrigT2L1CaloJetValidationMonitoring()
        online = TrigT2L1CaloJetOnlineMonitoring()
        cosmic = TrigT2L1CaloJetCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("T2L1CaloFullScanJet_JE_Time")
        time.TimerHistLimits = [0,20]
        self.doTiming=True

        self.AthenaMonTools = [ validation, online, time, cosmic]
        
        # here put your customizations
        t2l1fullscanfastjettool = T2L1CaloJetFullScanFastJetTool()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += T2L1CaloJetFullScanFastJetTool()
        t2l1fullscanfastjettool.coneRadius = 0.4        
        t2l1fullscanfastjettool.doTriggerTowers = False
        t2l1fullscanfastjettool.doCleaning = True
        
        self.T2JetTools = [t2l1fullscanfastjettool]
        
class T2L1CaloFullScanFastJet_TT_Jet (T2L1CaloFullScanJet):
    __slots__ = []
    def __init__(self, name="T2L1CaloFullScanFastJet_TT_Jet"):
        super( T2L1CaloFullScanFastJet_TT_Jet, self ).__init__(name)

        from TrigT2CaloJet.TrigT2CaloJetMonitoring import TrigT2L1CaloJetValidationMonitoring, TrigT2L1CaloJetOnlineMonitoring, TrigT2L1CaloJetCosmicMonitoring
        validation = TrigT2L1CaloJetValidationMonitoring()
        online = TrigT2L1CaloJetOnlineMonitoring()
        cosmic = TrigT2L1CaloJetCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("T2L1CaloFullScanJet_TT_Time")
        time.TimerHistLimits = [0,20]
        self.doTiming=True

        self.AthenaMonTools = [ validation, online, time, cosmic]
        
        # here put your customizations
        t2l1fullscanfastjettool = T2L1CaloJetFullScanFastJetTool()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += T2L1CaloJetFullScanFastJetTool()
        t2l1fullscanfastjettool.coneRadius = 0.4        
        t2l1fullscanfastjettool.doTriggerTowers = True
        t2l1fullscanfastjettool.doCleaning   = True
        t2l1fullscanfastjettool.doJetFinding = True
        
        self.T2JetTools = [t2l1fullscanfastjettool]
        
        # debug
        self.recordOutput = True
        
class T2L1CaloFullScanFastJet_10_TT_Jet (T2L1CaloFullScanJet):
    __slots__ = []
    def __init__(self, name="T2L1CaloFullScanFastJet_10_TT_Jet"):
        super( T2L1CaloFullScanFastJet_10_TT_Jet, self ).__init__(name)

        from TrigT2CaloJet.TrigT2CaloJetMonitoring import TrigT2L1CaloJetValidationMonitoring, TrigT2L1CaloJetOnlineMonitoring, TrigT2L1CaloJetCosmicMonitoring
        validation = TrigT2L1CaloJetValidationMonitoring()
        online = TrigT2L1CaloJetOnlineMonitoring()
        cosmic = TrigT2L1CaloJetCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("T2L1CaloFullScanJet_10_TT_Time")
        time.TimerHistLimits = [0,20]
        self.doTiming=True

        self.AthenaMonTools = [ validation, online, time, cosmic]
        
        # here put your customizations
        t2l1fullscanfastjettool = T2L1CaloJetFullScanFastJetTool()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += T2L1CaloJetFullScanFastJetTool()
        t2l1fullscanfastjettool.coneRadius = 1.0      
        t2l1fullscanfastjettool.doTriggerTowers = True
        
        self.T2JetTools = [t2l1fullscanfastjettool]


class T2L1CaloFullScanFastJet_10_JE_Jet (T2L1CaloFullScanJet):
    __slots__ = []
    def __init__(self, name="T2L1CaloFullScanFastJet_10_JE_Jet"):
        super( T2L1CaloFullScanFastJet_10_JE_Jet, self ).__init__(name)

        from TrigT2CaloJet.TrigT2CaloJetMonitoring import TrigT2L1CaloJetValidationMonitoring, TrigT2L1CaloJetOnlineMonitoring, TrigT2L1CaloJetCosmicMonitoring
        validation = TrigT2L1CaloJetValidationMonitoring()
        online = TrigT2L1CaloJetOnlineMonitoring()
        cosmic = TrigT2L1CaloJetCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("T2L1CaloFullScanJet_10_JE_Time")
        time.TimerHistLimits = [0,20]
        self.doTiming=True

        self.AthenaMonTools = [ validation, online, time, cosmic]
        
        # here put your customizations
        t2l1fullscanfastjettool = T2L1CaloJetFullScanFastJetTool()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += T2L1CaloJetFullScanFastJetTool()
        t2l1fullscanfastjettool.coneRadius = 1.0      
        t2l1fullscanfastjettool.doTriggerTowers = False
        
        self.T2JetTools = [t2l1fullscanfastjettool]


class T2L1CaloJet_Jet (T2L1CaloJet):
    __slots__ = []
    def __init__(self, name="T2L1CaloJet_Jet"):
        super( T2L1CaloJet_Jet, self ).__init__(name)

        from TrigT2CaloJet.TrigT2CaloJetMonitoring import TrigT2VirtualL1CaloJetValidationMonitoring, TrigT2VirtualL1CaloJetOnlineMonitoring, TrigT2VirtualL1CaloJetCosmicMonitoring
        validation = TrigT2VirtualL1CaloJetValidationMonitoring()
        online = TrigT2VirtualL1CaloJetOnlineMonitoring()
        cosmic = TrigT2VirtualL1CaloJetCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("T2L1CaloJet_Time")
        time.TimerHistLimits = [0,20]
        self.doTiming=True

        self.AthenaMonTools = [ validation, online, time, cosmic]
        

class T2CaloJetBase (T2CaloJet):
    __slots__ = []
    def __init__(self, name):
        super( T2CaloJetBase, self ).__init__(name)

        from TrigT2CaloJet.TrigT2CaloJetMonitoring import TrigT2CaloJetValidationMonitoring, TrigT2CaloJetOnlineMonitoring, TrigT2CaloJetCosmicMonitoring, TrigT2CaloJetTimeMonitoring
        validation = TrigT2CaloJetValidationMonitoring()
        online = TrigT2CaloJetOnlineMonitoring()
        cosmic = TrigT2CaloJetCosmicMonitoring()
        timeUnp = TrigT2CaloJetTimeMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("T2CaloJet_Time")

        self.AthenaMonTools = [ validation, online, time, timeUnp, cosmic]

        # are these the correct widths for the jets ??
        # really the best thing to do would be for T2CaloJet
        # to determine the jet size *itself*, perhaps even on
        # a jet-by-jet basis
        self.EtaIDWidth = 0.2
        self.PhiIDWidth = 0.2


### Classes for non-calibrated L2 jets ####

class T2CaloJet_Jet_noCalib (T2CaloJetBase):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_noCalib"):
        super(T2CaloJet_Jet_noCalib, self).__init__(name)
        
        # here put your customizations
        t2calojetconetool = T2CaloJetConeTool()
        t2calojetconetool.coneRadius = 0.4
        t2calojetconetool.numberOfIterations = 3
        t2calojetconetool.coneFJRadius=[1.0,0.7,0.4]
        t2calojetconetool.cellQualityThresholdLAr = 4000
        t2calojetconetool.leadingCellFraction = 0.9 

        self.EtaWidth = 0.5
        self.PhiWidth = 0.5
        self.clearJetGrid = False
        #self.OutputLevel = INFO

        t2calojetgridfromcells = T2CaloJetGridFromCells()
        t2calojetgridfromcells.doTiming = True
        t2calojetgridfromcells.doHECcellQualityCleaning = False

        self.jetOutputKey = "TrigT2CaloJet"

        self.T2JetTools = [t2calojetgridfromcells,
                           t2calojetconetool
                           ]

class T2CaloJet_Jet_noCalib_cellCleaning (T2CaloJet_Jet_noCalib):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_noCalib_cellCleaning"):
        super(T2CaloJet_Jet_noCalib_cellCleaning, self).__init__(name)
        
        t2calojetgridfromcells = T2CaloJetGridFromCells("T2CaloJetGridFromCells_cellCleaning")
        t2calojetgridfromcells.doHECcellQualityCleaning = True
        t2calojetgridfromcells.cellQualityThresholdHEC = 65535


class T2CaloJet_Jet_noCalib_noiseCut (T2CaloJet_Jet_noCalib):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_noCalib_noiseCut",noisecut=2.):
        mlog = logging.getLogger( 'T2CaloJet_Jet_noCalib_noiseCut:' )
        try: 
            from AthenaCommon.AppMgr import ToolSvc
            from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
            theNoiseTool = CaloNoiseToolDefault()
            ToolSvc += theNoiseTool
            #cellTool.CaloNoiseTool=theCaloNoiseTool
        except Exception:
            mlog.error("could not get handle to CaloNoiseToolDefault")
            print traceback.format_exc()
            return False

        super(T2CaloJet_Jet_noCalib_noiseCut, self).__init__(name)
        #self.t2calojetgridfromcells = T2CaloJetGridFromCells()
        self.T2JetTools[0].noiseTool = theNoiseTool
        self.T2JetTools[0].applyNoiseCut = True
        self.T2JetTools[0].noiseCutValue = noisecut

### Classes for L2 jets calibrated with the offline JES correction ###

class T2CaloJet_Jet_JESCalib (T2CaloJetBase):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_JESCalib"):
        super(T2CaloJet_Jet_JESCalib, self).__init__(name)
        
        # here put your customizations
        t2calojetconetool = T2CaloJetConeTool()
        t2calojetconetool.coneRadius = 0.4
        t2calojetconetool.numberOfIterations = 3
        t2calojetconetool.coneFJRadius=[1.0,0.7,0.4]
        t2calojetconetool.cellQualityThresholdLAr = 4000
        t2calojetconetool.leadingCellFraction = 0.9 

        self.EtaWidth = 0.5
        self.PhiWidth = 0.5
        self.clearJetGrid = False
        #self.OutputLevel = INFO

        t2jescalibtool = T2JESCalibTool("T2JESCalibTool_Jet")
        t2jescalibtool.MinEtCut = 1 
        # Jet calibration factors
        energyCorr = [ [ ] ]
        #energyCorr = EtaMassEnergyFactors.energyCorrDict['AntiKt4TopoJets_EM']
        t2jescalibtool.JES_Factors = str(energyCorr)

        t2calojetcalibtool = T2CaloJetCalibTool()
        t2calojetcalibtool.T2JetCalibTool = t2jescalibtool
        t2calojetcalibtool.doJetCalib = True
        t2calojetcalibtool.T2JetCorrToolTile1  = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolFcal1  = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolEm3Bar = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolHec0   = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolEm3Fwd = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolPsBar  = t2jescalibtool #dummy config

        t2calojetgridfromcells = T2CaloJetGridFromCells()
        t2calojetgridfromcells.doTiming = True
        t2calojetgridfromcells.doHECcellQualityCleaning = False

        self.jetOutputKey = "TrigT2CaloJet"

        self.T2JetTools = [t2calojetgridfromcells,
                           t2calojetconetool,
                           t2calojetcalibtool]

class T2CaloJet_Jet_JESCalib_cellCleaning (T2CaloJet_Jet_JESCalib):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_JESCalib_cellCleaning"):
        
        t2calojetgridfromcells = T2CaloJetGridFromCells("T2CaloJetGridFromCells_cellCleaning")
        t2calojetgridfromcells.doHECcellQualityCleaning = True
        t2calojetgridfromcells.cellQualityThresholdHEC = 65535


class T2CaloJet_Jet_JESCalib_noiseCut (T2CaloJet_Jet_JESCalib):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_JESCalib_noiseCut",noisecut=2.):
        mlog = logging.getLogger( 'T2CaloJet_Jet_JESCalib_noiseCut:' )
        try: 
            from AthenaCommon.AppMgr import ToolSvc
            from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
            theNoiseTool = CaloNoiseToolDefault()
            ToolSvc += theNoiseTool
            #cellTool.CaloNoiseTool=theCaloNoiseTool
        except Exception:
            mlog.error("could not get handle to CaloNoiseToolDefault")
            print traceback.format_exc()
            return False

        super(T2CaloJet_Jet_JESCalib_noiseCut, self).__init__(name)
        #self.t2calojetgridfromcells = T2CaloJetGridFromCells()
        self.T2JetTools[0].noiseTool = theNoiseTool
        self.T2JetTools[0].applyNoiseCut = True
        self.T2JetTools[0].noiseCutValue = noisecut

class T2CaloJet_Jet_JESCalib_writeLayers (T2CaloJet_Jet_JESCalib):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_JESCalib_writeLayers"):
        super(T2CaloJet_Jet_JESCalib_writeLayers, self).__init__(name)
        self.fillLayerInfo = True

class T2CaloJet_Jet_JESCalib_cellCleaning_writeLayers (T2CaloJet_Jet_JESCalib_cellCleaning):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_JESCalib_cellCleaning_writeLayers"):
        super(T2CaloJet_Jet_JESCalib_cellCleaning_writeLayers, self).__init__(name)
        self.fillLayerInfo = True

class T2CaloJet_Jet_JESCalib_noiseCut_writeLayers (T2CaloJet_Jet_JESCalib_noiseCut):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_JESCalib_noiseCut_writeLayers"):
        super(T2CaloJet_Jet_JESCalib_noiseCut_writeLayers, self).__init__(name)
        self.fillLayerInfo = True

### Classes for jets calibrated with the offline GSC procedure ###

class T2CaloJet_Jet_GSCalib (T2CaloJetBase):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_GSCalib"):
        super(T2CaloJet_Jet_GSCalib, self).__init__(name)
        
        # here put your customizations
        t2calojetconetool = T2CaloJetConeTool()
        t2calojetconetool.coneRadius = 0.4
        t2calojetconetool.numberOfIterations = 3
        t2calojetconetool.coneFJRadius=[1.0,0.7,0.4]
        t2calojetconetool.cellQualityThresholdLAr = 4000
        t2calojetconetool.leadingCellFraction = 0.9 

        self.EtaWidth = 0.5
        self.PhiWidth = 0.5
        self.clearJetGrid = False
        #self.OutputLevel = INFO

        t2jescalibtool = T2JESCalibTool("T2JESCalibTool_Jet")
        t2jescalibtool.MinEtCut = 1 
        energyCorr = [ [ ] ]
        #energyCorr = EtaMassEnergyFactors.energyCorrDict['AntiKt4TopoJets_EM']
        t2jescalibtool.JES_Factors = str(energyCorr)

        t2gscalibtooltile1 = T2GSCalibTool("T2GSCalibToolTile1_Jet")
        etaFit        = []
        etabins       = []
        ptbins        = []
        layerFracBins = []
        #etaFit        = Tile1FractionConstants.tile1EtaFitDict['AntiKt4TopoJets']
        #etabins       = Tile1FractionConstants.tile1EtaBinsDict['AntiKt4TopoJets']
        #ptbins        = Tile1FractionConstants.tile1PtBinsDict['AntiKt4TopoJets']
        #layerFracBins = Tile1FractionConstants.tile1LayerFracBinsDict['AntiKt4TopoJets']
        t2gscalibtooltile1.CalibConstants  = str(etaFit)
        t2gscalibtooltile1.EtaBins         = etabins
        t2gscalibtooltile1.EtaMin          = -1.7
        t2gscalibtooltile1.EtaMax          = 1.7
        t2gscalibtooltile1.PtBins          = ptbins
        t2gscalibtooltile1.PtMin           = 1000.0
        t2gscalibtooltile1.JetPropertyBins = layerFracBins

        t2gscalibtoolfcal1 = T2GSCalibTool("T2GSCalibToolFcal1_Jet")
        etaFit        = []
        etabins       = []
        ptbins        = []
        layerFracBins = []
        #etaFit        = FCAL1FractionConstants.fcal1EtaFitDict['AntiKt4TopoJets']
        #etabins       = FCAL1FractionConstants.fcal1EtaBinsDict['AntiKt4TopoJets']
        #ptbins        = FCAL1FractionConstants.fcal1PtBinsDict['AntiKt4TopoJets']
        #layerFracBins = FCAL1FractionConstants.fcal1LayerFracBinsDict['AntiKt4TopoJets']
        t2gscalibtoolfcal1.CalibConstants  = str(etaFit)
        t2gscalibtoolfcal1.EtaBins         = etabins
        t2gscalibtoolfcal1.EtaMin          = -4.4
        t2gscalibtoolfcal1.EtaGapMin       = -3.5
        t2gscalibtoolfcal1.EtaGapMax       = 3.5
        t2gscalibtoolfcal1.EtaMax          = 4.4
        t2gscalibtoolfcal1.PtBins          = ptbins
        t2gscalibtoolfcal1.PtMin           = 1000.0
        t2gscalibtoolfcal1.JetPropertyBins = layerFracBins

        t2gscalibtoolem3bar = T2GSCalibTool("T2GSCalibToolEm3Bar_Jet")
        t2gscalibtoolem3fwd = T2GSCalibTool("T2GSCalibToolEm3Fwd_Jet")
        etaFit        = []
        etabins       = []
        ptbins        = []
        layerFracBins = []
        #etaFit        = EM3FractionConstants.em3EtaFitDict['AntiKt4TopoJets']
        #etabins       = EM3FractionConstants.em3EtaBinsDict['AntiKt4TopoJets']
        #ptbins        = EM3FractionConstants.em3PtBinsDict['AntiKt4TopoJets']
        #layerFracBins = EM3FractionConstants.em3LayerFracBinsDict['AntiKt4TopoJets']
        t2gscalibtoolem3bar.CalibConstants  = str(etaFit)
        t2gscalibtoolem3bar.EtaBins         = etabins
        t2gscalibtoolem3bar.EtaMin          = -1.2
        t2gscalibtoolem3bar.EtaMax          = 1.2
        t2gscalibtoolem3bar.PtBins          = ptbins
        t2gscalibtoolem3bar.PtMin           = 1000
        t2gscalibtoolem3bar.JetPropertyBins = layerFracBins
        t2gscalibtoolem3fwd.CalibConstants  = str(etaFit)
        t2gscalibtoolem3fwd.EtaBins         = etabins
        t2gscalibtoolem3fwd.EtaMin          = -3.0
        t2gscalibtoolem3fwd.EtaGapMin       = -1.7
        t2gscalibtoolem3fwd.EtaGapMax       = 1.7
        t2gscalibtoolem3fwd.EtaMax          = 3.0
        t2gscalibtoolem3fwd.PtBins          = ptbins
        t2gscalibtoolem3fwd.PtMin           = 1000.0
        t2gscalibtoolem3fwd.JetPropertyBins = layerFracBins

        t2gscalibtoolhec0 = T2GSCalibTool("T2GSCalibToolHec0_Jet")
        etaFit        = []
        etabins       = []
        ptbins        = []
        layerFracBins = []
        #etaFit        = HEC0FractionConstants.hec0EtaFitDict['AntiKt4TopoJets']
        #etabins       = HEC0FractionConstants.hec0EtaBinsDict['AntiKt4TopoJets']
        #ptbins        = HEC0FractionConstants.hec0PtBinsDict['AntiKt4TopoJets']
        #layerFracBins = HEC0FractionConstants.hec0LayerFracBinsDict['AntiKt4TopoJets']
        t2gscalibtoolhec0.CalibConstants  = str(etaFit)
        t2gscalibtoolhec0.EtaBins         = etabins
        t2gscalibtoolhec0.EtaMin          = -3.0
        t2gscalibtoolhec0.EtaGapMin       = -1.4
        t2gscalibtoolhec0.EtaGapMax       = 1.4
        t2gscalibtoolhec0.EtaMax          = 3.0
        t2gscalibtoolhec0.PtBins          = ptbins
        t2gscalibtoolhec0.PtMin           = 1000.0
        t2gscalibtoolhec0.JetPropertyBins = layerFracBins

        t2gscalibtoolpsbar = T2GSCalibTool("T2GSCalibToolPsBar_Jet")
        etaFit        = []
        etabins       = []
        ptbins        = []
        layerFracBins = []
        #etaFit        = PresamplerFractionConstants.presEtaFitDict['AntiKt4TopoJets']
        #etabins       = PresamplerFractionConstants.presEtaBinsDict['AntiKt4TopoJets']
        #ptbins        = PresamplerFractionConstants.presPtBinsDict['AntiKt4TopoJets']
        #layerFracBins = PresamplerFractionConstants.presLayerFracBinsDict['AntiKt4TopoJets']
        t2gscalibtoolpsbar.CalibConstants  = str(etaFit)
        t2gscalibtoolpsbar.EtaBins         = etabins
        t2gscalibtoolpsbar.EtaMin          = -1.7
        t2gscalibtoolpsbar.EtaMax          = 1.7
        t2gscalibtoolpsbar.PtBins          = ptbins
        t2gscalibtoolpsbar.PtMin           = 1000.0
        t2gscalibtoolpsbar.JetPropertyBins = layerFracBins

        t2calojetcalibtool = T2CaloJetCalibTool()
        t2calojetcalibtool.T2JetCalibTool      = t2jescalibtool
        t2calojetcalibtool.T2JetCorrToolTile1  = t2gscalibtooltile1
        t2calojetcalibtool.T2JetCorrToolFcal1  = t2gscalibtoolfcal1
        t2calojetcalibtool.T2JetCorrToolEm3Bar = t2gscalibtoolem3bar
        t2calojetcalibtool.T2JetCorrToolHec0   = t2gscalibtoolhec0
        t2calojetcalibtool.T2JetCorrToolEm3Fwd = t2gscalibtoolem3fwd
        t2calojetcalibtool.T2JetCorrToolPsBar  = t2gscalibtoolpsbar
        t2calojetcalibtool.doJetCalib      = True
        t2calojetcalibtool.doJetCorrTile1  = True
        t2calojetcalibtool.doJetCorrFcal1  = True
        t2calojetcalibtool.doJetCorrEm3Bar = True
        t2calojetcalibtool.doJetCorrHec0   = True
        t2calojetcalibtool.doJetCorrEm3Fwd = True
        t2calojetcalibtool.doJetCorrPsBar  = True

        t2calojetgridfromcells = T2CaloJetGridFromCells()
        t2calojetgridfromcells.doTiming = True
        t2calojetgridfromcells.doHECcellQualityCleaning = False

        self.jetOutputKey = "TrigT2CaloJet"
        self.jetOutputKey = "TrigT2Calo"

        self.T2JetTools = [t2calojetgridfromcells,
                           t2calojetconetool,
                           t2calojetcalibtool]


class T2CaloJet_Jet_GSCalib_cellCleaning (T2CaloJet_Jet_GSCalib):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_GSCalib_cellCleaning"):
        
        t2calojetgridfromcells = T2CaloJetGridFromCells("T2CaloJetGridFromCells_cellCleaning")
        t2calojetgridfromcells.doHECcellQualityCleaning = True
        t2calojetgridfromcells.cellQualityThresholdHEC = 65535


class T2CaloJet_Jet_GSCalib_noiseCut (T2CaloJet_Jet_GSCalib):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_GSCalib_noiseCut",noisecut=2.):
        mlog = logging.getLogger( 'T2CaloJet_Jet_GSCalib_noiseCut:' )
        try: 
            from AthenaCommon.AppMgr import ToolSvc
            from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
            theNoiseTool = CaloNoiseToolDefault()
            ToolSvc += theNoiseTool
            #cellTool.CaloNoiseTool=theCaloNoiseTool
        except Exception:
            mlog.error("could not get handle to CaloNoiseToolDefault")
            print traceback.format_exc()
            return False

        super(T2CaloJet_Jet_GSCalib_noiseCut, self).__init__(name)
        #self.t2calojetgridfromcells = T2CaloJetGridFromCells()
        self.T2JetTools[0].noiseTool = theNoiseTool
        self.T2JetTools[0].applyNoiseCut = True
        self.T2JetTools[0].noiseCutValue = noisecut

class T2CaloJet_Jet_GSCalib_writeLayers (T2CaloJet_Jet_GSCalib):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_GSCalib_writeLayers"):
        super(T2CaloJet_Jet_GSCalib_writeLayers, self).__init__(name)
        self.fillLayerInfo = True

class T2CaloJet_Jet_GSCalib_cellCleaning_writeLayers (T2CaloJet_Jet_GSCalib_cellCleaning):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_GSCalib_cellCleaning_writeLayers"):
        super(T2CaloJet_Jet_GSCalib_cellCleaning_writeLayers, self).__init__(name)
        self.fillLayerInfo = True

class T2CaloJet_Jet_GSCalib_noiseCut_writeLayers (T2CaloJet_Jet_GSCalib_noiseCut):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_GSCalib_noiseCut_writeLayers"):
        super(T2CaloJet_Jet_GSCalib_noiseCut_writeLayers, self).__init__(name)
        self.fillLayerInfo = True

### Classes for jets calibrated with the dedicated sampling method ####

class T2CaloJet_Jet_sampCalib (T2CaloJetBase):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_sampCalib"):
        super(T2CaloJet_Jet_sampCalib, self).__init__(name)
        
        # here put your customizations
        t2calojetconetool = T2CaloJetConeTool()
        t2calojetconetool.coneRadius = 0.4
        t2calojetconetool.numberOfIterations = 3
        t2calojetconetool.coneFJRadius=[1.0,0.7,0.4]
        t2calojetconetool.cellQualityThresholdLAr = 4000
        t2calojetconetool.leadingCellFraction = 0.9 

        self.EtaWidth = 0.5
        self.PhiWidth = 0.5
        self.clearJetGrid = False
        #self.OutputLevel = INFO

        t2sampcalibtool = T2SampCalibTool("T2SampCalibTool_Jet")
        t2sampcalibtool.MinEtCut = 1 
        t2sampcalibtool.nEtaBin = 50
        t2sampcalibtool.EtRegionLimit = 5000
        #New database tag
        t2sampcalibtool.CreateDbObject=False
        t2sampcalibtool.ReadFromDB=True
        L2CalibFolder = "/CALO/T2CaloCalib/T2CaloJetCalib"
        L2CalibTag = "L2JetCalib-10TeV-Ideal-01"
        from IOVDbSvc.CondDB import conddb
        conddb.addFolder("CALO",L2CalibFolder+"<tag>"+L2CalibTag+"</tag>")

        #############################################################
        ## THESE VALUES ARE NOT USED !!!                            #
        ## ONLY USED AS A REFERENCE FOR WHAT WAS PUT IN THIS DB TAG #
        #############################################################

        t2sampcalibtool.EtaReg=[0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,
                                1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,
                                2.0,2.1,2.2,2.3,2.4,2.5,2.6,2.7,2.8,2.9,
                                3.0,3.1,3.2,3.3,3.4,3.5,3.6,3.7,3.8,3.9,
                                4.0,4.1,4.2,4.3,4.4,4.5,4.6,4.7,4.8,4.9,5.0]

        t2sampcalibtool.VECut=[ 1.0,      1.00500,  1.02006,  1.04534,  1.08107,
                                1.12763,  1.18547,  1.25517,  1.33743,  1.43308,
                                1.54308,  1.66852,  1.81065,  1.97091,  2.15090,
                                2.35240,  2.57746,  2.82832,  3.10748,  3.41774,
                                3.76220,  4.14432,  4.56790,  5.03722,  5.55694,
                                6.13228,  6.76900,  7.47346,  8.25272,  9.11458,
                               10.0677,  11.1215,  12.2866,  13.5748,  14.9987,
                               16.5728,  18.3128,  20.2360,  22.3618,  24.7114,
                               27.3082,  30.1784,  33.3507,  36.8567,  40.7316,
                                1.0,      1.0,      1.0,      1.0,      1.0]

        t2sampcalibtool.Weights=[
        1.71736, -0.0775919, 1., 0.,
	1.68115, -0.0298649, 1., 0.,
        1.72263, -0.0820785, 1., 0.,
        1.59158, -0.0349583, 1., 0.,
        1.71384, -0.0825993, 1., 0.,
        1.56296, -0.0334272, 1., 0.,
	1.71196, -0.0823075, 1., 0.,
        1.55044, -0.0300373, 1., 0.,
        1.73526, -0.0858796, 1., 0.,
        1.52924, -0.0275084, 1., 0.,
        1.74496, -0.0858104, 1., 0.,
        1.61094, -0.032966, 1., 0.,
        1.77075, -0.0874956, 1., 0.,
        1.69556, -0.0399038, 1., 0.,
        1.85845, -0.100017, 1., 0.,
        1.63193, -0.0292829, 1., 0.,
        1.92108, -0.108746, 1., 0.,
        1.60507, -0.0185852, 1., 0.,
        1.90991, -0.109427, 1., 0.,
        1.73985, -0.0214682, 1., 0.,
        1.97429, -0.114934, 1., 0.,
        1.49217, -0.0234559, 1., 0.,
        1.97011, -0.117122, 1., 0.,
        1.40092, -0.0201076, 1., 0.,
        1.96164, -0.119639, 1., 0.,
	1.63634, -0.0399844, 1., 0.,
        1.95562, -0.11617, 1., 0.,
        1.81431, -0.0515824, 1., 0.,
        1.96964, -0.105079, 1., 0.,
        1.86882, -0.0653143, 1., 0.,
	2.0155, -0.146639, 1., 0.,
        1.60657, -0.00444997, 1., 0.,
        1.82239, -0.111195, 1., 0.,
        1.69562, -0.0408746, 1., 0.,
        1.78718, -0.107874, 1., 0.,
        1.6367, -0.0480624, 1., 0.,
        1.73745, -0.100309, 1., 0.,
        1.54562, -0.0400314, 1., 0.,
        1.70874, -0.0979269, 1., 0.,
        1.53771, -0.0414734, 1., 0.,
        1.68658, -0.0964409, 1., 0.,
        1.53234, -0.037261, 1., 0.,
        1.62871, -0.0876685, 1., 0.,
        1.5901, -0.0464532, 1., 0.,
        1.60288, -0.0850231, 1., 0.,
        1.51135, -0.0319969, 1., 0.,
        1.55495, -0.0760847, 1., 0.,
        1.60024, -0.0493416, 1., 0.,
        1.61099, -0.0871627, 1., 0.,
	1.56848, -0.0372076, 1., 0.,
        1.61012, -0.080997, 1., 0.,
        1.51628, -0.031721, 1., 0.,
        1.5527, -0.0745029, 1., 0.,
        1.68515, -0.0610052, 1., 0.,
	1.54251, -0.0749989, 1., 0.,
        1.75605, -0.0614183, 1., 0.,
        1.5972, -0.0847241, 1., 0.,
        1.83407, -0.0712398, 1., 0.,
        1.63888, -0.085407, 1., 0.,
        2.15163, -0.111614, 1., 0.,
        1.86924, -0.127399, 1., 0.,
        2.04718, -0.021142, 1., 0.,
        1.93682, -0.114535, 1., 0.,
        2.42857, -0.0398346, 1., 0.,
        2.05425, -0.0634181, 1., 0.,
        2.31097, -0.17624, 1., 0.,
        2.3425, -0.134324, 1., 0.,
        1.25573, 0.0160847, 1., 0.,
        2.22724, -0.190316, 1., 0.,
        1.59135, -0.0403045, 1., 0.,
        1.70013, -0.105121, 1., 0.,
        2.18796, -0.184129, 1., 0.,
        1.46967, -0.0542641, 1., 0.,
	1.40854, -0.00397349, 1., 0.,
        1.7066, -0.103722, 1., 0.,
        2.02067, -0.133273, 1., 0.,
        2.3121, -0.244998, 1., 0.,
        -0.0145766, 0.362844, 1., 0.,
	1.62273, -0.0687159, 1., 0.,
        2.01868, -0.120886, 1., 0.,
        1.07068, 0.0528097, 1., 0.,
        3.34939, -0.412248, 1., 0.,
        1.18681, 0.0214479, 1., 0.,
        0.91684, 0.149364, 1., 0.,
        0.377452, 0.214947, 1., 0.,
        10.9671, -2.31455, 1., 0.,
        2.77778, -0.372772, 1., 0.,
        -6.52839, 2.00992, 1., 0.,
        -5.04373, 1.62889, 1., 0.,
        -9.22444, 2.66876, 1., 0.,
        #46; Eta range [min,max]: [4.5,4.6]
        #NEntries: 413
        3.33217,-0.26788,1,0,
        3.66022,-0.303515,1,0,
        #47; Eta range [min,max]: [4.6,4.7]
        #NEntries: 292
        3.64624,-0.30498,1,0,
        3.732,-0.297252,1,0,
        #48; Eta range [min,max]: [4.7,4.8]
        #NEntries: 207
        5.74177,-0.587572,1,0,
        6.30603,-0.649457,1,0,
        #49; Eta range [min,max]: [4.8,4.9]
        #NEntries: 100
        6.00682,-0.589085,1,0,
        16.6035,-2.09057,1,0,
        #50; Eta range [min,max]: [4.9,5]
        #NEntries: 4
        1.42662,-0.0942826,1,0,
        8.93293,-0.0674104,1,0
        ]

        t2calojetcalibtool = T2CaloJetCalibTool()
        t2calojetcalibtool.T2JetCalibTool = t2sampcalibtool
        t2calojetcalibtool.doJetCalib = True
        t2calojetcalibtool.T2JetCorrToolTile1  = t2sampcalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolFcal1  = t2sampcalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolEm3Bar = t2sampcalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolHec0   = t2sampcalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolEm3Fwd = t2sampcalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolPsBar  = t2sampcalibtool #dummy config

        t2calojetgridfromcells = T2CaloJetGridFromCells()
        t2calojetgridfromcells.doTiming = True
        t2calojetgridfromcells.doHECcellQualityCleaning = False

        self.jetOutputKey = "TrigT2CaloJet"

        self.T2JetTools = [t2calojetgridfromcells,
                           t2calojetconetool,
                           t2calojetcalibtool]

class T2CaloJet_Jet_sampCalib_cellCleaning (T2CaloJet_Jet_sampCalib):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_sampCalib_cellCleaning"):
        
        t2calojetgridfromcells = T2CaloJetGridFromCells("T2CaloJetGridFromCells_cellCleaning")
        t2calojetgridfromcells.doHECcellQualityCleaning = True
        t2calojetgridfromcells.cellQualityThresholdHEC = 65535


class T2CaloJet_Jet_sampCalib_noiseCut (T2CaloJet_Jet_sampCalib):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_Jet_sampCalib_noiseCut",noisecut=2.):
        mlog = logging.getLogger( 'T2CaloJet_Jet_sampCalib_noiseCut:' )
        try: 
            from AthenaCommon.AppMgr import ToolSvc
            from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
            theNoiseTool = CaloNoiseToolDefault()
            ToolSvc += theNoiseTool
            #cellTool.CaloNoiseTool=theCaloNoiseTool
        except Exception:
            mlog.error("could not get handle to CaloNoiseToolDefault")
            print traceback.format_exc()
            return False

        super(T2CaloJet_Jet_sampCalib_noiseCut, self).__init__(name)
        #self.t2calojetgridfromcells = T2CaloJetGridFromCells()
        self.T2JetTools[0].noiseTool = theNoiseTool
        self.T2JetTools[0].applyNoiseCut = True
        self.T2JetTools[0].noiseCutValue = noisecut

### Legacy classes: use only one calibration option, disallow the others ####

L2Calib="noCalib"
#L2Calib="JESCalib"
#L2Calib="JESCalib_LayerInfo"
#L2Calib="GSCalib"
#L2Calib="GSCalib_LayerInfo"
#L2Calib="sampCalib"

if L2Calib=="JESCalib" :
### Use L2 jets calibrated with offline JES correction ###
    class T2CaloJet_Jet (T2CaloJet_Jet_JESCalib):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet"):
            super(T2CaloJet_Jet, self).__init__(name)
    class T2CaloJet_Jet_noise (T2CaloJet_Jet_JESCalib_noiseCut):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_noise"):
            super(T2CaloJet_Jet_noise, self).__init__(name)
    class T2CaloJet_Jet_cellCleaning (T2CaloJet_Jet_JESCalib_cellCleaning):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_cellCleaning"):
            super(T2CaloJet_Jet_cellCleaning, self).__init__(name)
    class T2CaloJet_Jet_L15 (T2CaloJet_Jet_JESCalib):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_L15"):
            super(T2CaloJet_Jet_L15, self).__init__(name)
            #self.jetOutputKey = "TrigT2CaloJet"
elif L2Calib=="JESCalib_LayerInfo" :
### Use L2 jets calibrated with offline JES correction and write Layer energy ###
    class T2CaloJet_Jet (T2CaloJet_Jet_JESCalib_writeLayers):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet"):
            super(T2CaloJet_Jet, self).__init__(name)
    class T2CaloJet_Jet_noise (T2CaloJet_Jet_JESCalib_noiseCut_writeLayers):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_noise"):
            super(T2CaloJet_Jet_noise, self).__init__(name)
    class T2CaloJet_Jet_cellCleaning (T2CaloJet_Jet_JESCalib_cellCleaning_writeLayers):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_cellCleaning"):
            super(T2CaloJet_Jet_cellCleaning, self).__init__(name)
    class T2CaloJet_Jet_L15 (T2CaloJet_Jet_JESCalib_writeLayers):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_L15"):
            super(T2CaloJet_Jet_L15, self).__init__(name)
            #self.jetOutputKey = "TrigT2CaloJet"
elif L2Calib=="GSCalib" :
### Use L2 jets calibrated with offline GSC procedure ###
    class T2CaloJet_Jet (T2CaloJet_Jet_GSCalib):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet"):
            super(T2CaloJet_Jet, self).__init__(name)
    class T2CaloJet_Jet_noise (T2CaloJet_Jet_GSCalib_noiseCut):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_noise"):
            super(T2CaloJet_Jet_noise, self).__init__(name)
    class T2CaloJet_Jet_cellCleaning (T2CaloJet_Jet_GSCalib_cellCleaning):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_cellCleaning"):
            super(T2CaloJet_Jet_cellCleaning, self).__init__(name)
    class T2CaloJet_Jet_L15 (T2CaloJet_Jet_GSCalib):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_L15"):
            super(T2CaloJet_Jet_L15, self).__init__(name)
            #self.jetOutputKey = "TrigT2CaloJet"
elif L2Calib=="GSCalib_LayerInfo" :
### Use L2 jets calibrated with offline GSC procedure and write Layer energy ###
    class T2CaloJet_Jet (T2CaloJet_Jet_GSCalib_writeLayers):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet"):
            super(T2CaloJet_Jet, self).__init__(name)
    class T2CaloJet_Jet_noise (T2CaloJet_Jet_GSCalib_noiseCut_writeLayers):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_noise"):
            super(T2CaloJet_Jet_noise, self).__init__(name)
    class T2CaloJet_Jet_cellCleaning (T2CaloJet_Jet_GSCalib_cellCleaning_writeLayers):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_cellCleaning"):
            super(T2CaloJet_Jet_cellCleaning, self).__init__(name)
    class T2CaloJet_Jet_L15 (T2CaloJet_Jet_GSCalib_writeLayers):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_L15"):
            super(T2CaloJet_Jet_L15, self).__init__(name)
            #self.jetOutputKey = "TrigT2CaloJet"
elif L2Calib=="sampCalib" :
### Use L2 jets calibrated with dedicated sampling method ###
    class T2CaloJet_Jet (T2CaloJet_Jet_sampCalib):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet"):
            super(T2CaloJet_Jet, self).__init__(name)
    class T2CaloJet_Jet_noise (T2CaloJet_Jet_sampCalib_noiseCut):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_noise"):
            super(T2CaloJet_Jet_noise, self).__init__(name)
    class T2CaloJet_Jet_cellCleaning (T2CaloJet_Jet_sampCalib_cellCleaning):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_cellCleaning"):
            super(T2CaloJet_Jet_cellCleaning, self).__init__(name)
    class T2CaloJet_Jet_L15 (T2CaloJet_Jet_sampCalib):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_L15"):
            super(T2CaloJet_Jet_L15, self).__init__(name)
            #self.jetOutputKey = "TrigT2CaloJet"
else:
### Use non-calibrated L2 jets 
    class T2CaloJet_Jet (T2CaloJet_Jet_noCalib):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet"):
            super(T2CaloJet_Jet, self).__init__(name)
    class T2CaloJet_Jet_noise (T2CaloJet_Jet_noCalib_noiseCut):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_noise"):
            super(T2CaloJet_Jet_noise, self).__init__(name)
    class T2CaloJet_Jet_cellCleaning (T2CaloJet_Jet_noCalib_cellCleaning):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_cellCleaning"):
            super(T2CaloJet_Jet_cellCleaning, self).__init__(name)
    class T2CaloJet_Jet_L15 (T2CaloJet_Jet_noCalib):
        __slots__ = []
        def __init__ (self, name="T2CaloJet_Jet_L15"):
            super(T2CaloJet_Jet_L15, self).__init__(name)
            #self.jetOutputKey = "TrigT2CaloJet"

### L2 Jets from Front-End Buffers ###

class T2CaloJet_FEBJet (T2CaloJetBase):
    __slots__ = []
    def __init__ (self, name="T2CaloJet_FEBJet"):
        super(T2CaloJet_FEBJet, self).__init__(name)
        
        # here put your customizations
        t2calojetconetool = T2CaloJetConeTool()
        t2calojetconetool.coneRadius = 0.4
        t2calojetconetool.numberOfIterations = 3
        t2calojetconetool.coneFJRadius=[1.0,0.7,0.4]

        self.EtaWidth = 0.5
        self.PhiWidth = 0.5
        self.clearJetGrid = False
        #self.OutputLevel = INFO

        t2sampcalibtool = T2SampCalibTool("T2SampCalibTool_Jet")
        t2sampcalibtool.MinEtCut = 1 
        t2sampcalibtool.nEtaBin = 50
        t2sampcalibtool.EtRegionLimit = 5000
        #New database tag
        t2sampcalibtool.CreateDbObject=False
        t2sampcalibtool.ReadFromDB=True
        L2CalibFolder = "/CALO/T2CaloCalib/T2CaloJetCalib"
        L2CalibTag = "L2JetCalib-10TeV-Ideal-01"
        from IOVDbSvc.CondDB import conddb
        conddb.addFolder("CALO",L2CalibFolder+"<tag>"+L2CalibTag+"</tag>")

        #############################################################
        ## THESE VALUES ARE NOT USED !!!                            #
        ## ONLY USED AS A REFERENCE FOR WHAT WAS PUT IN THIS DB TAG #
        #############################################################

        t2sampcalibtool.EtaReg=[0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,
                                1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,
                                2.0,2.1,2.2,2.3,2.4,2.5,2.6,2.7,2.8,2.9,
                                3.0,3.1,3.2,3.3,3.4,3.5,3.6,3.7,3.8,3.9,
                                4.0,4.1,4.2,4.3,4.4,4.5,4.6,4.7,4.8,4.9,5.0]

        t2sampcalibtool.VECut=[ 1.0,      1.00500,  1.02006,  1.04534,  1.08107,
                                1.12763,  1.18547,  1.25517,  1.33743,  1.43308,
                                1.54308,  1.66852,  1.81065,  1.97091,  2.15090,
                                2.35240,  2.57746,  2.82832,  3.10748,  3.41774,
                                3.76220,  4.14432,  4.56790,  5.03722,  5.55694,
                                6.13228,  6.76900,  7.47346,  8.25272,  9.11458,
                               10.0677,  11.1215,  12.2866,  13.5748,  14.9987,
                               16.5728,  18.3128,  20.2360,  22.3618,  24.7114,
                               27.3082,  30.1784,  33.3507,  36.8567,  40.7316,
                                1.0,      1.0,      1.0,      1.0,      1.0]

        t2sampcalibtool.Weights=[
        1.71736, -0.0775919, 1., 0.,
	1.68115, -0.0298649, 1., 0.,
        1.72263, -0.0820785, 1., 0.,
        1.59158, -0.0349583, 1., 0.,
        1.71384, -0.0825993, 1., 0.,
        1.56296, -0.0334272, 1., 0.,
	1.71196, -0.0823075, 1., 0.,
        1.55044, -0.0300373, 1., 0.,
        1.73526, -0.0858796, 1., 0.,
        1.52924, -0.0275084, 1., 0.,
        1.74496, -0.0858104, 1., 0.,
        1.61094, -0.032966, 1., 0.,
        1.77075, -0.0874956, 1., 0.,
        1.69556, -0.0399038, 1., 0.,
        1.85845, -0.100017, 1., 0.,
        1.63193, -0.0292829, 1., 0.,
        1.92108, -0.108746, 1., 0.,
        1.60507, -0.0185852, 1., 0.,
        1.90991, -0.109427, 1., 0.,
        1.73985, -0.0214682, 1., 0.,
        1.97429, -0.114934, 1., 0.,
        1.49217, -0.0234559, 1., 0.,
        1.97011, -0.117122, 1., 0.,
        1.40092, -0.0201076, 1., 0.,
        1.96164, -0.119639, 1., 0.,
	1.63634, -0.0399844, 1., 0.,
        1.95562, -0.11617, 1., 0.,
        1.81431, -0.0515824, 1., 0.,
        1.96964, -0.105079, 1., 0.,
        1.86882, -0.0653143, 1., 0.,
	2.0155, -0.146639, 1., 0.,
        1.60657, -0.00444997, 1., 0.,
        1.82239, -0.111195, 1., 0.,
        1.69562, -0.0408746, 1., 0.,
        1.78718, -0.107874, 1., 0.,
        1.6367, -0.0480624, 1., 0.,
        1.73745, -0.100309, 1., 0.,
        1.54562, -0.0400314, 1., 0.,
        1.70874, -0.0979269, 1., 0.,
        1.53771, -0.0414734, 1., 0.,
        1.68658, -0.0964409, 1., 0.,
        1.53234, -0.037261, 1., 0.,
        1.62871, -0.0876685, 1., 0.,
        1.5901, -0.0464532, 1., 0.,
        1.60288, -0.0850231, 1., 0.,
        1.51135, -0.0319969, 1., 0.,
        1.55495, -0.0760847, 1., 0.,
        1.60024, -0.0493416, 1., 0.,
        1.61099, -0.0871627, 1., 0.,
	1.56848, -0.0372076, 1., 0.,
        1.61012, -0.080997, 1., 0.,
        1.51628, -0.031721, 1., 0.,
        1.5527, -0.0745029, 1., 0.,
        1.68515, -0.0610052, 1., 0.,
	1.54251, -0.0749989, 1., 0.,
        1.75605, -0.0614183, 1., 0.,
        1.5972, -0.0847241, 1., 0.,
        1.83407, -0.0712398, 1., 0.,
        1.63888, -0.085407, 1., 0.,
        2.15163, -0.111614, 1., 0.,
        1.86924, -0.127399, 1., 0.,
        2.04718, -0.021142, 1., 0.,
        1.93682, -0.114535, 1., 0.,
        2.42857, -0.0398346, 1., 0.,
        2.05425, -0.0634181, 1., 0.,
        2.31097, -0.17624, 1., 0.,
        2.3425, -0.134324, 1., 0.,
        1.25573, 0.0160847, 1., 0.,
        2.22724, -0.190316, 1., 0.,
        1.59135, -0.0403045, 1., 0.,
        1.70013, -0.105121, 1., 0.,
        2.18796, -0.184129, 1., 0.,
        1.46967, -0.0542641, 1., 0.,
	1.40854, -0.00397349, 1., 0.,
        1.7066, -0.103722, 1., 0.,
        2.02067, -0.133273, 1., 0.,
        2.3121, -0.244998, 1., 0.,
        -0.0145766, 0.362844, 1., 0.,
	1.62273, -0.0687159, 1., 0.,
        2.01868, -0.120886, 1., 0.,
        1.07068, 0.0528097, 1., 0.,
        3.34939, -0.412248, 1., 0.,
        1.18681, 0.0214479, 1., 0.,
        0.91684, 0.149364, 1., 0.,
        0.377452, 0.214947, 1., 0.,
        10.9671, -2.31455, 1., 0.,
        2.77778, -0.372772, 1., 0.,
        -6.52839, 2.00992, 1., 0.,
        -5.04373, 1.62889, 1., 0.,
        -9.22444, 2.66876, 1., 0.,
        #46; Eta range [min,max]: [4.5,4.6]
        #NEntries: 413
        3.33217,-0.26788,1,0,
        3.66022,-0.303515,1,0,
        #47; Eta range [min,max]: [4.6,4.7]
        #NEntries: 292
        3.64624,-0.30498,1,0,
        3.732,-0.297252,1,0,
        #48; Eta range [min,max]: [4.7,4.8]
        #NEntries: 207
        5.74177,-0.587572,1,0,
        6.30603,-0.649457,1,0,
        #49; Eta range [min,max]: [4.8,4.9]
        #NEntries: 100
        6.00682,-0.589085,1,0,
        16.6035,-2.09057,1,0,
        #50; Eta range [min,max]: [4.9,5]
        #NEntries: 4
        1.42662,-0.0942826,1,0,
        8.93293,-0.0674104,1,0
        ]

        t2calojetcalibtool = T2CaloJetCalibTool()
        t2calojetcalibtool.T2SampCalibTool = t2sampcalibtool
        t2calojetcalibtool.UseFebJets = True
        t2calojetcalibtool.doJetCalib = False
        t2calojetcalibtool.T2JetCorrToolTile1  = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolFcal1  = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolEm3Bar = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolHec0   = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolEm3Fwd = t2jescalibtool #dummy config
        t2calojetcalibtool.T2JetCorrToolPsBar  = t2jescalibtool #dummy config

        t2calojetgridfromfebs = T2CaloJetGridFromFEBHeader()
        t2calojetgridfromfebs.doTiming = True

        self.jetOutputKey = "TrigT2CaloFEBJet"

        self.T2JetTools = [t2calojetgridfromfebs,
                            t2calojetconetool,
                            t2calojetcalibtool]
        



