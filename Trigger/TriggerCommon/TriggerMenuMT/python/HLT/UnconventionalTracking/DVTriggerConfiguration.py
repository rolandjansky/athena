# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.CFElements import seqAND
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.Logging import logging

logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

trkPairOutName = "HLT_TrigDV_VSITrkPair"
vtxOutName = "HLT_TrigDV_VSIVertex"
vtxCountName = "HLT_TrigDV_VtxCount"

def DVRecoFragment(ConfigFlags):

    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    FSConfig = getInDetTrigConfig("jet")
    LRTConfig = getInDetTrigConfig( "DVtxLRT" )

    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
    from DecisionHandling.DecisionHandlingConf import (ViewCreatorDVROITool)
    from TrigEDMConfig.TriggerEDMRun3 import recordable
    inputMakerAlg = EventViewCreatorAlgorithm(
        "IMDVRoILRT",
        mergeUsingFeature = False,
        RoITool = ViewCreatorDVROITool(
            'ViewCreatorDVRoI',
            RoisWriteHandleKey  = recordable( LRTConfig.roi ),
            RoIEtaWidth = LRTConfig.etaHalfWidth,
            RoIPhiWidth = LRTConfig.phiHalfWidth,
        ),
        Views = "DVRoIViews",
        InViewRoIs = "InViewRoIs",
        RequireParentView = False,
        ViewFallThrough = True)

    from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTracking
    viewAlgs, viewVerify = makeInDetTrigFastTracking( config = LRTConfig, LRTInputCollection = FSConfig.trkTracks_FTF(), rois=inputMakerAlg.InViewRoIs )

    viewVerify.DataObjects += [ ( 'TrigRoiDescriptorCollection',  'StoreGateSvc+%s' % inputMakerAlg.InViewRoIs ),
                                ( 'TrackCollection', 'StoreGateSvc+%s' % FSConfig.trkTracks_FTF() ),
                                ( 'xAOD::TrackParticleContainer', 'StoreGateSvc+%s' % FSConfig.tracks_FTF()),
                                ( 'xAOD::VertexContainer', 'StoreGateSvc+%s' % FSConfig.vertex ) ]


    from TrigEDMConfig.TriggerEDMRun3 import recordable
    from TrigVrtSecInclusive.TrigVrtSecInclusiveConfig import TrigVrtSecInclusiveCfg
    vertexingAlgs = TrigVrtSecInclusiveCfg( "TrigVrtSecInclusive_TrigDV",
                                            FirstPassTracksName = FSConfig.tracks_FTF(),
                                            SecondPassTracksName = LRTConfig.tracks_FTF(),
                                            PrimaryVertexInputName = FSConfig.vertex,
                                            VxCandidatesOutputName = recordable(vtxOutName),
                                            TrkPairOutputName = recordable(trkPairOutName) )

    recoAlgSequence = seqAND("DVRecoSequence", [viewAlgs, vertexingAlgs])
    inputMakerAlg.ViewNodeName = recoAlgSequence.name()

    algSequence = seqAND("TrigDVRecoSequence", [inputMakerAlg, recoAlgSequence])
    sequenceOut = vtxOutName

    return (algSequence, inputMakerAlg, sequenceOut)




def DVRecoSequence():
    from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlg
    from TrigStreamerHypo.TrigStreamerHypoConfig import StreamerHypoToolGenerator

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ( TrkSeq, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(DVRecoFragment,ConfigFlags)

    HypoAlg = TrigStreamerHypoAlg("TrigDVRecoDummyStream")

    log.debug("Building the Step dictinary for TrigDV reco")
    return MenuSequence( Sequence    = TrkSeq,
                        Maker       = InputMakerAlg,
                        Hypo        = HypoAlg,
                        HypoToolGen = StreamerHypoToolGenerator
                        )




def DVTriggerEDSequence():
    from TrigLongLivedParticlesHypo.TrigVrtSecInclusiveHypoConfig import TrigVSIHypoToolFromDict
    from TrigLongLivedParticlesHypo.TrigVrtSecInclusiveHypoConfig import createTrigVSIHypoAlg

    theHypoAlg = createTrigVSIHypoAlg("TrigDVHypoAlg")

    from TrigEDMConfig.TriggerEDMRun3 import recordable
    theHypoAlg.verticesKey = vtxOutName
    theHypoAlg.vtxCountKey = recordable(vtxCountName)

    #run at the event level
    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
    from AthenaConfiguration.ComponentFactory import CompFactory
    inputMakerAlg = conf2toConfigurable(CompFactory.InputMakerForRoI( "IM_TrigDV_ED" ))
    inputMakerAlg.RoITool = conf2toConfigurable(CompFactory.ViewCreatorInitialROITool())

    log.info("Building the Step dictinary for DisVtxTrigger!")
    return MenuSequence( Sequence    = seqAND("TrigDVEDEmptyStep",[inputMakerAlg]),
                        Maker       = inputMakerAlg,
                        Hypo        = theHypoAlg,
                        HypoToolGen = TrigVSIHypoToolFromDict,
                        )
