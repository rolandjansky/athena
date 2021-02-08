# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CAMenuSequence, ChainStep, Chain, EmptyMenuSequence, SelectionCA, InViewReco
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from TrigL2MuonSA.TrigL2MuonSAConfig_newJO import l2MuFastAlgCfg, l2MuFastHypoCfg
from TrigmuComb.TrigmuCombConfig_newJO import l2MuCombRecoCfg, l2MuCombHypoCfg
from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoToolFromDict, TrigmuCombHypoToolFromDict, TrigMuonEFMSonlyHypoToolFromDict, TrigMuonEFCombinerHypoToolFromDict
from TrigInDetConfig.TrigInDetConfig import trigInDetFastTrackingCfg

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict

from AthenaConfiguration.ComponentFactory import CompFactory
from RegionSelector.RegSelToolConfig import regSelTool_RPC_Cfg, regSelTool_TGC_Cfg, regSelTool_MDT_Cfg, regSelTool_CSC_Cfg

from MuonConfig.MuonBytestreamDecodeConfig import RpcBytestreamDecodeCfg, TgcBytestreamDecodeCfg, MdtBytestreamDecodeCfg, CscBytestreamDecodeCfg
from MuonConfig.MuonRdoDecodeConfig import RpcRDODecodeCfg, TgcRDODecodeCfg, MdtRDODecodeCfg, CscRDODecodeCfg, CscClusterBuildCfg

from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
from LArGeoAlgsNV.LArGMConfig import LArGMCfg
from TileGeoModel.TileGMConfig import TileGMCfg
from MuonConfig.MuonSegmentFindingConfig import MooSegmentFinderAlgCfg
from MuonConfig.MuonTrackBuildingConfig import MuonTrackBuildingCfg
from MuonCombinedConfig.MuonCombinedReconstructionConfig import MuonCombinedMuonCandidateAlgCfg
from MuonCombinedConfig.MuonCombinedReconstructionConfig import MuonCombinedInDetCandidateAlg, MuonCombinedAlgCfg, MuonCreatorAlgCfg

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

    RegSelTool_RPC = acc.popToolsAndMerge(regSelTool_RPC_Cfg(flags))
    RegSelTool_TGC = acc.popToolsAndMerge(regSelTool_TGC_Cfg(flags))
    RegSelTool_MDT = acc.popToolsAndMerge(regSelTool_MDT_Cfg(flags))
    RegSelTool_CSC = acc.popToolsAndMerge(regSelTool_CSC_Cfg(flags))

    # Get RPC BS decoder
    rpcAcc = RpcBytestreamDecodeCfg( flags, name = "RpcRawDataProvider_"+RoIs )
    rpcAcc.getEventAlgo("RpcRawDataProvider_"+RoIs).RoIs = RoIs
    rpcAcc.getEventAlgo("RpcRawDataProvider_"+RoIs).RegionSelectionTool = RegSelTool_RPC
    acc.merge( rpcAcc )

    # Get RPC BS->RDO convertor
    rpcAcc = RpcRDODecodeCfg( flags, name= "RpcRdoToRpcPrepData_"+RoIs )
    rpcAcc.getEventAlgo("RpcRdoToRpcPrepData_"+RoIs).RoIs = RoIs
    acc.merge( rpcAcc )

    # Get TGC BS decoder
    tgcAcc = TgcBytestreamDecodeCfg( flags, name="TgcRawDataProvider_"+RoIs )
    tgcAcc.getEventAlgo("TgcRawDataProvider_"+RoIs).RoIs = RoIs
    tgcAcc.getEventAlgo("TgcRawDataProvider_"+RoIs).RegionSelectionTool = RegSelTool_TGC
    acc.merge( tgcAcc )

    # Get TGC BS->RDO convertor
    tgcAcc = TgcRDODecodeCfg( flags, name="TgcRdoToTgcPrepData_"+RoIs )
    tgcAcc.getEventAlgo("TgcRdoToTgcPrepData_"+RoIs).RoIs = RoIs
    acc.merge( tgcAcc )

    # Get MDT BS decoder
    mdtAcc = MdtBytestreamDecodeCfg( flags, name="MdtRawDataProvider_"+RoIs )
    mdtAcc.getEventAlgo("MdtRawDataProvider_"+RoIs).RoIs = RoIs
    mdtAcc.getEventAlgo("MdtRawDataProvider_"+RoIs).RegionSelectionTool = RegSelTool_MDT
    acc.merge( mdtAcc )

    # Get MDT BS->RDO convertor
    mdtAcc = MdtRDODecodeCfg( flags, name="MdtRdoToMdtPrepData_"+RoIs )
    mdtAcc.getEventAlgo("MdtRdoToMdtPrepData_"+RoIs).RoIs = RoIs
    acc.merge( mdtAcc )

    # Get CSC BS decoder
    cscAcc = CscBytestreamDecodeCfg( flags, name="CscRawDataProvider_"+RoIs )
    cscAcc.getEventAlgo("CscRawDataProvider_"+RoIs).RoIs = RoIs
    cscAcc.getEventAlgo("CscRawDataProvider_"+RoIs).RegionSelectionTool = RegSelTool_CSC
    acc.merge( cscAcc )

    # Get CSC BS->RDO convertor
    cscAcc = CscRDODecodeCfg( flags, name="CscRdoToCscPrepData_"+RoIs )
    cscAcc.getEventAlgo("CscRdoToCscPrepData_"+RoIs).RoIs = RoIs
    acc.merge( cscAcc )

    # Get CSC cluster builder
    cscAcc = CscClusterBuildCfg( flags, name="CscThresholdClusterBuilder_"+RoIs )
    acc.merge( cscAcc )

    return acc


