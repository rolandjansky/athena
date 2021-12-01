# 
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags 
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from AthenaCommon.CFElements import parOR, seqAND
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorInitialROITool
from TrigEDMConfig.TriggerEDMRun3 import recordable
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool

def AFPTrkRecoBaseSequence(ConfigFlags):

    # Create inputs using input maker
    AFPInputMaker = InputMakerForRoI("IM_AFPTrackingFS")
    AFPInputMaker.RoITool = ViewCreatorInitialROITool()
    AFPInputMaker.RoIs = "AFPRoIs"


    from AFP_ByteStream2RawCnv.AFP_ByteStream2RawCnvConf import AFP_RawDataProvider
    AFP_Raw = AFP_RawDataProvider()

    from AFP_Raw2Digi.AFP_Raw2DigiConf import AFP_Raw2Digi
    AFP_R2D = AFP_Raw2Digi()
    
    # SiD part
    # setup clustering tool
    from AthenaConfiguration.ComponentFactory import CompFactory
    clusterNeighbour = CompFactory.getComp("AFPSiClusterBasicNearestNeighbour")("AFPSiClusterBasicNearestNeighbour")
    rowColToLocal = CompFactory.getComp("AFPSiRowColToLocalCSTool")("AFPSiRowColToLocalCSTool")
    clusterTool = CompFactory.getComp("AFPSiClusterTool")("AFPSiClusterTool", clusterAlgTool=clusterNeighbour, rowColToLocalCSTool = rowColToLocal)
    from AFP_SiClusterTools.AFP_SiClusterToolsConf import AFPSiCluster
    AFP_SiCl = AFPSiCluster("AFPSiCluster", clusterRecoTool = clusterTool)
    
    # setup track reconstruction algorithm tools
    kalmanTool0 = CompFactory.getComp("AFPSiDBasicKalmanTool")("AFPSiDBasicKalmanTool0", stationID=0, tracksContainerName = recordable("HLT_AFPTrackContainer"))
    kalmanTool1 = CompFactory.getComp("AFPSiDBasicKalmanTool")("AFPSiDBasicKalmanTool1", stationID=1, tracksContainerName = recordable("HLT_AFPTrackContainer"))
    kalmanTool2 = CompFactory.getComp("AFPSiDBasicKalmanTool")("AFPSiDBasicKalmanTool2", stationID=2, tracksContainerName = recordable("HLT_AFPTrackContainer"))
    kalmanTool3 = CompFactory.getComp("AFPSiDBasicKalmanTool")("AFPSiDBasicKalmanTool3", stationID=3, tracksContainerName = recordable("HLT_AFPTrackContainer"))
    kalmanToolsList=[kalmanTool0, kalmanTool1, kalmanTool2, kalmanTool3]
    # collect all output names and make a list with unique names for write handle keys; if this goes wrong AFP_SIDLocRecoTool::initialize() will complain
    outputKalmanList=[]
    for kalmanTool in kalmanToolsList:
        try:
            # in case the "tracksContainerName" is set
            contName=getattr(kalmanTool, "tracksContainerName")
        except AttributeError:
            # in case the "tracksContainerName" is not set
            contName=kalmanTool.getDefaultProperty("tracksContainerName")
        if contName not in outputKalmanList:
            outputKalmanList.append(contName)
    trackRecoTool = CompFactory.getComp("AFP_SIDLocRecoTool")("AFP_SIDLocRecoTool", RecoToolsList=kalmanToolsList, AFPTrackContainerList=outputKalmanList )
    from AFP_LocReco.AFP_LocRecoConf import AFP_SIDLocReco
    AFP_SID = AFP_SIDLocReco("AFP_SIDLocReco", recoTool = trackRecoTool)
    
    
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    
    monTool_AFP_SiClusterTool = GenericMonitoringTool("MonTool_AFP_SiClusterTool")
    monTool_AFP_SiClusterTool.defineHistogram( 'HitsSize', path='EXPERT', type='TH1F', title='SID hits size',xbins=50, xmin=0, xmax=50 )
    monTool_AFP_SiClusterTool.defineHistogram( 'ClusterSize', path='EXPERT', type='TH1F', title='SID cluster size',xbins=50, xmin=0, xmax=50 )
    AFP_SiCl.clusterRecoTool.MonTool = monTool_AFP_SiClusterTool
    
    
    monTool_AFP_SIDLocRecoTool = GenericMonitoringTool("MonTool_AFP_SIDLocRecoTool")
    monTool_AFP_SIDLocRecoTool.defineHistogram( 'TrkSize', path='EXPERT', type='TH1F', title='AFP tracks size',xbins=50, xmin=0, xmax=100 )
    AFP_SID.recoTool.MonTool = monTool_AFP_SIDLocRecoTool
    
    for i, kalmanTool in enumerate(kalmanToolsList):
        monTool_AFP_BasicKalman = GenericMonitoringTool("MonTool_AFP_"+kalmanTool.name())
    
        monTool_AFP_BasicKalman.defineHistogram( 'TrkStationID', path='EXPERT', type='TH1F', title='Track station ID',xbins=4, xmin=0, xmax=4 )
        monTool_AFP_BasicKalman.defineHistogram( 'TrkXLocal', path='EXPERT', type='TH1F', title='Track xLocal',xbins=100, xmin=-200, xmax=200 )
        monTool_AFP_BasicKalman.defineHistogram( 'TrkYLocal', path='EXPERT', type='TH1F', title='Track yLocal',xbins=100, xmin=-30, xmax=30 )
        monTool_AFP_BasicKalman.defineHistogram( 'TrkZLocal', path='EXPERT', type='TH1F', title='Track zLocal',xbins=100, xmin=-500000, xmax=500000 )
        monTool_AFP_BasicKalman.defineHistogram( 'TrkXSlope', path='EXPERT', type='TH1F', title='Track x slope',xbins=100, xmin=-0.5, xmax=0.5 )
        monTool_AFP_BasicKalman.defineHistogram( 'TrkYSlope', path='EXPERT', type='TH1F', title='Track y slope',xbins=100, xmin=-0.5, xmax=0.5 )
        monTool_AFP_BasicKalman.defineHistogram( 'TrkNClusters', path='EXPERT', type='TH1F', title='Track number of clusters',xbins=100, xmin=0, xmax=100 )
        monTool_AFP_BasicKalman.defineHistogram( 'TrkNHoles', path='EXPERT', type='TH1F', title='Track number of holes',xbins=100, xmin=0, xmax=100 )
        monTool_AFP_BasicKalman.defineHistogram( 'TrkChi2', path='EXPERT', type='TH1F', title='Track chi2',xbins=50, xmin=0, xmax=10 )

        AFP_SID.recoTool.RecoToolsList[i].MonTool = monTool_AFP_BasicKalman
    
    
    # TID part
    BarWeight = [1.0, 1.0, 1.0, 1.0] 
    TimeOffset0 = [105707, 105670, 105675, 105638,
                   105834, 105819, 105778, 105754,
                   105900, 105892, 105870, 105820,
                   105726, 105720, 105714, 105717]
    basicTool0 = CompFactory.getComp("AFPTDBasicTool")("AFPTDBasicTool0", stationID=0, maxAllowedLength=100, TimeOffset=TimeOffset0, BarWeight=BarWeight)
    TimeOffset3 = [105796, 105761, 105742, 105696,
                   105890, 105871, 105839, 105816,
                   105923, 105899, 105862, 105853,
                   105707, 105711, 105714, 105713]
    basicTool3 = CompFactory.getComp("AFPTDBasicTool")("AFPTDBasicTool3", stationID=3, maxAllowedLength=100, TimeOffset=TimeOffset3, BarWeight=BarWeight)
    basicToolsList=[basicTool0, basicTool3]
    # collect all output names and make a list with unique names for write handle keys; if this goes wrong AFP_SIDLocRecoTool::initialize() will complain
    outputBasicList=[]
    for basicTool in basicToolsList:
        try:
            # in case the "tracksContainerName" is set
            contName=getattr(basicTool, "tracksContainerName")
        except AttributeError:
            # in case the "tracksContainerName" is not set
            contName=basicTool.getDefaultProperty("tracksContainerName")
        if contName not in outputBasicList:
            outputBasicList.append(contName)
    ToFtrackRecoTool = CompFactory.getComp("AFP_TDLocRecoTool")("AFP_TDLocRecoTool", RecoToolsList=basicToolsList, AFPToFTrackContainerList=outputBasicList )
    from AFP_LocReco.AFP_LocRecoConf import AFP_TDLocReco
    AFP_TD = AFP_TDLocReco("AFP_TDLocReco", recoTool=ToFtrackRecoTool)

    # AFP_Proton part to be added here

    AFPRecoSeq = parOR("AFPRecoSeq", [AFP_Raw, AFP_R2D, AFP_SiCl, AFP_SID, AFP_TD])    

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
