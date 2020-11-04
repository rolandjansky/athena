# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.SystemOfUnits import GeV
from TrigCaloHypo.TrigCaloHypoConf import TrigEFCaloHypoNoise
from LArCellRec.LArCellRecConf import LArNoisyROTool
from IOVDbSvc.CondDB import conddb

from LArCabling.LArCablingAccess import LArOnOffIdMapping
LArOnOffIdMapping()

class EFCaloHypoNoiseConfig (TrigEFCaloHypoNoise):
    __slots__ = []
    def __init__(self, name = "EFCaloHypoNoiseConfig",ef_thr=20*GeV,etaMin=0,etaMax=10):
        super( EFCaloHypoNoiseConfig, self ).__init__( name )

        self.Etcut = ef_thr
        self.BadFEBCut=3
        from LArBadChannelTool.LArBadChannelToolConf import LArBadFebCondAlg
        conddb.addFolder('LAR_ONL',"/LAR/BadChannels/KnownBADFEBs", className="AthenaAttributeList")
        conddb.addFolder('LAR_ONL',"/LAR/BadChannels/KnownMNBFEBs", className="AthenaAttributeList")
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        condSeq+=LArBadFebCondAlg("LArKnownBadFebAlg",ReadKey="/LAR/BadChannels/KnownBADFEBs",WriteKey="LArKnownBadFEBs")
        condSeq+=LArBadFebCondAlg("LArKnownMNBFebAlg",ReadKey="/LAR/BadChannels/KnownMNBFEBs",WriteKey="LArKnownMNBFEBs")
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

from AthenaConfiguration.ComponentFactory import CompFactory

class TrigLArNoiseBurstHypoToolIncCfg ( CompFactory.TrigLArNoiseBurstHypoToolInc ):
     def __init__(self,name="TrigLArNoiseBurstHypoToolIncCfg", **kwargs):
         super(TrigLArNoiseBurstHypoToolIncCfg,self).__init__(name,**kwargs)
         from LArBadChannelTool.LArBadChannelToolConf import LArBadFebCondAlg
         from AthenaCommon.AlgSequence import AthSequencer
         condSeq = AthSequencer("AthCondSeq")
         if ( not hasattr(condSeq,"LArKnownBadFebAlg") ):
            conddb.addFolder('LAR_ONL',"/LAR/BadChannels/KnownBADFEBs", className="AthenaAttributeList")
            condSeq+=LArBadFebCondAlg("LArKnownBadFebAlg",ReadKey="/LAR/BadChannels/KnownBADFEBs",WriteKey="LArKnownBadFEBs")
         if ( not hasattr(condSeq,"LArKnownMNBFebAlg") ):
            conddb.addFolder('LAR_ONL',"/LAR/BadChannels/KnownMNBFEBs", className="AthenaAttributeList")
            condSeq+=LArBadFebCondAlg("LArKnownMNBFebAlg",ReadKey="/LAR/BadChannels/KnownMNBFEBs",WriteKey="LArKnownMNBFEBs")
         theLArNoisyROTool=LArNoisyROTool(SaturatedCellTightCut=20,MNBLooseCut=5,MNBTightCut=17)
         self.NoiseTool = theLArNoisyROTool

class TrigLArNoiseBurstAlgCfg ( CompFactory.TrigLArNoiseBurstAlg ):
     def __init__(self,name="TrigLArNoiseBurstAlgCfg", **kwargs):
         super(TrigLArNoiseBurstAlgCfg,self).__init__(name,**kwargs)
         self.HypoTools = [TrigLArNoiseBurstHypoToolIncCfg()]

def TrigLArNoiseBurstHypoToolGen(chainDict):
     return TrigLArNoiseBurstHypoToolIncCfg(chainDict['chainName'])
