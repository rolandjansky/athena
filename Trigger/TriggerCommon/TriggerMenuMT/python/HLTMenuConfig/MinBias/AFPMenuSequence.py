# 
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags 
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from AthenaCommon.CFElements import parOR, seqAND
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorInitialROITool
from AthenaCommon.GlobalFlags import globalflags
from TrigEDMConfig.TriggerEDMRun3 import recordable
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool

def AFPTrkRecoBaseSequence(ConfigFlags):

    # Create inputs using input maker
    AFPInputMaker = InputMakerForRoI("IM_AFPTrackingFS")
    AFPInputMaker.RoITool = ViewCreatorInitialROITool()
    AFPInputMaker.RoIs = "AFPRoIs"

    # get AFP_GeometryTool 
    from AthenaConfiguration.ComponentFactory import CompFactory
    AFP_Geometry_tool = CompFactory.getComp("AFP_GeometryTool")("AFP_Geometry_tool")

    from AFP_ByteStream2RawCnv.AFP_ByteStream2RawCnvConf import AFP_RawDataProvider
    AFP_Raw = AFP_RawDataProvider()

    from AFP_Raw2Digi.AFP_Raw2DigiConf import AFP_Raw2Digi
    AFP_R2D = AFP_Raw2Digi()

    # select between a real data or a simulation mode - Simulation = 0, RealData = 1
    if globalflags.DataSource()=='data':
        AFP_SIDLocReco_DataType = 1
    else:
        AFP_SIDLocReco_DataType = 0

    AFP_SIDLocReco_ListAlgoSID = ["SIDBasicKalman"]
    AFP_SIDLocReco_AmpThresh = 5000.
    
    # SID part
    from AFP_LocReco.AFP_LocRecoConf import AFP_SIDLocReco
    AFP_SID = AFP_SIDLocReco("AFP_SIDLocReco",
                            AFP_Geometry = AFP_Geometry_tool,
                            vecListAlgoSID = AFP_SIDLocReco_ListAlgoSID,
                            DataType = AFP_SIDLocReco_DataType,
                            AmpThresh = AFP_SIDLocReco_AmpThresh, 
                            AFPTrackContainerKey = recordable('HLT_AFPTrackContainer'))
    
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool("MonTool_AFP_SIDLocReco")

    monTool.defineHistogram( 'HitsSize', path='EXPERT', type='TH1F', title='SID hits size',xbins=50, xmin=0, xmax=50 )

    monTool.defineHistogram( 'TrkSize', path='EXPERT', type='TH1F', title='AFP tracks size',xbins=50, xmin=0, xmax=100 )
    
    monTool.defineHistogram( 'TrkStationID', path='EXPERT', type='TH1F', title='Track station ID',xbins=4, xmin=0, xmax=4 )
    monTool.defineHistogram( 'TrkXLocal', path='EXPERT', type='TH1F', title='Track xLocal',xbins=100, xmin=-50, xmax=50 )
    monTool.defineHistogram( 'TrkYLocal', path='EXPERT', type='TH1F', title='Track yLocal',xbins=100, xmin=-30, xmax=30 )
    monTool.defineHistogram( 'TrkZLocal', path='EXPERT', type='TH1F', title='Track zLocal',xbins=100, xmin=-50, xmax=50 )
    
    monTool.defineHistogram( 'TrkXSlope', path='EXPERT', type='TH1F', title='Track x slope',xbins=100, xmin=-0.5, xmax=0.5 )
    monTool.defineHistogram( 'TrkYSlope', path='EXPERT', type='TH1F', title='Track y slope',xbins=100, xmin=-0.5, xmax=0.5 )
    monTool.defineHistogram( 'TrkNHoles', path='EXPERT', type='TH1F', title='Track number of holes',xbins=0, xmin=0, xmax=200 )
    monTool.defineHistogram( 'TrkChi2', path='EXPERT', type='TH1F', title='Track chi2',xbins=50, xmin=0, xmax=10 )

    AFP_SID.MonTool = monTool
    
    # TID part
    # from AFP_LocReco.AFP_LocRecoConf import AFP_TDLocReco
    # AFP_TD = AFP_TDLocReco("AFP_TDLocReco")

    # if globalflags.DataSource()=='data':
    #     AFP_TDLocReco.DataType = 1
    # else:
    #     AFP_TDLocReco.DataType = 0

    # # select algo for timing reco and parameters
    # AFP_TDLocReco.ListAlgoTD = ["TDZVertex"]
    # AFP_TDLocReco.AmpThresh = 10.

    AFPRecoSeq = parOR("AFPRecoSeq", [AFP_Raw, AFP_R2D, AFP_SID])    

    return (AFPRecoSeq, AFPInputMaker)


