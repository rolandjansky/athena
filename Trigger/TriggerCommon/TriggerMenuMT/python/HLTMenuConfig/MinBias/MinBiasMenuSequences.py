#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from AthenaCommon.CFElements import parOR
from AthenaCommon.CFElements import seqAND
from TrigInDetConfig.InDetSetup import makeInDetAlgs
from TrigEDMConfig.TriggerEDMRun3 import recordable
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorInitialROITool
from AthenaCommon.GlobalFlags import globalflags
from TrigInDetConfig.ConfigSettings import getInDetTrigConfig


########
# to move into TrigMinBiasHypoConfigMT?

def SPCountHypoToolGen(chainDict):        
    from TrigT2MinBias.TrigT2MinBiasConf import SPCountHypoTool
    hypo = SPCountHypoTool(chainDict["chainName"])
    if "hmt" in chainDict["chainName"]:
        hypo.totNumSctSP = int( chainDict["chainParts"][0]["hypoL2Info"].strip("sp") )
    if "mb_sptrk" in chainDict["chainName"]:
        hypo.totNumPixSP  = 2
        hypo.totNumSctSP  = 3
            # will set here thresholds
    return hypo



def TrackCountHypoToolGen(chainDict):
    from TrigMinBias.TrigMinBiasConf import  TrackCountHypoTool
    hypo = TrackCountHypoTool(chainDict["chainName"])
    if "hmt" in chainDict["chainName"]:
        hypo.required_ntrks = int( chainDict["chainParts"][0]["hypoEFInfo"].strip("trk") )
    if "mb_sptrk" in chainDict["chainName"]:
        hypo.min_pt  = 0.2
        hypo.max_z0  = 401
        # will set here cuts
    return hypo


### Now the sequences

# These are obsoletee and can probably be deleted. Leeft here commented as need feedback fomr experts

## def minbiasSpacePointMenuSequence():
##     # menu components
##     # retrieve the reco seuqnece
##     from TriggerMenuMT.HLTMenuConfig.MinBias.MinBiasSPRecoSequences import minbiasSpacePointAthSequence
##     ( minbiasSpacePointSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(minbiasSpacePointAthSequence,ConfigFlags)

##     #hypo
##     mbHypoAlg = MbCountHypoAlgMT("MinBiasHypoAlg_sp")
##     mbHypoAlg.MinBiasContainerKey=sequenceOut

##     return  MenuSequence( Sequence    = minbiasSpacePointSequence,
##                           Maker       = InputMakerAlg,
##                           Hypo        = mbHypoAlg,
##                           HypoToolGen = TrigMinBiasHypoToolFromDict )


## #obsolete?
## def minbiasTrackMenuSequence():
##     # menu components
##     # retrieve the reco seuqnece
##     from TriggerMenuMT.HLTMenuConfig.MinBias.MinBiasTrkRecoSequences import minbiasTrackAthSequence
##     (minbiasTrackSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(minbiasTrackAthSequence,ConfigFlags)

##     #hypo
##     mbHypoAlg = MbCountHypoAlgMT("MinBiasHypoAlg_trk")
##     mbHypoAlg.MinBiasContainerKey=sequenceOut

##     return  MenuSequence( Sequence    = minbiasTrackSequence,
##                           Maker       = InputMakerAlg,
##                           Hypo        = mbHypoAlg,
##                           HypoToolGen = TrigMinBiasHypoToolFromDict )


