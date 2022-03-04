# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequenceCA, ChainStep, Chain, EmptyMenuSequence, SelectionCA, InViewRecoCA
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.Enums import BeamType, Format

from TrigL2MuonSA.TrigL2MuonSAConfig_newJO import l2MuFastAlgCfg, l2MuFastHypoCfg
from TrigmuComb.TrigmuCombConfig_newJO import l2MuCombRecoCfg, l2MuCombHypoCfg
from TrigMuonHypo.TrigMuonHypoConfig import TrigMufastHypoToolFromDict, TrigmuCombHypoToolFromDict, TrigMuonEFMSonlyHypoToolFromDict, TrigMuonEFCombinerHypoToolFromDict, TrigMuonEFTrackIsolationHypoToolFromDict
from TrigInDetConfig.TrigInDetConfig import trigInDetFastTrackingCfg

from TriggerMenuMT.HLT.Config.Utility.ChainDictTools import splitChainDict
from TriggerMenuMT.HLT.Muon.MuonRecoSequences import muonDecodeCfg

from AthenaConfiguration.ComponentFactory import CompFactory

from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
from MuonConfig.MuonSegmentFindingConfig import MooSegmentFinderAlgCfg
from MuonConfig.MuonTrackBuildingConfig import MuonTrackBuildingCfg
from MuonCombinedConfig.MuonCombinedReconstructionConfig import MuonCombinedMuonCandidateAlgCfg, MuonInsideOutRecoAlgCfg, MuonInDetToMuonSystemExtensionAlgCfg
from MuonSegmentTrackMaker.MuonTrackMakerAlgsMonitoring import MuPatTrackBuilderMonitoring
from MuonCombinedConfig.MuonCombinedReconstructionConfig import MuonCombinedInDetCandidateAlgCfg, MuonCombinedAlgCfg, MuonCreatorAlgCfg

from TrigMuonEF.TrigMuonEFConfig_newJO import TrigMuonEFTrackIsolationAlgCfg, MuonFilterAlgCfg, MergeEFMuonsAlgCfg
from AthenaCommon.CFElements import seqAND, parOR, seqOR

from AthenaConfiguration.AccumulatorCache import AccumulatorCache

import pprint
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

def fakeHypoAlgCfg(flags, name="FakeHypoForMuon"):
    HLTTest__TestHypoAlg=CompFactory.HLTTest.TestHypoAlg
    return HLTTest__TestHypoAlg( name, Input="" )

