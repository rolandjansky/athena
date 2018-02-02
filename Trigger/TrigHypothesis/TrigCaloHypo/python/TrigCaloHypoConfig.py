# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.SystemOfUnits import GeV
from TrigCaloHypo.TrigCaloHypoConf import TrigEFCaloHypoNoise
from LArCellRec.LArCellRecConf import LArNoisyROTool
from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
from IOVDbSvc.CondDB import conddb
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

class EFCaloHypoNoiseConfig (TrigEFCaloHypoNoise):
    __slots__ = []
    def __init__(self, name = "EFCaloHypoNoiseConfig",ef_thr=20*GeV,etaMin=0,etaMax=10):
        super( EFCaloHypoNoiseConfig, self ).__init__( name )

        self.Etcut = ef_thr
        self.BadFEBCut=3
        if 'COMP200' not in conddb.GetInstance() and not conddb.isMC:
           if not hasattr(svcMgr.ToolSvc, "KnownBADFEBsTool"):
              theBadFebTool=LArBadChanTool("KnownBADFEBsTool")
              theBadFebTool.CoolMissingFEBsFolder="/LAR/BadChannels/KnownBADFEBs"
              havefolder=False
              for fld in conddb.iovdbsvc.Folders:
                 if "KnownBADFEBs" in fld: havefolder=True
              pass
              if not havefolder:
                 conddb.addFolder("LAR_ONL","/LAR/BadChannels/KnownBADFEBs")   
              svcMgr.ToolSvc+=theBadFebTool
           else:
              theBadFebTool=svcMgr.ToolSvc.KnownBADFEBsTool
           if not hasattr(svcMgr.ToolSvc, "KnownMNBFEBsTool"):
              theMNBFebTool=LArBadChanTool("KnownMNBFEBsTool")
              theMNBFebTool.CoolMissingFEBsFolder="/LAR/BadChannels/KnownMNBFEBs"
              havefolder=False
              for fld in conddb.iovdbsvc.Folders:
                 if "KnownMNBFEBs" in fld: havefolder=True
              pass
              if not havefolder:
                 conddb.addFolder("LAR_ONL","/LAR/BadChannels/KnownMNBFEBs")   
              svcMgr.ToolSvc+=theMNBFebTool
           else:
              theMNBFebTool=svcMgr.ToolSvc.KnownMNBFEBsTool
           theLArNoisyROTool=LArNoisyROTool(SaturatedCellTightCut=20,MNBLooseCut=5,MNBTightCut=17,KnownBADFEBsTool=theBadFebTool,KnownMNBFEBsTool=theMNBFebTool)
        else:   
           theLArNoisyROTool=LArNoisyROTool(SaturatedCellTightCut=20,MNBLooseCut=5,MNBTightCut=17)
        self.NoiseTool = theLArNoisyROTool

from TrigCaloHypo.TrigCaloHypoConf import TrigL2JetHypo

class L2JetHypoBase (TrigL2JetHypo):
    __slots__ = []
    def __init__(self, name):
        super( L2JetHypoBase, self ).__init__( name )
        
        #validation = TrigL2JetHypoValidationMonitoring()
        #online = TrigL2JetHypoOnlineMonitoring()
        #cosmic = TrigL2JetHypoCosmicMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("L2JetHypo_Time")
        
        self.AthenaMonTools = [ time ]
        
        # switch on basic cleaning by default for all chains
        self.n90CleaningThreshold = 2
        self.presamplerCleaningThreshold = 0.9

class L2JetHypo (L2JetHypoBase):
    __slots__ = []
    def __init__(self, name = "L2JetHypo",l2_thr=20*GeV):
        super( L2JetHypo, self ).__init__( name )

        self.Etcut_L2 = l2_thr
        self.doHECfnLeadingCellsCleaning = False
        self.doEMfJetQualityCleaning = False
        self.doHECfJetQualityCleaning = False
        self.doTimeQualityCleaning = False
        self.doEMfCleaningHighEta = False
        self.doEMfCleaningLowEta = False