# NEW:
def MinBiasSPSequence():
    SpList = []
    from TrigT2MinBias.TrigT2MinBiasConf import TrigCountSpacePointsMT, SPCountHypoAlgMT

    SPInputMakerAlg = EventViewCreatorAlgorithm("IM_SPEventViewCreator")
    SPInputMakerAlg.ViewFallThrough = True
    SPInputMakerAlg.RoITool = ViewCreatorInitialROITool()
    SPInputMakerAlg.InViewRoIs = "InputRoI"
    SPInputMakerAlg.Views = "SPView"

    IDTrigConfig = getInDetTrigConfig( 'minBias' )
    idAlgs, verifier = makeInDetAlgs(  config = IDTrigConfig, rois=SPInputMakerAlg.InViewRoIs, viewVerifier='SPViewDataVerifier' )
    verifier.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+InputRoI' ),
                                 ( 'SCT_ID' , 'DetectorStore+SCT_ID' ),
                                 ( 'PixelID' , 'DetectorStore+PixelID' ),
                                 ( 'TagInfo' , 'DetectorStore+ProcessingTags' )]

    # Make sure required objects are still available at whole-event level
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence.SGInputLoader.Load += [( 'SCT_ID' , 'DetectorStore+SCT_ID' ),
                                           ( 'PixelID' , 'DetectorStore+PixelID' ),
                                           ( 'TagInfo' , 'DetectorStore+ProcessingTags' )]
    
    SpList = idAlgs[:-2]
    
    SpCount=TrigCountSpacePointsMT()
    SpCount.SpacePointsKey=recordable("HLT_SpacePointCounts")
    
    from TrigT2MinBias.TrigT2MinBiasMonitoringMT import SpCountMonitoring
    SpCount.MonTool = SpCountMonitoring()
    
    SPrecoSeq = parOR("SPrecoSeq", SpList + [ SpCount ])
    SPSequence = seqAND("SPSequence", [SPInputMakerAlg, SPrecoSeq])
    SPInputMakerAlg.ViewNodeName = SPrecoSeq.name()
    
    
    SpCountHypo =SPCountHypoAlgMT()
    SpCountHypo.SpacePointsKey=recordable("HLT_SpacePointCounts")
    
    return MenuSequence( Sequence   = SPSequence,
                        Maker       = SPInputMakerAlg,
                        Hypo        = SpCountHypo,
                        HypoToolGen = SPCountHypoToolGen )


def MinBiasTrkSequence():
        from TrigMinBias.TrigMinBiasConf import TrackCountHypoAlgMT

        TrkInputMakerAlg = EventViewCreatorAlgorithm("IM_TrkEventViewCreator")
        TrkInputMakerAlg.ViewFallThrough = True
        TrkInputMakerAlg.RoITool = ViewCreatorInitialROITool()
        TrkInputMakerAlg.InViewRoIs = "InputRoI" # contract with the consumer
        TrkInputMakerAlg.Views = "TrkView"
        TrkInputMakerAlg.RequireParentView = True
        TrkInputMakerAlg.ViewNodeName = "TrkCountHypoAlgMTNode"

        # prepare algorithms to run in views, first, inform scheduler that input data is available in parent view (has to be done by hand)
        IDTrigConfig = getInDetTrigConfig( 'minBias' )
        idAlgs, verifier = makeInDetAlgs( config = IDTrigConfig, rois=TrkInputMakerAlg.InViewRoIs, viewVerifier='TrkrecoSeqDataVerifier')
        verifier.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+InputRoI' ),
                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_FlaggedCondData_TRIG' ),
                                 ( 'InDet::SCT_ClusterContainer' , 'StoreGateSvc+SCT_TrigClusters' ),
                                 ( 'SpacePointContainer' , 'StoreGateSvc+SCT_TrigSpacePoints' ),
                                 ( 'InDet::PixelClusterContainer' , 'StoreGateSvc+PixelTrigClusters' ),
                                 ( 'SpacePointContainer' , 'StoreGateSvc+PixelTrigSpacePoints' )]

        if globalflags.InputFormat.is_bytestream():
          verifier.DataObjects += [( 'IDCInDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' ),
                                   ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' )]


        TrkList = idAlgs[-2:] # FTF and Track to xAOD::TrackParticle conversion alg
        TrackCountHypo=TrackCountHypoAlgMT()
        TrackCountHypo.trackCountKey=recordable("HLT_TrackCount")
        TrackCountHypo.tracksKey=recordable("HLT_IDTrack_MinBias_FTF")

        from TrigMinBias.TrackCountMonitoringMT import TrackCountMonitoring
        TrackCountHypo.MonTool = TrackCountMonitoring()

        TrkrecoSeq = parOR("TrkrecoSeq", [verifier]+TrkList)
        TrkSequence = seqAND("TrkSequence", [TrkInputMakerAlg, TrkrecoSeq])
        TrkInputMakerAlg.ViewNodeName = TrkrecoSeq.name()

        return MenuSequence( Sequence   = TrkSequence,
                            Maker       = TrkInputMakerAlg,
                            Hypo        = TrackCountHypo,
                            HypoToolGen = TrackCountHypoToolGen )