def efMuHypoCfg(flags, name="UNSPECIFIED", inputMuons="UNSPECIFIED"):
    TrigMuonEFHypoAlg = CompFactory.TrigMuonEFHypoAlg
    efHypo = TrigMuonEFHypoAlg(name)
    efHypo.MuonDecisions = inputMuons
    return efHypo

def muFastStep(flags, chainDict):
    # Step 1 (L2MuonSA)
    selAcc = SelectionCA("L2MuFastReco")
    # Set EventViews for L2MuonSA step
    reco = InViewReco("L2MuFastReco")

    #external data loading to view
    reco.mergeReco( MuFastViewDataVerifier() )

    # decoding
    decodeAcc = decodeCfg(flags, selAcc.name+"RoIs")
    reco.mergeReco(decodeAcc)

    #L2 SA alg
    reco.mergeReco(l2MuFastAlgCfg( flags, roisKey=selAcc.name+"RoIs"))

    selAcc.mergeReco(reco)

    l2muFastHypo = l2MuFastHypoCfg( flags,
                                    name = 'TrigL2MuFastHypo',
                                    muFastInfo = 'MuonL2SAInfo' )

    selAcc.addHypoAlgo(l2muFastHypo)

    l2muFastSequence = CAMenuSequence(selAcc, 
                                      HypoToolGen = TrigMufastHypoToolFromDict )

    return ChainStep( name=selAcc.name, Sequences=[l2muFastSequence], chainDicts=[chainDict] )

def muCombStep(flags, chainDict):
    ### Set muon step2 - L2muComb ###
    selAccL2CB = SelectionCA("L2MuonCB")

    # L2MuonCB reco
    recoL2CB = l2MuCombRecoCfg(flags)

    #external data loading to view
    recoL2CB.mergeReco( MuCombViewDataVerifier() )

    #ID tracking
    recoL2CB.mergeReco(trigInDetFastTrackingCfg( flags, roisKey=recoL2CB.inputMaker().InViewRoIs, signatureName="Muon" ))

    selAccL2CB.mergeReco(recoL2CB)

    l2muCombHypo = l2MuCombHypoCfg(flags,
                                   name = 'TrigL2MuCombHypo',
                                   muCombInfo = 'HLT_MuonL2CBInfo' )

    selAccL2CB.addHypoAlgo(l2muCombHypo)
    l2muCombSequence = CAMenuSequence(selAccL2CB,
                                      HypoToolGen = TrigmuCombHypoToolFromDict)

    return ChainStep( name=selAccL2CB.name, Sequences=[l2muCombSequence], chainDicts=[chainDict] )

