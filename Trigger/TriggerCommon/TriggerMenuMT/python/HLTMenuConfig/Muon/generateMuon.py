# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CAMenuSequence, ChainStep, Chain, createStepView
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from TrigL2MuonSA.TrigL2MuonSAConfig_newJO import l2MuFastAlgCfg, l2MuFastHypoCfg
from TrigmuComb.TrigmuCombConfig_newJO import l2MuCombRecoCfg, l2MuCombHypoCfg
from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoToolFromDict, TrigmuCombHypoToolFromDict, TrigMuonEFMSonlyHypoToolFromDict
from TrigInDetConfig.TrigInDetConfig import TrigInDetConfig

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict

from AthenaConfiguration.ComponentFactory import CompFactory

import pprint
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Muon.generateMuon' )

def fakeHypoAlgCfg(flags, name="FakeHypoForMuon"):
    HLTTest__TestHypoAlg=CompFactory.HLTTest.TestHypoAlg
    return HLTTest__TestHypoAlg( name, Input="" )

def EFMuonViewDataVerifierCfg():
    EFMuonViewDataVerifier =  CompFactory.AthViews.ViewDataVerifier("VDVEFMuon")
    EFMuonViewDataVerifier.DataObjects = [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                                          ( 'Muon::MdtPrepDataContainer' , 'StoreGateSvc+MDT_DriftCircles' ),
                                          ( 'Muon::TgcPrepDataContainer' , 'StoreGateSvc+TGC_Measurements' ),
                                          ( 'Muon::RpcPrepDataContainer' , 'StoreGateSvc+RPC_Measurements' ),
                                          ( 'Muon::CscStripPrepDataContainer' , 'StoreGateSvc+CSC_Measurements' ),
                                          ( 'Muon::CscPrepDataContainer' , 'StoreGateSvc+CSC_Clusters' ),
                                      ]
    result = ComponentAccumulator()
    result.addEventAlgo(EFMuonViewDataVerifier)
    return result

def MuFastViewDataVerifier():
    result = ComponentAccumulator()
    alg = CompFactory.AthViews.ViewDataVerifier( name = "VDVMuFast",
                                                 DataObjects = [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                                                                ( 'RpcPad_Cache' , 'StoreGateSvc+RpcRdoCache' ),
                                                                ( 'RpcCoinDataCollection_Cache' , 'StoreGateSvc+RpcCoinCache' ),
                                                                ( 'RpcPrepDataCollection_Cache' , 'StoreGateSvc+RpcPrdCache' ),
                                                                ( 'TgcRdo_Cache' , 'StoreGateSvc+TgcRdoCache' ),
                                                                ( 'MdtCsm_Cache' , 'StoreGateSvc+MdtCsmRdoCache' ),
                                                                ( 'CscRawDataCollection_Cache' , 'StoreGateSvc+CscRdoCache' ),
                                                                ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+L2MuFastRecoRoIs' ),
                                                                ( 'DataVector< LVL1::RecMuonRoI >' , 'StoreGateSvc+HLT_RecMURoIs' )
                                                               ]  )
    result.addEventAlgo(alg)
    return result

def MuCombViewDataVerifier():
    result = ComponentAccumulator()
    alg = CompFactory.AthViews.ViewDataVerifier( name = "VDVMuComb",
                                                 DataObjects = [( 'xAOD::L2StandAloneMuonContainer' , 'StoreGateSvc+MuonL2SAInfo' )])
    result.addEventAlgo(alg)
    return result

#Not the ideal place to keep the track cnv alg configuration. Temproarily adding it here 
#until a better location can be found
def MuonTrackCollectionCnvToolCfg(flags, name = "MuonTrackCollectionCnvTool", **kwargs):
    TrackCollectionCnvTool = CompFactory.xAODMaker.TrackCollectionCnvTool

    result = ComponentAccumulator()
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonCombinedParticleCreatorCfg
    acc = MuonCombinedParticleCreatorCfg(flags)
    kwargs.setdefault("TrackParticleCreator",  acc.popPrivateTools())
    result.merge(acc)

    result.setPrivateTools(TrackCollectionCnvTool(name=name, **kwargs))
    return result

