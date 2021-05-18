# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from LArCellRec.LArCellRecConf import LArNoisyROTool
from IOVDbSvc.CondDB import conddb

from LArCabling.LArCablingAccess import LArOnOffIdMapping
LArOnOffIdMapping()

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
         self.HypoTools = [TrigLArNoiseBurstHypoToolIncCfg()]

def TrigLArNoiseBurstHypoToolGen(chainDict):
     return TrigLArNoiseBurstHypoToolIncCfg(chainDict['chainName'])