@AccumulatorCache
def EFMuonCBViewDataVerifierCfg(flags, name):
    EFMuonCBViewDataVerifier =  CompFactory.AthViews.ViewDataVerifier("VDVEFCBMuon_"+name)
    EFMuonCBViewDataVerifier.DataObjects = [( 'Muon::MdtPrepDataContainer' , 'StoreGateSvc+MDT_DriftCircles' ),  
                                            ( 'Muon::TgcPrepDataContainer' , 'StoreGateSvc+TGC_Measurements' ),
                                            ( 'Muon::RpcPrepDataContainer' , 'StoreGateSvc+RPC_Measurements' ),
                                            ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]

    if flags.Detector.GeometryCSC:
        EFMuonCBViewDataVerifier.DataObjects += [
                                                ( 'Muon::CscStripPrepDataContainer' , 'StoreGateSvc+CSC_Measurements' ),
                                                ( 'Muon::CscPrepDataContainer' , 'StoreGateSvc+CSC_Clusters' )]
    if 'FS' in name:
        EFMuonCBViewDataVerifier.DataObjects += [( 'MuonCandidateCollection' , 'StoreGateSvc+MuonCandidates_FS' )]
    elif flags.Beam.Type is not BeamType.Collisions:
        EFMuonCBViewDataVerifier.DataObjects +=[( 'Muon::HoughDataPerSectorVec' , 'StoreGateSvc+HoughDataPerSectorVec' )]

    else:
        EFMuonCBViewDataVerifier.DataObjects += [( 'MuonCandidateCollection' , 'StoreGateSvc+MuonCandidates' ),
                                                 ( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+'+flags.Trigger.InDetTracking.Muon.tracks_FTF ),
                                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_FlaggedCondData' )]
    if flags.Input.Format is Format.BS:
        EFMuonCBViewDataVerifier.DataObjects += [( 'IDCInDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' ),
                                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' )]
    if flags.Input.isMC:
        EFMuonCBViewDataVerifier.DataObjects += [( 'PixelRDO_Container' , 'StoreGateSvc+PixelRDOs' ),
                                                 ( 'SCT_RDO_Container' , 'StoreGateSvc+SCT_RDOs' ) ]
    result = ComponentAccumulator()
    result.addEventAlgo(EFMuonCBViewDataVerifier)
    return result

def EFMuonViewDataVerifierCfg(flags, name='RoI'):
    EFMuonViewDataVerifier =  CompFactory.AthViews.ViewDataVerifier("VDVEFMuon_"+name)
    EFMuonViewDataVerifier.DataObjects = [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                                          ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+EFMuMSReco_'+name+'RoIs' ),
                                          ( 'RpcPad_Cache' , 'StoreGateSvc+RpcRdoCache' ),
                                          ( 'RpcCoinDataCollection_Cache' , 'StoreGateSvc+RpcCoinCache' ),
                                          ( 'RpcPrepDataCollection_Cache' , 'StoreGateSvc+RpcPrdCache' ),
                                          ( 'TgcRdo_Cache' , 'StoreGateSvc+TgcRdoCache' ),
                                          ( 'MdtCsm_Cache' , 'StoreGateSvc+MdtCsmRdoCache' )]
    if flags.Detector.GeometryCSC:
        EFMuonViewDataVerifier.DataObjects += [( 'CscRawDataCollection_Cache' , 'StoreGateSvc+CscRdoCache' )]

    result = ComponentAccumulator()
    result.addEventAlgo(EFMuonViewDataVerifier)
    return result

def MuonInsideOutViewDataVerifierCfg(flags):
    MuonViewDataVerifier =  CompFactory.AthViews.ViewDataVerifier("VDVMuInsideOut")
    MuonViewDataVerifier.DataObjects = [( 'Muon::RpcPrepDataContainer' , 'StoreGateSvc+RPC_Measurements' ),
                                        ( 'Muon::TgcPrepDataContainer' , 'StoreGateSvc+TGC_Measurements')]
                                     
    if flags.Detector.GeometryCSC:
        MuonViewDataVerifier.DataObjects += [( 'Muon::CscPrepDataContainer' , 'StoreGateSvc+CSC_Clusters' )]
    if flags.Beam.Type is not BeamType.Cosmics:
        MuonViewDataVerifier.DataObjects +=[( 'Muon::HoughDataPerSectorVec' , 'StoreGateSvc+HoughDataPerSectorVec' )]


    result = ComponentAccumulator()
    result.addEventAlgo(MuonViewDataVerifier)
    return result

def MuFastViewDataVerifier(flags):
    result = ComponentAccumulator()
    dataobjects = [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                   ( 'RpcPad_Cache' , 'StoreGateSvc+RpcRdoCache' ),
                   ( 'RpcCoinDataCollection_Cache' , 'StoreGateSvc+RpcCoinCache' ),
                   ( 'RpcPrepDataCollection_Cache' , 'StoreGateSvc+RpcPrdCache' ),
                   ( 'TgcRdo_Cache' , 'StoreGateSvc+TgcRdoCache' ),
                   ( 'MdtCsm_Cache' , 'StoreGateSvc+MdtCsmRdoCache' ),
                   ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+L2MuFastRecoRoIs' )]
    if flags.Detector.GeometryCSC:
        dataobjects += [( 'CscRawDataCollection_Cache' , 'StoreGateSvc+CscRdoCache' )]
    if flags.Trigger.enableL1MuonPhase1:
        dataobjects += [( 'xAOD::MuonRoIContainer' , 'StoreGateSvc+LVL1MuonRoIs' )]
    else:
        dataobjects += [( 'DataVector< LVL1::RecMuonRoI >' , 'StoreGateSvc+HLT_RecMURoIs' )]
    alg = CompFactory.AthViews.ViewDataVerifier( name = "VDVMuFast",
                                                 DataObjects = dataobjects )
    result.addEventAlgo(alg)
    return result

def MuCombViewDataVerifier():
    result = ComponentAccumulator()
    alg = CompFactory.AthViews.ViewDataVerifier( name = "VDVMuComb",
                                                 DataObjects = [( 'xAOD::L2StandAloneMuonContainer' , 'StoreGateSvc+MuonL2SAInfo' )])
    result.addEventAlgo(alg)
    return result

def MuIsoViewDataVerifierCfg():
    result = ComponentAccumulator()
    alg = CompFactory.AthViews.ViewDataVerifier( name = "VDVMuIso",
                                                 DataObjects = [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+EFMuIsoRecoRoIs' ),
                                                                ( 'xAOD::MuonContainer' , 'StoreGateSvc+InViewIsoMuons' )])
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
    particleCreator = acc.popPrivateTools()
    result.addPublicTool(particleCreator) # Still public in TrackParticleCnvAlg
    kwargs.setdefault("TrackParticleCreator", particleCreator)
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



def efMuHypoConf(flags, name="UNSPECIFIED", inputMuons="UNSPECIFIED"):
    TrigMuonEFHypoAlg = CompFactory.TrigMuonEFHypoAlg
    efHypo = TrigMuonEFHypoAlg(name)
    efHypo.MuonDecisions = inputMuons
    return efHypo

def efMuIsoHypoConf(flags, name="UNSPECIFIED", inputMuons="UNSPECIFIED"):
    TrigMuonEFHypoAlg = CompFactory.TrigMuonEFTrackIsolationHypoAlg
    efHypo = TrigMuonEFHypoAlg(name)
    efHypo.EFMuonsName = inputMuons
    return efHypo

@AccumulatorCache
def _muFastStepSeq(flags):
    # Step 1 (L2MuonSA)
    selAcc = SelectionCA("L2MuFastReco")
    # Set EventViews for L2MuonSA step
    reco = InViewRecoCA("L2MuFastReco")

    #external data loading to view
    reco.mergeReco( MuFastViewDataVerifier(flags) )

    # decoding
    decodeAcc = muonDecodeCfg(flags, selAcc.name+"RoIs")
    reco.mergeReco(decodeAcc)

    #L2 SA alg
    reco.mergeReco(l2MuFastAlgCfg( flags, roisKey=selAcc.name+"RoIs"))

    selAcc.mergeReco(reco)

    l2muFastHypo = l2MuFastHypoCfg( flags,
                                    name = 'TrigL2MuFastHypo',
                                    muFastInfo = 'MuonL2SAInfo' )

    selAcc.addHypoAlgo(l2muFastHypo)

    l2muFastSequence = MenuSequenceCA(selAcc, 
                                      HypoToolGen = TrigMufastHypoToolFromDict )

    return (selAcc , l2muFastSequence)

def muFastStep(flags, chainDict):

    selAcc , l2muFastSequence = _muFastStepSeq(flags)

    return ChainStep( name=selAcc.name, Sequences=[l2muFastSequence], chainDicts=[chainDict] )

@AccumulatorCache
def _muCombStepSeq(flags):
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
    l2muCombSequence = MenuSequenceCA(selAccL2CB,
                                      HypoToolGen = TrigmuCombHypoToolFromDict)

    return (selAccL2CB , l2muCombSequence)

def muCombStep(flags, chainDict):

    selAccL2CB , l2muCombSequence = _muCombStepSeq(flags)

    return ChainStep( name=selAccL2CB.name, Sequences=[l2muCombSequence], chainDicts=[chainDict] )

@AccumulatorCache
def _muEFSAStepSeq(flags, name='RoI'):
    #EF MS only
    selAccMS = SelectionCA('EFMuMSReco_'+name)
    
    viewName="EFMuMSReco_"+name
    if 'FS' in name:
        ViewCreatorFSROITool=CompFactory.ViewCreatorFSROITool
        roiTool         = ViewCreatorFSROITool(RoisWriteHandleKey="MuonFS_RoIs")
        requireParentView = False
    else:
        ViewCreatorFetchFromViewROITool=CompFactory.ViewCreatorFetchFromViewROITool
        roiTool         = ViewCreatorFetchFromViewROITool(RoisWriteHandleKey="Roi_L2SAMuonForEF", InViewRoIs = "forMS", ViewToFetchFrom = "L2MuFastRecoViews")
        requireParentView = True
                                                         
    recoMS = InViewRecoCA(name=viewName, RoITool = roiTool, RequireParentView = requireParentView)
    
    recoMS.merge(TrackingGeometrySvcCfg(flags))
    ###################

    recoMS.mergeReco(EFMuonViewDataVerifierCfg(flags, name))

    # decoding
    recoMS.mergeReco(muonDecodeCfg(flags, selAccMS.name+"RoIs"))

    #Reco
    recoMS.mergeReco( MooSegmentFinderAlgCfg(flags,name="TrigMooSegmentFinder_"+name,UseTGCNextBC=False, UseTGCPriorBC=False))

    recoMS.mergeReco(MuonTrackBuildingCfg(flags, name="TrigMuPatTrackBuilder_"+name, MonTool = MuPatTrackBuilderMonitoring("MuPatTrackBuilderMonitoringSA_"+name)))
    recoMS.mergeReco(MuonTrackParticleCnvCfg(flags, name = "TrigMuonTrackParticleCnvAlg_"+name))
    recoMS.mergeReco(MuonCombinedMuonCandidateAlgCfg(flags, name = "TrigMuonCandidateAlg_"+name))
    recoMS.mergeReco(MuonCreatorAlgCfg(flags, name = "TrigMuonCreatorAlg_"+name, MuonContainerLocation="Muons_"+name))

    selAccMS.mergeReco(recoMS)

    efmuMSHypo = efMuHypoConf( flags,
                              name = 'TrigMuonEFMSonlyHypo_'+name,
                              inputMuons = "Muons_"+name )

    selAccMS.addHypoAlgo(efmuMSHypo)
    
    efmuMSSequence = MenuSequenceCA(selAccMS,
                                    HypoToolGen = TrigMuonEFMSonlyHypoToolFromDict)

    return (selAccMS , efmuMSSequence)

def muEFSAStep(flags, chainDict, name='RoI'):

    selAccMS , efmuMSSequence = _muEFSAStepSeq(flags, name)

    return ChainStep( name=selAccMS.name, Sequences=[efmuMSSequence], chainDicts=[chainDict] )

@AccumulatorCache
def _muEFCBStepSeq(flags, name='RoI'):
    #EF combined muons
    selAccEFCB = SelectionCA("EFCBMuon_"+name)

    viewName = 'EFMuCBReco_'+name                                                       
    trackName = flags.Trigger.InDetTracking.Muon.tracks_FTF
    muonCandName = "MuonCandidates"
    if 'FS' in name:
        muonCandName = "MuonCandidates_FS"
        ViewCreatorCentredOnIParticleROITool=CompFactory.ViewCreatorCentredOnIParticleROITool
        roiTool         = ViewCreatorCentredOnIParticleROITool(RoisWriteHandleKey="MuonCandidates_FS_ROIs")
        viewMakerAlg = CompFactory.EventViewCreatorAlgorithm("IM"+viewName,
                                                             ViewFallThrough = True,
                                                             mergeUsingFeature = True,
                                                             PlaceMuonInView = True,
                                                             RequireParentView = True,
                                                             InViewMuons = "InViewMuons",
                                                             InViewMuonCandidates = muonCandName,
                                                             RoITool         = roiTool,
                                                             InViewRoIs      = viewName+'RoIs',
                                                             Views           = viewName+'Views',
                                                             ViewNodeName    = viewName+"InView")
        recoCB = InViewRecoCA("EFMuCBReco_"+name, viewMaker=viewMakerAlg)
        #ID tracking
        recoCB.mergeReco(trigInDetFastTrackingCfg( flags, roisKey=recoCB.inputMaker().InViewRoIs, signatureName="MuonFS" ))
        trackName = flags.Trigger.InDetTracking.MuonFS.tracks_FTF
    else:
        recoCB = InViewRecoCA(viewName)
        recoCB.inputMaker().RequireParentView = True

    recoCB.mergeReco(EFMuonCBViewDataVerifierCfg(flags, name))
    
    indetCandCfg = MuonCombinedInDetCandidateAlgCfg(flags, name="TrigMuonCombinedInDetCandidateAlg_"+name, TrackParticleLocation=[trackName], 
                                                 InDetCandidateLocation="IndetCandidates_"+name, DoSiliconAssocForwardMuons=False, InDetForwardTrackSelector="")
    recoCB.mergeReco(indetCandCfg)
    muonCombCfg = MuonCombinedAlgCfg(flags, name="TrigMuonCombinedAlg_"+name, MuonCandidateLocation=muonCandName, 
                                     InDetCandidateLocation="IndetCandidates_"+name)
    recoCB.mergeReco(muonCombCfg)

    muonCreatorCBCfg = MuonCreatorAlgCfg(flags, name="TrigMuonCreatorAlgCB_"+name, MuonCandidateLocation=[muonCandName], TagMaps=["muidcoTagMap"], 
                                         InDetCandidateLocation="IndetCandidates_"+name, MuonContainerLocation = "MuonsCB_"+name, SegmentContainerName = "xaodCBSegments", TrackSegmentContainerName = "TrkCBSegments",
                                         ExtrapolatedLocation = "CBExtrapolatedMuons", MSOnlyExtrapolatedLocation = "CBMSonlyExtrapolatedMuons", CombinedLocation = "HLT_CBCombinedMuon_"+name)
    recoCB.mergeReco(muonCreatorCBCfg)

    #Inside out recovery
    finalMuons = "MuonsCB_"+name
    if 'FS' not in name:
        acc = ComponentAccumulator()
        seqIO = seqOR("muonInsideOutSeq")
        acc.addSequence(seqIO)
        seqFilter = seqAND("muonFilterSeq")
        acc.addSequence(seqFilter, seqIO.name)
        muonFilterCfg = MuonFilterAlgCfg(flags, name="FilterZeroMuons", MuonContainerLocation="MuonsCB_"+name)
        acc.merge(muonFilterCfg, sequenceName=seqFilter.name)
        seqIOreco = parOR("muonInsideOutRecoSeq")
        acc.addSequence(seqIOreco, parentName=seqFilter.name)
        recoCB.mergeReco(MuonInsideOutViewDataVerifierCfg(flags))
        inDetExtensionCfg = MuonInDetToMuonSystemExtensionAlgCfg(flags, name="TrigInDetMuonExtensionAlg_"+name, InputInDetCandidates="InDetCandidates_"+name,
                                                                 WriteInDetCandidates="InDetCandidatesSystemExtended_"+name)
        acc.merge(inDetExtensionCfg, sequenceName=seqIOreco.name)
        muonInsideOutCfg = MuonInsideOutRecoAlgCfg(flags, name="TrigMuonInsideOutRecoAlg", InDetCandidateLocation = "InDetCandidatesSystemExtended_"+name)
        acc.merge(muonInsideOutCfg, sequenceName=seqIOreco.name)
        insideOutCreatorAlgCfg = MuonCreatorAlgCfg(flags, name="TrigMuonCreatorAlgInsideOut", TagMaps=["muGirlTagMap"], InDetCandidateLocation="IndetCandidates_"+name,
                                                   MuonContainerLocation = "MuonsInsideOut_"+name, SegmentContainerName = "xaodInsideOutCBSegments", 
                                                   TrackSegmentContainerName = "TrkInsideOutCBSegments", ExtrapolatedLocation = "InsideOutCBExtrapolatedMuons",
                                                   MSOnlyExtrapolatedLocation = "InsideOutCBMSOnlyExtrapolatedMuons", CombinedLocation = "InsideOutCBCombinedMuon")
        acc.merge(insideOutCreatorAlgCfg, sequenceName=seqIOreco.name)

        finalMuons = "MuonsCBMerged"
        muonMergeCfg = MergeEFMuonsAlgCfg(flags, name="MergeEFMuons", MuonCBContainerLocation = "MuonsCB_"+name,
                                          MuonInsideOutContainerLocation = "MuonsInsideOut_"+name, MuonOutputLocation = finalMuons)
        acc.merge(muonMergeCfg, sequenceName=seqIO.name)
        recoCB.mergeReco(acc)
    
    selAccEFCB.mergeReco(recoCB)

    efmuCBHypo = efMuHypoConf( flags,
                              name = 'TrigMuonEFCBHypo_'+name,
                              inputMuons = finalMuons )

    selAccEFCB.addHypoAlgo(efmuCBHypo)

    efmuCBSequence = MenuSequenceCA(selAccEFCB,
                                    HypoToolGen = TrigMuonEFCombinerHypoToolFromDict)
   
    return (selAccEFCB , efmuCBSequence)

def muEFCBStep(flags, chainDict, name='RoI'):

    selAccEFCB , efmuCBSequence = _muEFCBStepSeq(flags, name)
    
    return ChainStep( name=selAccEFCB.name, Sequences=[efmuCBSequence], chainDicts=[chainDict] )

@AccumulatorCache
def _muEFIsoStepSeq(flags):
    #Track isolation
    selAccEFIso = SelectionCA("EFIsoMuon")

    viewName = 'EFMuIsoReco'                                                       
    ViewCreatorCentredOnIParticleROITool=CompFactory.ViewCreatorCentredOnIParticleROITool
    roiTool         = ViewCreatorCentredOnIParticleROITool(RoisWriteHandleKey="MuonIso_ROIs", RoIEtaWidth=0.15, RoIPhiWidth=0.15)
    viewMakerAlg = CompFactory.EventViewCreatorAlgorithm("IM"+viewName,
                                                         ViewFallThrough = True,
                                                         mergeUsingFeature = True,
                                                         PlaceMuonInView = True,
                                                         RequireParentView = True,
                                                         InViewMuons = "InViewIsoMuons",
                                                         InViewMuonCandidates = "IsoMuonCandidates",
                                                         RoITool         = roiTool,
                                                         InViewRoIs      = viewName+'RoIs',
                                                         Views           = viewName+'Views',
                                                         ViewNodeName    = viewName+"InView")
    recoIso = InViewRecoCA("EFMuIsoReco", viewMaker=viewMakerAlg)
    #ID tracking
    recoIso.mergeReco(trigInDetFastTrackingCfg( flags, roisKey=recoIso.inputMaker().InViewRoIs, signatureName="MuonIso" ))
    recoIso.mergeReco(MuIsoViewDataVerifierCfg())
    recoIso.mergeReco(TrigMuonEFTrackIsolationAlgCfg(flags, IdTrackParticles=flags.Trigger.InDetTracking.MuonIso.tracks_FTF,
                                                    MuonEFContainer="InViewIsoMuons", 
                                                    ptcone02Name="InViewIsoMuons.ptcone02", 
                                                    ptcone03Name="InViewIsoMuons.ptcone03"))

    selAccEFIso.mergeReco(recoIso)
    efmuIsoHypo = efMuIsoHypoConf( flags,
                                  name = 'TrigMuonIsoHypo',
                                  inputMuons = "MuonsIso" )
    selAccEFIso.addHypoAlgo(efmuIsoHypo)

    efmuIsoSequence = MenuSequenceCA(selAccEFIso,
                                     HypoToolGen = TrigMuonEFTrackIsolationHypoToolFromDict)
    
    return (selAccEFIso , efmuIsoSequence)

def muEFIsoStep(flags, chainDict):
    selAccEFIso , efmuIsoSequence = _muEFIsoStepSeq(flags)
    
    return ChainStep( name=selAccEFIso.name, Sequences=[efmuIsoSequence], chainDicts=[chainDict] )

def generateChains( flags, chainDict ):
    chainDict = splitChainDict(chainDict)[0]

    #Clone and replace offline flags so we can set muon trigger specific values
    muonflags = flags.cloneAndReplace('Muon', 'Trigger.Offline.SA.Muon')


    l1Thresholds=[]
    for part in chainDict['chainParts']:
        l1Thresholds.append(part['L1threshold'])

    log.debug('dictionary is: %s\n', pprint.pformat(chainDict))
    def _empty(name):
        return ChainStep(name="EmptyNoL2MuComb", Sequences=[EmptyMenuSequence("EmptyNoL2MuComb")], chainDicts=[chainDict])
    if 'noL1' in chainDict['chainName']:
        muonflagsCB = flags.cloneAndReplace('Muon', 'Trigger.Offline.Muon')
        chain = Chain(name=chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[muEFSAStep(muonflags, chainDict, 'FS'), muEFCBStep(muonflagsCB, chainDict, 'FS')])
    else:
        if 'msonly' in chainDict['chainName']:
            chain = Chain( name=chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[ muFastStep(muonflags, chainDict), _empty("EmptyNoL2MuComb"), muEFSAStep(muonflags, chainDict)] )
        else:
            muonflagsCB = flags.cloneAndReplace('Muon', 'Trigger.Offline.Muon').cloneAndReplace('MuonCombined', 'Trigger.Offline.Combined.MuonCombined')
            if 'ivar' in chainDict['chainName']:
                chain = Chain( name=chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[ muFastStep(muonflags, chainDict), muCombStep(muonflagsCB, chainDict), muEFSAStep(muonflags, chainDict), muEFCBStep(muonflagsCB, chainDict), muEFIsoStep(muonflagsCB, chainDict) ] )
            else:
                chain = Chain( name=chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[ muFastStep(muonflags, chainDict), muCombStep(muonflagsCB, chainDict), muEFSAStep(muonflags, chainDict), muEFCBStep(muonflagsCB, chainDict) ] )
    return chain