def MuonRecTrackParticleContainerCnvToolCfg(flags, name = "MuonRecTrackParticleContainerCnvTool", **kwargs):
    RecTrackParticleCnvTool = CompFactory.xAODMaker.RecTrackParticleContainerCnvTool

    result = ComponentAccumulator()
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonCombinedParticleCreatorCfg
    acc = MuonCombinedParticleCreatorCfg(flags)
    kwargs.setdefault("TrackParticleCreator",  acc.popPrivateTools())
    result.merge(acc)

    result.setPrivateTools(RecTrackParticleCnvTool(name=name, **kwargs))
    return result

def MuonTrackParticleCnvCfg(flags, name = "MuonTrackParticleCnvAlg",**kwargs):
    TrackParticleCnv = CompFactory.xAODMaker.TrackParticleCnvAlg
    result=ComponentAccumulator()

    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonCombinedParticleCreatorCfg
    acc = MuonCombinedParticleCreatorCfg(flags)
    kwargs.setdefault("TrackParticleCreator", acc.popPrivateTools())
    result.merge(acc)

    acc = MuonTrackCollectionCnvToolCfg(flags)
    kwargs.setdefault("TrackCollectionCnvTool", acc.popPrivateTools())
    result.merge(acc)

    acc = MuonRecTrackParticleContainerCnvToolCfg(flags)
    kwargs.setdefault("RecTrackParticleContainerCnvTool", acc.popPrivateTools())
    result.merge(acc)

    kwargs.setdefault("TrackContainerName", "MuonSpectrometerTracks")
    kwargs.setdefault("xAODTrackParticlesFromTracksContainerName", "MuonSpectrometerTrackParticles")
    kwargs.setdefault("AODContainerName", "")
    kwargs.setdefault("AODTruthContainerName", "")
    kwargs.setdefault("xAODTruthLinkVector",  "")
    kwargs.setdefault("ConvertTrackParticles", False)
    kwargs.setdefault("ConvertTracks", True)

    trackcnv = TrackParticleCnv(name=name, **kwargs )

    result.addEventAlgo( trackcnv, primary=True )
    return result

def efMuMSHypoCfg(flags, name="UNSPECIFIED", inputMuons="UNSPECIFIED"):
    TrigMuonEFHypoAlg = CompFactory.TrigMuonEFHypoAlg
    efMSHypo = TrigMuonEFHypoAlg(name)
    efMSHypo.MuonDecisions = inputMuons
    return efMSHypo


