# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from LArCellRec.LArCellRecConf import LArNoisyROTool
from IOVDbSvc.CondDB import conddb

from LArCabling.LArCablingAccess import LArOnOffIdMapping
LArOnOffIdMapping()

from AthenaConfiguration.ComponentFactory import CompFactory

class TrigLArNoiseBurstHypoToolCfg ( CompFactory.TrigLArNoiseBurstHypoTool ):
     def __init__(self,name="TrigLArNoiseBurstHypoToolCfg", **kwargs):
         super(TrigLArNoiseBurstHypoToolCfg,self).__init__(name,**kwargs)
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
         from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool,defineHistogram
         monTool = GenericMonitoringTool()
         cutLabels = ["Input","BadFEBFlaggedPartitions", "BadFEB_WFlaggedPartitions", "SatTightFlaggedPartitions", "MNBLooseFlaggedPartions", "MNBTightFlaggedPartions", "Output" ]
         monTool.Histograms = [ defineHistogram('bitWise_flags', type='TH1I', path='EXPERT', title="LArNoiseBurst Cut Counter;Cut ;  Count", xbins=len(cutLabels), xmin=0, xmax=len(cutLabels), xlabels=cutLabels),
         defineHistogram('TIME_larnoisetool', type='TH1F', path='EXPERT', title="Time; time(ps)", xbins=100, xmin=-100.0,xmax=15000) ]
         self.MonTool = monTool
         self.NoiseTool = theLArNoisyROTool

class TrigLArNoiseBurstAlgCfg ( CompFactory.TrigLArNoiseBurstAlg ):
     def __init__(self,name="TrigLArNoiseBurstAlgCfg", **kwargs):
         super(TrigLArNoiseBurstAlgCfg,self).__init__(name,**kwargs)
         self.HypoTools = [TrigLArNoiseBurstHypoToolCfg()]

def TrigLArNoiseBurstHypoToolGen(chainDict):
     return TrigLArNoiseBurstHypoToolCfg(chainDict['chainName'])


class TrigL2CaloLayersHypo_PreS_092Cfg (CompFactory.TrigL2CaloLayersHypoTool):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super( TrigL2CaloLayersHypo_PreS_092Cfg, self ).__init__( name )
        self.EnergyFractionCut=[0.92,1.0,1.0,1.0]
        self.AcceptAll = False

class TrigL2CaloLayersHypo_PreS_080Cfg (CompFactory.TrigL2CaloLayersHypoTool):
    __slots__ = []
    def __init__(self, name="TrigL2CaloLayersHypo_PreS_080Cfg", **kwargs):
        super( TrigL2CaloLayersHypo_PreS_080Cfg, self ).__init__( name )
        self.EnergyFractionCut=[0.80,1.0,1.0,1.0]
        self.AcceptAll = False

class TrigL2CaloLayersAlgCfg ( CompFactory.TrigL2CaloLayersAlg ):
     def __init__(self,name="TrigL2CaloLayersCfg", **kwargs):
         super(TrigL2CaloLayersAlgCfg,self).__init__(name,**kwargs)
         self.HypoTools = [TrigL2CaloLayersHypo_PreS_092Cfg()]
         #self.HypoTools = [TrigL2CaloLayersHypo_PreS_092Cfg(),
         #                  TrigL2CaloLayersHypo_PreS_080Cfg()]

def TrigL2CaloLayersHypoToolGen(chainDict):
     return TrigL2CaloLayersHypo_PreS_080Cfg(chainDict['chainName'])