def AFPTrkRecoSequence():

    # Retrieve input maker and reco seq
    (AFPPassThroughRecoSeq, AFPPassThroughInputMaker) = RecoFragmentsPool.retrieve(AFPTrkRecoBaseSequence,ConfigFlags)
    AFPPassThroughSequence = seqAND("AFPPassThroughSequence", [AFPPassThroughInputMaker, AFPPassThroughRecoSeq])

    from AthenaConfiguration.ComponentFactory import CompFactory
    # Pass-through hypo
    def trigStreamerAFPHypoTool(chain_dict):
        return conf2toConfigurable(CompFactory.TrigStreamerHypoTool(chain_dict["chainName"]))
    
    hypo = conf2toConfigurable(CompFactory.TrigStreamerHypoAlg("AFPPassThroughHypo"))
    trigHypoToolGen = trigStreamerAFPHypoTool

    return MenuSequence(Sequence = AFPPassThroughSequence,
                        Maker    = AFPPassThroughInputMaker,
                        Hypo     = hypo,
                        HypoToolGen = trigHypoToolGen)

def TestTrigAFPDijetHypoToolGen(chainDict):
    from TrigAFPHypo.TrigAFPHypoConf import TestTrigAFPDijetHypoTool
    hypo = TestTrigAFPDijetHypoTool(chainDict["chainName"])

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool("MonTool_AFP_Hypo")
    monTool.defineHistogram('DijetMass', type='TH1F', path='EXPERT', title="Dijet mass", xbins=100, xmin=0, xmax=2000)
    monTool.defineHistogram('DijetRapidity', type='TH1F', path='EXPERT', title="Dijet rapidity", xbins=100, xmin=-5, xmax=5)

    monTool.defineHistogram('XiJet1', type='TH1F', path='EXPERT', title="Jet 1 xi", xbins=100, xmin=0, xmax=1)
    monTool.defineHistogram('XiJet2', type='TH1F', path='EXPERT', title="Jet 2 x1", xbins=100, xmin=0, xmax=1)

    monTool.defineHistogram('PredictProtonAEnergy', type='TH1F', path='EXPERT', title="Predicted proton energy A", xbins=100, xmin=0, xmax=10000)
    monTool.defineHistogram('PredictProtonCEnergy', type='TH1F', path='EXPERT', title="Predicted proton energy C", xbins=100, xmin=0, xmax=10000)

    monTool.defineHistogram('SideA_predictX', type='TH1F', path='EXPERT', title="Predicted X side A", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideA_predictY', type='TH1F', path='EXPERT', title="Predicted Y side A", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideC_predictX', type='TH1F', path='EXPERT', title="Predicted X side C", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideC_predictY', type='TH1F', path='EXPERT', title="Predicted Y side C", xbins=100, xmin=-100, xmax=100)

    monTool.defineHistogram('XDiff', type='TH1F', path='EXPERT', title="X difference", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('YDiff', type='TH1F', path='EXPERT', title="Y difference", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('distance', type='TH1F', path='EXPERT', title="distance", xbins=100, xmin=0, xmax=50)
    
    monTool.defineHistogram('SideA_trackX', type='TH1F', path='EXPERT', title="Track X side A", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideA_trackY', type='TH1F', path='EXPERT', title="Track Y side A", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideA_diffX', type='TH1F', path='EXPERT', title="Track X diff side A", xbins=100, xmin=-50, xmax=50)
    monTool.defineHistogram('SideA_diffY', type='TH1F', path='EXPERT', title="Track Y diff side A", xbins=100, xmin=-50, xmax=50)
    
    monTool.defineHistogram('SideC_trackX', type='TH1F', path='EXPERT', title="Track X side C", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideC_trackY', type='TH1F', path='EXPERT', title="Track Y side C", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideC_diffX', type='TH1F', path='EXPERT', title="Track X diff side C", xbins=100, xmin=-50, xmax=50)
    monTool.defineHistogram('SideC_diffY', type='TH1F', path='EXPERT', title="Track Y diff side C", xbins=100, xmin=-50, xmax=50)
    
    #monToolHypo.HistPath = 'AFPComboHypo/'+tool.getName()
    hypo.MonTool = monTool

    return hypo

def AFPTrkRecoHypoSequence():

    (AFPRecoSeqHypo, AFPInputMakerHypo) = RecoFragmentsPool.retrieve(AFPTrkRecoBaseSequence,ConfigFlags)
    AFPSequenceHypo = seqAND("AFPSequenceHypo", [AFPInputMakerHypo, AFPRecoSeqHypo])
    
    # Hypo
    from TrigAFPHypo.TrigAFPHypoConf import TestTrigAFPDijetHypoAlg
    hypoAlg = TestTrigAFPDijetHypoAlg("TestTrigAFPDijetHypoAlg",
                                   AFPTrackContainer = 'HLT_AFPTrackContainer',
                                   InputJetCollection = 'HLT_AntiKt4EMTopoJets_nojcalib')
    

    return MenuSequence(Sequence = AFPSequenceHypo,
                        Maker    = AFPInputMakerHypo,
                        Hypo     = hypoAlg,
                        HypoToolGen = TestTrigAFPDijetHypoToolGen)
