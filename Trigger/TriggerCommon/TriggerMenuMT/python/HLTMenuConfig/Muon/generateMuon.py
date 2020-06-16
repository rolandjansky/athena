# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CAMenuSequence, ChainStep, Chain, getChainStepName, createStepView
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from TrigL2MuonSA.TrigL2MuonSAConfig_newJO import l2MuFastAlgCfg, l2MuFastHypoCfg
from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoToolFromDict

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict

from AthenaConfiguration.ComponentFactory import CompFactory

import pprint
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Muon.generateMuon' )

def fakeHypoAlgCfg(flags, name="FakeHypoForMuon"):
    HLTTest__TestHypoAlg=CompFactory.HLTTest.TestHypoAlg
    return HLTTest__TestHypoAlg( name, Input="" )

def generateChains( flags, chainDict ):
    chainDict = splitChainDict(chainDict)[0]
    
    # Step 1 (L2MuonSA)
    stepName = getChainStepName('Muon', 1)
    stepReco, stepView = createStepView(stepName)

    acc = ComponentAccumulator()
    acc.addSequence(stepView)

    # Set EventViews for L2MuonSA step
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco
    reco = InViewReco("L2MuFastReco")



    # decoding
    # Get RPC BS decoder 
    from MuonConfig.MuonBytestreamDecodeConfig import RpcBytestreamDecodeCfg
    rpcAcc = RpcBytestreamDecodeCfg( flags, forTrigger=True )
    rpcAcc.getEventAlgo("RpcRawDataProvider").RoIs = reco.name+"RoIs"
    reco.mergeReco( rpcAcc )

    # Get RPC BS->RDO convertor
    from MuonConfig.MuonRdoDecodeConfig import RpcRDODecodeCfg    
    rpcAcc = RpcRDODecodeCfg( flags, forTrigger=True )
    rpcAcc.getEventAlgo("RpcRdoToRpcPrepData").RoIs = reco.name+"RoIs"
    reco.mergeReco( rpcAcc )

    # Get TGC BS decoder 
    from MuonConfig.MuonBytestreamDecodeConfig import TgcBytestreamDecodeCfg
    tgcAcc = TgcBytestreamDecodeCfg( flags, forTrigger=True )
    tgcAcc.getEventAlgo("TgcRawDataProvider").RoIs = reco.name+"RoIs"
    reco.mergeReco( tgcAcc )

    # Get TGC BS->RDO convertor
    from MuonConfig.MuonRdoDecodeConfig import TgcRDODecodeCfg    
    tgcAcc = TgcRDODecodeCfg( flags, forTrigger=True )
    tgcAcc.getEventAlgo("TgcRdoToTgcPrepData").RoIs = reco.name+"RoIs"
    reco.mergeReco( tgcAcc )

    # Get MDT BS decoder 
    from MuonConfig.MuonBytestreamDecodeConfig import MdtBytestreamDecodeCfg
    mdtAcc = MdtBytestreamDecodeCfg( flags, forTrigger=True )
    mdtAcc.getEventAlgo("MdtRawDataProvider").RoIs = reco.name+"RoIs"
    reco.mergeReco( mdtAcc )

    # Get MDT BS->RDO convertor
    from MuonConfig.MuonRdoDecodeConfig import MdtRDODecodeCfg    
    mdtAcc = MdtRDODecodeCfg( flags, forTrigger=True )
    mdtAcc.getEventAlgo("MdtRdoToMdtPrepData").RoIs = reco.name+"RoIs"
    reco.mergeReco( mdtAcc )

    # Get CSC BS decoder 
    from MuonConfig.MuonBytestreamDecodeConfig import CscBytestreamDecodeCfg
    cscAcc = CscBytestreamDecodeCfg( flags, forTrigger=True )
    cscAcc.getEventAlgo("CscRawDataProvider").RoIs = reco.name+"RoIs"
    reco.mergeReco( cscAcc )

    # Get CSC BS->RDO convertor
    from MuonConfig.MuonRdoDecodeConfig import CscRDODecodeCfg    
    cscAcc = CscRDODecodeCfg( flags, forTrigger=True )
    cscAcc.getEventAlgo("CscRdoToCscPrepData").RoIs = reco.name+"RoIs"
    reco.mergeReco( cscAcc )

    # Get CSC cluster builder
    from MuonConfig.MuonRdoDecodeConfig import CscClusterBuildCfg
    cscAcc = CscClusterBuildCfg( flags, forTrigger=True )
    reco.mergeReco( cscAcc )



    # Get Reco alg of muFast Step in order to set into the view
    algAcc, alg = l2MuFastAlgCfg( flags, roisKey=reco.name+"RoIs")

    reco.addRecoAlg( alg )
    reco.merge( algAcc )
    #    l2muFastReco = l2MuFastRecoCfg(flags)
    acc.merge( reco, sequenceName=stepReco.getName() )

    ### Set muon step1 ###
    l2muFastHypo = l2MuFastHypoCfg( flags,
                                    name = 'TrigL2MuFastHypo',
                                    muFastInfo = 'MuonL2SAInfo' )

    acc.addEventAlgo(l2muFastHypo, sequenceName=stepView.getName())

    l2muFastSequence = CAMenuSequence( Sequence = reco.sequence(),
                                     Maker = reco.inputMaker(),
                                     Hypo = l2muFastHypo,
                                     HypoToolGen = TrigMufastHypoToolFromDict,
                                     CA = acc )

    l2muFastSequence.createHypoTools(chainDict)

    l2muFastStep = ChainStep( stepName, [l2muFastSequence] )

    ### Set muon step2 ###
    # Please set up L2muComb step here

    #EF MS only
    stepEFMSName = getChainStepName('EFMSMuon', 2)
    stepEFMSReco, stepEFMSView = createStepView(stepEFMSName)

    #Clone and replace offline flags so we can set muon trigger specific values
    muonflags = flags.cloneAndReplace('Muon', 'Trigger.Offline.Muon')
    muonflags.Muon.useTGCPriorNextBC=True
    muonflags.Muon.enableErrorTuning=False
    muonflags.Muon.MuonTrigger=True
    muonflags.Muon.SAMuonTrigger=True
    muonflags.lock()

    accMS = ComponentAccumulator()
    accMS.addSequence(stepEFMSView)

    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco
    recoMS = InViewReco("EFMuMSReco")
    recoMS.inputMaker().RequireParentView = True
    EFMuonViewDataVerifier =  CompFactory.getComp("AthViews::ViewDataVerifier")("EFMuonViewDataVerifier")
    EFMuonViewDataVerifier.DataObjects = [( 'Muon::MdtPrepDataContainer' , 'StoreGateSvc+MDT_DriftCircles' ),
                                          ( 'Muon::TgcPrepDataContainer' , 'StoreGateSvc+TGC_Measurements' ),
                                          ( 'Muon::RpcPrepDataContainer' , 'StoreGateSvc+RPC_Measurements' ),
                                          ( 'Muon::CscStripPrepDataContainer' , 'StoreGateSvc+CSC_Measurements' ),
                                          ( 'Muon::CscPrepDataContainer' , 'StoreGateSvc+CSC_Clusters' )]

    recoMS.addRecoAlg(EFMuonViewDataVerifier)

    from MuonConfig.MuonSegmentFindingConfig import MooSegmentFinderAlgCfg
    segCfg = MooSegmentFinderAlgCfg(muonflags,name="TrigMooSegmentFinder",UseTGCNextBC=False, UseTGCPriorBC=False)
    recoMS.mergeReco(segCfg)

    from MuonConfig.MuonTrackBuildingConfig import MuonTrackBuildingCfg
    trkCfg = MuonTrackBuildingCfg(muonflags, name="TrigMuPatTrackBuilder")
    recoMS.mergeReco(trkCfg)

    #The MuonCandidateAlg is not quite fully working yet
    #from MuonCombinedConfig.MuonCombinedReconstructionConfig import MuonCombinedMuonCandidateAlgCfg
    #candCfg = MuonCombinedMuonCandidateAlgCfg(muonflags, name = "TrigMuonCandidateAlg")
    #recoMS.mergeReco(candCfg)

    accMS.merge(recoMS, sequenceName=stepEFMSReco.getName())

    # TODO remove once full step is in place
    HLTTest__TestHypoTool=CompFactory.HLTTest.TestHypoTool
    fakeHypoAlg = fakeHypoAlgCfg(muonflags, name='FakeHypoForMuon')
    def makeFakeHypoTool(chainDict, cfg=None):
        return HLTTest__TestHypoTool(chainDict['chainName'])

    accMS.addEventAlgo(fakeHypoAlg, sequenceName=stepEFMSView.getName())

    efmuMSSequence = CAMenuSequence( Sequence = recoMS.sequence(),
                                     Maker = recoMS.inputMaker(),
                                     Hypo = fakeHypoAlg, 
                                     HypoToolGen = makeFakeHypoTool,
                                     CA = accMS )

    efmuMSSequence.createHypoTools(chainDict)

    efmuMSStep = ChainStep( stepEFMSName, [efmuMSSequence] )

    l1Thresholds=[]
    for part in chainDict['chainParts']:
        l1Thresholds.append(part['L1threshold'])
    
    log.debug('dictionary is: %s\n', pprint.pformat(chainDict))


    chain = Chain( name=chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[ l2muFastStep, efmuMSStep ] )
    return chain