def muEFSAStep(flags, chainDict):
    #EF MS only
    selAccMS = SelectionCA('EFMuMSReco')
    
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
    recoMS.merge( BeamPipeGeometryCfg(flags) )
    recoMS.merge(PixelGeometryCfg(flags))
    recoMS.merge(SCT_GeometryCfg(flags))
    recoMS.merge(TRT_GeometryCfg(flags))
    recoMS.merge(TrackingGeometrySvcCfg(flags))
    recoMS.merge(LArGMCfg(flags))
    recoMS.merge(TileGMCfg(flags))
    ###################

    recoMS.mergeReco(EFMuonViewDataVerifierCfg())

    # decoding
    recoMS.mergeReco(decodeCfg(flags, selAccMS.name+"RoIs"))

    #Reco
    recoMS.mergeReco( MooSegmentFinderAlgCfg(flags,name="TrigMooSegmentFinder",UseTGCNextBC=False, UseTGCPriorBC=False))
    recoMS.mergeReco(MuonTrackBuildingCfg(flags, name="TrigMuPatTrackBuilder"))
    recoMS.mergeReco(MuonTrackParticleCnvCfg(flags, name = "TrigMuonTrackParticleCnvAlg"))
    recoMS.mergeReco(MuonCombinedMuonCandidateAlgCfg(flags, name = "TrigMuonCandidateAlg"))
    recoMS.mergeReco(MuonCreatorAlgCfg(flags, name = "TrigMuonCreatorAlg"))

    selAccMS.mergeReco(recoMS)

    efmuMSHypo = efMuHypoCfg( flags,
                              name = 'TrigMuonEFMSonlyHypo',
                              inputMuons = "Muons" )

    selAccMS.addHypoAlgo(efmuMSHypo)

    efmuMSSequence = CAMenuSequence(selAccMS,
                                    HypoToolGen = TrigMuonEFMSonlyHypoToolFromDict)

    return ChainStep( name=selAccMS.name, Sequences=[efmuMSSequence], chainDicts=[chainDict] )

def muEFCBStep(flags, chainDict):
    #EF combined muons
    selAccEFCB = SelectionCA("EFCBMuon")
    
    recoCB = InViewReco("EFMuCBReco")
    recoCB.inputMaker().RequireParentView = True

    recoCB.mergeReco(EFMuonCBViewDataVerifierCfg())
    
    indetCandCfg = MuonCombinedInDetCandidateAlg(flags, name="TrigMuonCombinedInDetCandidateAlg_RoI", TrackParticleLocation=["HLT_IDTrack_Muon_FTF"], 
                                                 InDetCandidateLocation="IndetCandidates_RoI", TrackSelector="",DoSiliconAssocForwardMuons=False, InDetForwardTrackSelector="")
    recoCB.mergeReco(indetCandCfg)
    muonCombCfg = MuonCombinedAlgCfg(flags, name="TrigMuonCombinedAlg_RoI", MuonCandidateLocation="MuonCandidates", 
                                     InDetCandidateLocation="IndetCandidates_RoI")
    recoCB.mergeReco(muonCombCfg)

    muonCreatorCBCfg = MuonCreatorAlgCfg(flags, name="TrigMuonCreatorAlgCB_RoI", MuonCandidateLocation="MuonCandidates", TagMaps=["muidcoTagMap"], 
                                         InDetCandidateLocation="InDetCandidates_RoI", MuonContainerLocation = "MuonsCB", SegmentContainerName = "xaodCBSegments", TrackSegmentContainerName = "TrkCBSegments",
                                         ExtrapolatedLocation = "CBExtrapolatedMuons", MSOnlyExtrapolatedLocation = "CBMSonlyExtrapolatedMuons", CombinedLocation = "HLT_CBCombinedMuon_RoI")
    recoCB.mergeReco(muonCreatorCBCfg)
    
    selAccEFCB.mergeReco(recoCB)

    efmuCBHypo = efMuHypoCfg( flags,
                              name = 'TrigMuonEFCBHypo',
                              inputMuons = "MuonsCB" )

    selAccEFCB.addHypoAlgo(efmuCBHypo)

    efmuCBSequence = CAMenuSequence(selAccEFCB,
                                    HypoToolGen = TrigMuonEFCombinerHypoToolFromDict)
    
    return ChainStep( name=selAccEFCB.name, Sequences=[efmuCBSequence], chainDicts=[chainDict] )

def generateChains( flags, chainDict ):
    chainDict = splitChainDict(chainDict)[0]

    #Clone and replace offline flags so we can set muon trigger specific values
    muonflags = flags.cloneAndReplace('Muon', 'Trigger.Offline.Muon')

    muFast = muFastStep(muonflags, chainDict)
    muEFSA = muEFSAStep(muonflags, chainDict)

    l1Thresholds=[]
    for part in chainDict['chainParts']:
        l1Thresholds.append(part['L1threshold'])

    log.debug('dictionary is: %s\n', pprint.pformat(chainDict))
    def _empty(name):
        return ChainStep(name="EmptyNoL2MuComb", Sequences=[EmptyMenuSequence("EmptyNoL2MuComb")], chainDicts=[chainDict])
    if 'msonly' in chainDict['chainName']:
        chain = Chain( name=chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[ muFast, _empty("EmptyNoL2MuComb"), muEFSA, _empty("EmptyNoEFCB") ] )
    else:
        muComb = muCombStep(muonflags, chainDict)
        muEFCB = muEFCBStep(muonflags, chainDict)
        chain = Chain( name=chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[ muFast, muComb, muEFSA, muEFCB ] )
    return chain

