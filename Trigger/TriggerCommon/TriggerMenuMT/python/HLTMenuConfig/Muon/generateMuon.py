# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CAMenuSequence, ChainStep, Chain, createStepView, EmptyMenuSequence
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from TrigL2MuonSA.TrigL2MuonSAConfig_newJO import l2MuFastAlgCfg, l2MuFastHypoCfg
from TrigmuComb.TrigmuCombConfig_newJO import l2MuCombRecoCfg, l2MuCombHypoCfg
from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoToolFromDict, TrigmuCombHypoToolFromDict, TrigMuonEFMSonlyHypoToolFromDict, TrigMuonEFCombinerHypoToolFromDict
from TrigInDetConfig.TrigInDetConfig import trigInDetFastTrackingCfg

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict

from AthenaConfiguration.ComponentFactory import CompFactory

import pprint
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Muon.generateMuon' )

def fakeHypoAlgCfg(flags, name="FakeHypoForMuon"):
    HLTTest__TestHypoAlg=CompFactory.HLTTest.TestHypoAlg
    return HLTTest__TestHypoAlg( name, Input="" )

def EFMuonCBViewDataVerifierCfg():
    EFMuonCBViewDataVerifier =  CompFactory.AthViews.ViewDataVerifier("VDVEFCBMuon")
    EFMuonCBViewDataVerifier.DataObjects = [( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+HLT_IDTrack_Muon_FTF' ),
                                            ( 'Muon::MdtPrepDataContainer' , 'StoreGateSvc+MDT_DriftCircles' ),  
                                            ( 'Muon::TgcPrepDataContainer' , 'StoreGateSvc+TGC_Measurements' ),
                                            ( 'Muon::RpcPrepDataContainer' , 'StoreGateSvc+RPC_Measurements' ),
                                            ( 'MuonCandidateCollection' , 'StoreGateSvc+MuonCandidates' ),
                                            ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]
    result = ComponentAccumulator()
    result.addEventAlgo(EFMuonCBViewDataVerifier)
    return result

def EFMuonViewDataVerifierCfg():
    EFMuonViewDataVerifier =  CompFactory.AthViews.ViewDataVerifier("VDVEFMuon")
    EFMuonViewDataVerifier.DataObjects = [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                                          ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+EFMuMSRecoRoIs' ),
                                          ( 'RpcPad_Cache' , 'StoreGateSvc+RpcRdoCache' ),
                                          ( 'RpcCoinDataCollection_Cache' , 'StoreGateSvc+RpcCoinCache' ),
                                          ( 'RpcPrepDataCollection_Cache' , 'StoreGateSvc+RpcPrdCache' ),
                                          ( 'TgcRdo_Cache' , 'StoreGateSvc+TgcRdoCache' ),
                                          ( 'MdtCsm_Cache' , 'StoreGateSvc+MdtCsmRdoCache' ),
                                          ( 'CscRawDataCollection_Cache' , 'StoreGateSvc+CscRdoCache' )
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

def decodeCfg(flags, RoIs):
    acc = ComponentAccumulator()

    from RegionSelector.RegSelToolConfig import regSelTool_RPC_Cfg
    RegSelTool_RPC = acc.popToolsAndMerge(regSelTool_RPC_Cfg(flags))

    from RegionSelector.RegSelToolConfig import regSelTool_TGC_Cfg
    RegSelTool_TGC = acc.popToolsAndMerge(regSelTool_TGC_Cfg(flags))

    from RegionSelector.RegSelToolConfig import regSelTool_MDT_Cfg
    RegSelTool_MDT = acc.popToolsAndMerge(regSelTool_MDT_Cfg(flags))

    from RegionSelector.RegSelToolConfig import regSelTool_CSC_Cfg
    RegSelTool_CSC = acc.popToolsAndMerge(regSelTool_CSC_Cfg(flags))

    # Get RPC BS decoder
    from MuonConfig.MuonBytestreamDecodeConfig import RpcBytestreamDecodeCfg
    rpcAcc = RpcBytestreamDecodeCfg( flags, name = "RpcRawDataProvider_"+RoIs )
    rpcAcc.getEventAlgo("RpcRawDataProvider_"+RoIs).RoIs = RoIs
    rpcAcc.getEventAlgo("RpcRawDataProvider_"+RoIs).RegionSelectionTool = RegSelTool_RPC
    acc.merge( rpcAcc )

    # Get RPC BS->RDO convertor
    from MuonConfig.MuonRdoDecodeConfig import RpcRDODecodeCfg
    rpcAcc = RpcRDODecodeCfg( flags, name= "RpcRdoToRpcPrepData_"+RoIs )
    rpcAcc.getEventAlgo("RpcRdoToRpcPrepData_"+RoIs).RoIs = RoIs
    acc.merge( rpcAcc )

    # Get TGC BS decoder
    from MuonConfig.MuonBytestreamDecodeConfig import TgcBytestreamDecodeCfg
    tgcAcc = TgcBytestreamDecodeCfg( flags, name="TgcRawDataProvider_"+RoIs )
    tgcAcc.getEventAlgo("TgcRawDataProvider_"+RoIs).RoIs = RoIs
    tgcAcc.getEventAlgo("TgcRawDataProvider_"+RoIs).RegionSelectionTool = RegSelTool_TGC
    acc.merge( tgcAcc )

    # Get TGC BS->RDO convertor
    from MuonConfig.MuonRdoDecodeConfig import TgcRDODecodeCfg
    tgcAcc = TgcRDODecodeCfg( flags, name="TgcRdoToTgcPrepData_"+RoIs )
    tgcAcc.getEventAlgo("TgcRdoToTgcPrepData_"+RoIs).RoIs = RoIs
    acc.merge( tgcAcc )

    # Get MDT BS decoder
    from MuonConfig.MuonBytestreamDecodeConfig import MdtBytestreamDecodeCfg
    mdtAcc = MdtBytestreamDecodeCfg( flags, name="MdtRawDataProvider_"+RoIs )
    mdtAcc.getEventAlgo("MdtRawDataProvider_"+RoIs).RoIs = RoIs
    mdtAcc.getEventAlgo("MdtRawDataProvider_"+RoIs).RegionSelectionTool = RegSelTool_MDT
    acc.merge( mdtAcc )

    # Get MDT BS->RDO convertor
    from MuonConfig.MuonRdoDecodeConfig import MdtRDODecodeCfg
    mdtAcc = MdtRDODecodeCfg( flags, name="MdtRdoToMdtPrepData_"+RoIs )
    mdtAcc.getEventAlgo("MdtRdoToMdtPrepData_"+RoIs).RoIs = RoIs
    acc.merge( mdtAcc )

    # Get CSC BS decoder
    from MuonConfig.MuonBytestreamDecodeConfig import CscBytestreamDecodeCfg
    cscAcc = CscBytestreamDecodeCfg( flags, name="CscRawDataProvider_"+RoIs )
    cscAcc.getEventAlgo("CscRawDataProvider_"+RoIs).RoIs = RoIs
    cscAcc.getEventAlgo("CscRawDataProvider_"+RoIs).RegionSelectionTool = RegSelTool_CSC
    acc.merge( cscAcc )

    # Get CSC BS->RDO convertor
    from MuonConfig.MuonRdoDecodeConfig import CscRDODecodeCfg
    cscAcc = CscRDODecodeCfg( flags, name="CscRdoToCscPrepData_"+RoIs )
    cscAcc.getEventAlgo("CscRdoToCscPrepData_"+RoIs).RoIs = RoIs
    acc.merge( cscAcc )

    # Get CSC cluster builder
    from MuonConfig.MuonRdoDecodeConfig import CscClusterBuildCfg
    cscAcc = CscClusterBuildCfg( flags, name="CscThresholdClusterBuilder_"+RoIs )
    acc.merge( cscAcc )

    return acc


def efMuHypoCfg(flags, name="UNSPECIFIED", inputMuons="UNSPECIFIED"):
    TrigMuonEFHypoAlg = CompFactory.TrigMuonEFHypoAlg
    efHypo = TrigMuonEFHypoAlg(name)
    efHypo.MuonDecisions = inputMuons
    return efHypo


def generateChains( flags, chainDict ):
    chainDict = splitChainDict(chainDict)[0]

    #Clone and replace offline flags so we can set muon trigger specific values
    muonflags = flags.cloneAndReplace('Muon', 'Trigger.Offline.Muon')

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
    decodeAcc = decodeCfg(muonflags, reco.name+"RoIs")
    reco.mergeReco(decodeAcc)


    # Get Reco alg of muFast Step in order to set into the view
    algAcc, alg = l2MuFastAlgCfg( muonflags, roisKey=reco.name+"RoIs")

    l2MuFastAlgAcc = ComponentAccumulator()
    l2MuFastAlgAcc.addEventAlgo(alg)

    reco.mergeReco( l2MuFastAlgAcc )
    reco.merge( algAcc )
    acc.merge( reco, sequenceName=stepReco.getName() )

    ### Set muon step1 ###
    l2muFastHypo = l2MuFastHypoCfg( muonflags,
                                    name = 'TrigL2MuFastHypo',
                                    muFastInfo = 'MuonL2SAInfo' )

    acc.addEventAlgo(l2muFastHypo, sequenceName=stepView.getName())

    l2muFastSequence = CAMenuSequence(acc, 
                                      HypoToolGen = TrigMufastHypoToolFromDict )

    l2muFastStep = ChainStep( name=stepName, Sequences=[l2muFastSequence], chainDicts=[chainDict] )

    if 'msonly' not in chainDict['chainName']:
        #only run in combined muon chains
        ### Set muon step2 - L2muComb ###
        stepL2CBName = 'L2MuonCB'
        stepL2CBReco, stepL2CBView = createStepView(stepL2CBName)

        accL2CB = ComponentAccumulator()
        accL2CB.addSequence(stepL2CBView)

        # Set EventViews for L2MuonCB step
        recoL2CB = l2MuCombRecoCfg(muonflags)
        #external data loading to view
        recoL2CB.inputMaker().RequireParentView = True
        recoL2CB.mergeReco( MuCombViewDataVerifier() )

        #ID tracking
        accID = trigInDetFastTrackingCfg( muonflags, roisKey=recoL2CB.inputMaker().InViewRoIs, signatureName="Muon" )
        recoL2CB.mergeReco(accID)

        accL2CB.merge(recoL2CB, sequenceName = stepL2CBReco.getName())

        l2muCombHypo = l2MuCombHypoCfg(muonflags,
                                       name = 'TrigL2MuCombHypo',
                                       muCombInfo = 'HLT_MuonL2CBInfo' )

        accL2CB.addEventAlgo(l2muCombHypo, sequenceName=stepL2CBView.getName())

        l2muCombSequence = CAMenuSequence(accL2CB,
                                          HypoToolGen = TrigmuCombHypoToolFromDict)

        l2muCombStep = ChainStep( name=stepL2CBName, Sequences=[l2muCombSequence], chainDicts=[chainDict] )


    #EF MS only
    stepEFMSName = 'EFMSMuon'
    stepEFMSReco, stepEFMSView = createStepView(stepEFMSName)

    accMS = ComponentAccumulator()
    accMS.addSequence(stepEFMSView)
    
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco
    ViewCreatorFetchFromViewROITool=CompFactory.ViewCreatorFetchFromViewROITool
    viewName="EFMuMSReco"
    viewMakerAlg = CompFactory.EventViewCreatorAlgorithm("IM"+viewName,
                                                         ViewFallThrough = True,
                                                         RoIsLink        = 'initialRoI',
                                                         RoITool         = ViewCreatorFetchFromViewROITool(RoisWriteHandleKey="Roi_L2SAMuonForEF", InViewRoIs = "forMS", ViewToFetchFrom = "L2MuFastRecoViews"),
                                                         InViewRoIs      = viewName+'RoIs',
                                                         Views           = viewName+'Views',
                                                         ViewNodeName    = viewName+"InView")
    recoMS = InViewReco(name=viewName, viewMaker=viewMakerAlg)
    recoMS.inputMaker().RequireParentView = True
    
    #Probably this block will eventually need to move somewhere more central
    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    accMS.merge( BeamPipeGeometryCfg(muonflags) )

    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    accMS.merge(PixelGeometryCfg(muonflags))
    
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
    accMS.merge(SCT_GeometryCfg(muonflags))
    
    from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
    accMS.merge(TRT_GeometryCfg(muonflags))

    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    accMS.merge(TrackingGeometrySvcCfg(muonflags))

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    accMS.merge(LArGMCfg(muonflags))

    from TileGeoModel.TileGMConfig import TileGMCfg
    accMS.merge(TileGMCfg(muonflags))
    ###################

    EFMuonViewDataVerifier = EFMuonViewDataVerifierCfg()
    recoMS.mergeReco(EFMuonViewDataVerifier)

    # decoding
    decodeAcc = decodeCfg(muonflags, recoMS.name+"RoIs")
    recoMS.mergeReco(decodeAcc)
    
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

    efmuMSHypo = efMuHypoCfg( muonflags,
                              name = 'TrigMuonEFMSonlyHypo',
                              inputMuons = "Muons" )

    accMS.addEventAlgo(efmuMSHypo, sequenceName=stepEFMSView.getName())

    efmuMSSequence = CAMenuSequence(accMS,
                                    HypoToolGen = TrigMuonEFMSonlyHypoToolFromDict)

    efmuMSStep = ChainStep( name=stepEFMSName, Sequences=[efmuMSSequence], chainDicts=[chainDict] )


    #EF combined muons
    if 'msonly' not in chainDict['chainName']:
        stepEFCBName = 'EFCBMuon'
        stepEFCBReco, stepEFCBView = createStepView(stepEFCBName)

        accCB = ComponentAccumulator()
        accCB.addSequence(stepEFCBView)
    
        from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco
        recoCB = InViewReco("EFMuCBReco")
        recoCB.inputMaker().RequireParentView = True

        recoCB.mergeReco(EFMuonCBViewDataVerifierCfg())
    
        from MuonCombinedConfig.MuonCombinedReconstructionConfig import MuonCombinedInDetCandidateAlg, MuonCombinedAlgCfg
        indetCandCfg = MuonCombinedInDetCandidateAlg(muonflags, name="TrigMuonCombinedInDetCandidateAlg_RoI", TrackParticleLocation=["HLT_IDTrack_Muon_FTF"], 
                                                     InDetCandidateLocation="IndetCandidates_RoI", TrackSelector="",
                                                     DoSiliconAssocForwardMuons=False, InDetForwardTrackSelector="")
        recoCB.mergeReco(indetCandCfg)

        muonCombCfg = MuonCombinedAlgCfg(muonflags, name="TrigMuonCombinedAlg_RoI", MuonCandidateLocation="MuonCandidates", 
                                         InDetCandidateLocation="IndetCandidates_RoI")
        recoCB.mergeReco(muonCombCfg)

        muonCreatorCBCfg = MuonCreatorAlgCfg(muonflags, name="TrigMuonCreatorAlgCB_RoI", MuonCandidateLocation="MuonCandidates", TagMaps=["muidcoTagMap"], 
                                            InDetCandidateLocation="InDetCandidates_RoI", MuonContainerLocation = "MuonsCB", SegmentContainerName = "xaodCBSegments", TrackSegmentContainerName = "TrkCBSegments",
                                            ExtrapolatedLocation = "CBExtrapolatedMuons", MSOnlyExtrapolatedLocation = "CBMSonlyExtrapolatedMuons", CombinedLocation = "HLT_CBCombinedMuon_RoI")
        recoCB.mergeReco(muonCreatorCBCfg)

        accCB.merge(recoCB, sequenceName=stepEFCBReco.getName())

        efmuCBHypo = efMuHypoCfg( muonflags,
                                  name = 'TrigMuonEFCBHypo',
                                  inputMuons = "MuonsCB" )

        accCB.addEventAlgo(efmuCBHypo, sequenceName=stepEFCBView.getName())

        efmuCBSequence = CAMenuSequence(accCB,
                                        HypoToolGen = TrigMuonEFCombinerHypoToolFromDict)

        efmuCBStep = ChainStep( name=stepEFCBName, Sequences=[efmuCBSequence], chainDicts=[chainDict] )


    l1Thresholds=[]
    for part in chainDict['chainParts']:
        l1Thresholds.append(part['L1threshold'])

    log.debug('dictionary is: %s\n', pprint.pformat(chainDict))
    def _empty(name):
        return ChainStep(name="EmptyNoL2MuComb", Sequences=[EmptyMenuSequence("EmptyNoL2MuComb")], chainDicts=[chainDict])
    if 'msonly' in chainDict['chainName']:
        chain = Chain( name=chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[ l2muFastStep, _empty("EmptyNoL2MuComb"), efmuMSStep, _empty("EmptyNoEFCB"), _empty("JustEmpty") ] )
    else:
        chain = Chain( name=chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[ l2muFastStep, l2muCombStep, efmuMSStep, efmuCBStep, _empty("JustEmpty") ] )
    return chain

