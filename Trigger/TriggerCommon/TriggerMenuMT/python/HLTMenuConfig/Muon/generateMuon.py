# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CAMenuSequence, ChainStep, Chain, EmptyMenuSequence, SelectionCA
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
    selAcc = SelectionCA("L2MuFastReco")
    # Set EventViews for L2MuonSA step
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco
    reco = InViewReco("L2MuFastReco")

    #external data loading to view
    reco.mergeReco( MuFastViewDataVerifier() )

    # decoding
    decodeAcc = decodeCfg(muonflags, selAcc.name+"RoIs")
    reco.mergeReco(decodeAcc)

    #L2 SA alg
    reco.mergeReco(l2MuFastAlgCfg( muonflags, roisKey=selAcc.name+"RoIs"))

    selAcc.mergeReco(reco)

    ### Set muon step1 ###
    l2muFastHypo = l2MuFastHypoCfg( muonflags,
                                    name = 'TrigL2MuFastHypo',
                                    muFastInfo = 'MuonL2SAInfo' )

    selAcc.addHypoAlgo(l2muFastHypo)

    l2muFastSequence = CAMenuSequence(selAcc, 
                                      HypoToolGen = TrigMufastHypoToolFromDict )

    l2muFastStep = ChainStep( name=selAcc.name, Sequences=[l2muFastSequence], chainDicts=[chainDict] )

    if 'msonly' not in chainDict['chainName']:
        #only run in combined muon chains
        ### Set muon step2 - L2muComb ###
        selAccL2CB = SelectionCA("L2MuonCB")

        # L2MuonCB reco
        recoL2CB = l2MuCombRecoCfg(muonflags)

        #external data loading to view
        recoL2CB.mergeReco( MuCombViewDataVerifier() )

        #ID tracking
        recoL2CB.mergeReco(trigInDetFastTrackingCfg( muonflags, roisKey=recoL2CB.inputMaker().InViewRoIs, signatureName="Muon" ))

        selAccL2CB.mergeReco(recoL2CB)

        l2muCombHypo = l2MuCombHypoCfg(muonflags,
                                       name = 'TrigL2MuCombHypo',
                                       muCombInfo = 'HLT_MuonL2CBInfo' )

        selAccL2CB.addHypoAlgo(l2muCombHypo)
        l2muCombSequence = CAMenuSequence(selAccL2CB,
                                          HypoToolGen = TrigmuCombHypoToolFromDict)

        l2muCombStep = ChainStep( name=selAccL2CB.name, Sequences=[l2muCombSequence], chainDicts=[chainDict] )


    #EF MS only
    selAccMS = SelectionCA('EFMuMSReco')
    
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco
    ViewCreatorFetchFromViewROITool=CompFactory.ViewCreatorFetchFromViewROITool
    viewName="EFMuMSReco"
    viewMakerAlg = CompFactory.EventViewCreatorAlgorithm("IM"+viewName,
                                                         ViewFallThrough = True,
                                                         RequireParentView = True,
                                                         RoIsLink        = 'initialRoI',
                                                         RoITool         = ViewCreatorFetchFromViewROITool(RoisWriteHandleKey="Roi_L2SAMuonForEF", InViewRoIs = "forMS", ViewToFetchFrom = "L2MuFastRecoViews"),
                                                         InViewRoIs      = viewName+'RoIs',
                                                         Views           = viewName+'Views',
                                                         ViewNodeName    = viewName+"InView")
    recoMS = InViewReco(name=viewName, viewMaker=viewMakerAlg)
    
    #Probably this block will eventually need to move somewhere more central
    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    recoMS.merge( BeamPipeGeometryCfg(muonflags) )

    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    recoMS.merge(PixelGeometryCfg(muonflags))
    
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
    recoMS.merge(SCT_GeometryCfg(muonflags))
    
    from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
    recoMS.merge(TRT_GeometryCfg(muonflags))

    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    recoMS.merge(TrackingGeometrySvcCfg(muonflags))

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    recoMS.merge(LArGMCfg(muonflags))

    from TileGeoModel.TileGMConfig import TileGMCfg
    recoMS.merge(TileGMCfg(muonflags))
    ###################

    recoMS.mergeReco(EFMuonViewDataVerifierCfg())

    # decoding
    recoMS.mergeReco(decodeCfg(muonflags, selAccMS.name+"RoIs"))
    
    from MuonConfig.MuonSegmentFindingConfig import MooSegmentFinderAlgCfg
    recoMS.mergeReco( MooSegmentFinderAlgCfg(muonflags,name="TrigMooSegmentFinder",UseTGCNextBC=False, UseTGCPriorBC=False))

    from MuonConfig.MuonTrackBuildingConfig import MuonTrackBuildingCfg
    recoMS.mergeReco(MuonTrackBuildingCfg(muonflags, name="TrigMuPatTrackBuilder"))
    recoMS.mergeReco(MuonTrackParticleCnvCfg(muonflags, name = "TrigMuonTrackParticleCnvAlg"))
    
    from MuonCombinedConfig.MuonCombinedReconstructionConfig import MuonCombinedMuonCandidateAlgCfg
    recoMS.mergeReco(MuonCombinedMuonCandidateAlgCfg(muonflags, name = "TrigMuonCandidateAlg"))

    from MuonCombinedConfig.MuonCombinedReconstructionConfig import MuonCreatorAlgCfg
    recoMS.mergeReco(MuonCreatorAlgCfg(muonflags, name = "TrigMuonCreatorAlg"))

    selAccMS.mergeReco(recoMS)

    efmuMSHypo = efMuHypoCfg( muonflags,
                              name = 'TrigMuonEFMSonlyHypo',
                              inputMuons = "Muons" )

    selAccMS.addHypoAlgo(efmuMSHypo)

    efmuMSSequence = CAMenuSequence(selAccMS,
                                    HypoToolGen = TrigMuonEFMSonlyHypoToolFromDict)

    efmuMSStep = ChainStep( name=selAccMS.name, Sequences=[efmuMSSequence], chainDicts=[chainDict] )


    #EF combined muons
    if 'msonly' not in chainDict['chainName']:
        selAccEFCB = SelectionCA("EFCBMuon")
    
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

        selAccEFCB.mergeReco(recoCB)

        efmuCBHypo = efMuHypoCfg( muonflags,
                                  name = 'TrigMuonEFCBHypo',
                                  inputMuons = "MuonsCB" )

        selAccEFCB.addHypoAlgo(efmuCBHypo)

        efmuCBSequence = CAMenuSequence(selAccEFCB,
                                        HypoToolGen = TrigMuonEFCombinerHypoToolFromDict)

        efmuCBStep = ChainStep( name=selAccEFCB.name, Sequences=[efmuCBSequence], chainDicts=[chainDict] )


    l1Thresholds=[]
    for part in chainDict['chainParts']:
        l1Thresholds.append(part['L1threshold'])

    log.debug('dictionary is: %s\n', pprint.pformat(chainDict))
    def _empty(name):
        return ChainStep(name="EmptyNoL2MuComb", Sequences=[EmptyMenuSequence("EmptyNoL2MuComb")], chainDicts=[chainDict])
    if 'msonly' in chainDict['chainName']:
        chain = Chain( name=chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[ l2muFastStep, _empty("EmptyNoL2MuComb"), efmuMSStep, _empty("EmptyNoEFCB") ] )
    else:
        chain = Chain( name=chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[ l2muFastStep, l2muCombStep, efmuMSStep, efmuCBStep ] )
    return chain