def generateChains( flags, chainDict ):
    chainDict = splitChainDict(chainDict)[0]
    
    # Step 1 (L2MuonSA)
    stepName = 'L2MuonSA'
    stepReco, stepView = createStepView(stepName)

    acc = ComponentAccumulator()
    acc.addSequence(stepView)

    # Set EventViews for L2MuonSA step
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco
    reco = InViewReco("L2MuFastReco")

    #external data loading to view
    reco.mergeReco( MuFastViewDataVerifier() )


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

    l2MuFastAlgAcc = ComponentAccumulator()
    l2MuFastAlgAcc.addEventAlgo(alg)
    
    reco.mergeReco( l2MuFastAlgAcc )
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

    l2muFastStep = ChainStep( name=stepName, Sequences=[l2muFastSequence], chainDicts=[chainDict] )

    if 'msonly' not in chainDict['chainName']: 
        #only run in combined muon chains
        ### Set muon step2 - L2muComb ###
        stepL2CBName = 'L2MuonCB'
        stepL2CBReco, stepL2CBView = createStepView(stepL2CBName)

        accL2CB = ComponentAccumulator()
        accL2CB.addSequence(stepL2CBView)
        
        # Set EventViews for L2MuonCB step
        recoL2CB = l2MuCombRecoCfg(flags)
        #external data loading to view
        recoL2CB.inputMaker().RequireParentView = True
        recoL2CB.mergeReco( MuCombViewDataVerifier() )

        #ID tracking
        accID = TrigInDetConfig( flags, roisKey=recoL2CB.inputMaker().InViewRoIs, signatureName="Muon" )
        recoL2CB.mergeReco(accID)
        
        accL2CB.merge(recoL2CB, sequenceName = stepL2CBReco.getName())

        l2muCombHypo = l2MuCombHypoCfg( flags,
                                        name = 'TrigL2MuCombHypo',
                                        muCombInfo = 'HLT_MuonL2CBInfo' )

        accL2CB.addEventAlgo(l2muCombHypo, sequenceName=stepL2CBView.getName())

        l2muCombSequence = CAMenuSequence( Sequence = recoL2CB.sequence(),
                                           Maker = recoL2CB.inputMaker(),
                                           Hypo = l2muCombHypo,
                                           HypoToolGen = TrigmuCombHypoToolFromDict,
                                           CA = accL2CB )

        l2muCombStep = ChainStep( name=stepL2CBName, Sequences=[l2muCombSequence], chainDicts=[chainDict] )
    

    if 'msonly' in chainDict['chainName']: 
        #only runningn in MS-only chains for now
        #EF MS only
        stepEFMSName = 'EFMSMuon'
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
    
        #Probably this block will eventually need to move somewhere more central
        from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
        accMS.merge( BeamPipeGeometryCfg(flags) ) 
        
        from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
        accMS.merge(PixelGeometryCfg(flags))
    
        from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
        accMS.merge(SCT_GeometryCfg(flags))
        
        from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
        accMS.merge(TRT_GeometryCfg(flags))
    
        from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
        accMS.merge(TrackingGeometrySvcCfg(flags))
        ###################
    
        EFMuonViewDataVerifier = EFMuonViewDataVerifierCfg()
        recoMS.mergeReco(EFMuonViewDataVerifier)

        from MuonConfig.MuonSegmentFindingConfig import MooSegmentFinderAlgCfg
        segCfg = MooSegmentFinderAlgCfg(muonflags,name="TrigMooSegmentFinder",UseTGCNextBC=False, UseTGCPriorBC=False)
        recoMS.mergeReco(segCfg)

        from MuonConfig.MuonTrackBuildingConfig import MuonTrackBuildingCfg
        trkCfg = MuonTrackBuildingCfg(muonflags, name="TrigMuPatTrackBuilder")
        recoMS.mergeReco(trkCfg)

        cnvCfg = MuonTrackParticleCnvCfg(muonflags, name = "TrigMuonTrackParticleCnvAlg")
        recoMS.mergeReco(cnvCfg)

        from MuonCombinedConfig.MuonCombinedReconstructionConfig import MuonCombinedMuonCandidateAlgCfg
        candCfg = MuonCombinedMuonCandidateAlgCfg(muonflags, name = "TrigMuonCandidateAlg")
        recoMS.mergeReco(candCfg)

        from MuonCombinedConfig.MuonCombinedReconstructionConfig import MuonCreatorAlgCfg
        creatorCfg = MuonCreatorAlgCfg(muonflags, name = "TrigMuonCreatorAlg")
        recoMS.mergeReco(creatorCfg)

        accMS.merge(recoMS, sequenceName=stepEFMSReco.getName())

        efmuMSHypo = efMuMSHypoCfg( muonflags,
                                    name = 'TrigMuonEFMSonlyHypo',
                                    inputMuons = "Muons" )

        accMS.addEventAlgo(efmuMSHypo, sequenceName=stepEFMSView.getName())

        efmuMSSequence = CAMenuSequence( Sequence = recoMS.sequence(),
                                         Maker = recoMS.inputMaker(),
                                         Hypo = efmuMSHypo, 
                                         HypoToolGen = TrigMuonEFMSonlyHypoToolFromDict,
                                         CA = accMS )

        efmuMSStep = ChainStep( name=stepEFMSName, Sequences=[efmuMSSequence], chainDicts=[chainDict] )

    l1Thresholds=[]
    for part in chainDict['chainParts']:
        l1Thresholds.append(part['L1threshold'])
    
    log.debug('dictionary is: %s\n', pprint.pformat(chainDict))

    if 'msonly' in chainDict['chainName']: 
        chain = Chain( name=chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[ l2muFastStep, efmuMSStep ] )
    else:
        chain = Chain( name=chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[ l2muFastStep, l2muCombStep ] )
    return chain

