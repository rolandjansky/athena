# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import (seqAND, parOR)
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence
from AthenaCommon.Logging import logging

from ..Config.MenuComponents import RecoFragmentsPool
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from TrigEDMConfig.TriggerEDMRun3 import recordable
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorCentredOnIParticleROITool
from TrigInDetConfig.ConfigSettings import getInDetTrigConfig

logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

def DJPromptStep():
    from TrigLongLivedParticlesHypo.TrigDJHypoConfig import (TrigDJHypoPromptToolFromDict)
    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import (DisplacedJetPromptHypoAlg)

    hypo_alg = DisplacedJetPromptHypoAlg("DJTrigPromptHypoAlg")

    #get the jet tracking config to get the track collection name
    fscfg = getInDetTrigConfig("jet")

    hypo_alg.min_trk_pt = 1.0
    hypo_alg.min_evt_jet_pt = 180
    hypo_alg.stdTracksKey = fscfg.tracks_FTF()
    hypo_alg.jetContainerKey = recordable("HLT_AntiKt4EMTopoJets_subjesIS")
    hypo_alg.vtxKey = fscfg.vertex_jet
    hypo_alg.countsKey = "DispJetTrigger_Counts"

    #run at the event level
    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
    from AthenaConfiguration.ComponentFactory import CompFactory
    im_alg = conf2toConfigurable(CompFactory.InputMakerForRoI( "IM_DJTRIG_Prompt" ))
    im_alg.RoITool = conf2toConfigurable(CompFactory.ViewCreatorInitialROITool())

    return MenuSequence( Sequence    = seqAND("DJTrigPromptEmptyStep",[im_alg]),
                         Maker       = im_alg,
                         Hypo        = hypo_alg,
                         HypoToolGen = TrigDJHypoPromptToolFromDict,
                     )

def DJEDStep():
    from TrigLongLivedParticlesHypo.TrigDJHypoConfig import (TrigDJHypoEDToolFromDict)
    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import (DisplacedJetEventDecisionHypoAlg)

    hypo_alg = DisplacedJetEventDecisionHypoAlg("DJTrigEventDecisionHypoAlg")

    #run at the event level
    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
    from AthenaConfiguration.ComponentFactory import CompFactory
    im_alg = conf2toConfigurable(CompFactory.InputMakerForRoI( "IM_DJTRRIG_ED" ))
    im_alg.RoITool = conf2toConfigurable(CompFactory.ViewCreatorInitialROITool())
    im_alg.mergeUsingFeature = True #need to merge using the jet feature which is attached not the roi which is the same

    return MenuSequence( Sequence    = seqAND("DJTrigEDEmptyStep",[im_alg]),
                         Maker       = im_alg,
                         Hypo        = hypo_alg,
                         HypoToolGen = TrigDJHypoEDToolFromDict,
                     )

def DJDispFragment(ConfigFlags):
    lrtcfg = getInDetTrigConfig( 'DJetLRT' )
    roiTool = ViewCreatorCentredOnIParticleROITool('ViewCreatorDJRoI', RoisWriteHandleKey = recordable(lrtcfg.roi), RoIEtaWidth = 0.4, RoIPhiWidth = 0.4)
    InputMakerAlg = EventViewCreatorAlgorithm("IMDJRoIFTF",mergeUsingFeature = True, RoITool = roiTool,Views = "DJRoIViews",InViewRoIs = "InViewRoIs",RequireParentView = False,ViewFallThrough = True)
    fscfg = getInDetTrigConfig("jet")
    from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTracking

    algs, view_verify = makeInDetTrigFastTracking(config=lrtcfg, LRTInputCollection = fscfg.trkTracks_FTF(), rois = InputMakerAlg.InViewRoIs)

    reco_seq = parOR("UncTrkrecoSeqDJTrigDispRecoSeq", algs)

    view_verify.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % InputMakerAlg.InViewRoIs )]
    view_verify.DataObjects += [( 'TrackCollection' , 'StoreGateSvc+%s' % fscfg.trkTracks_FTF() )]

    InputMakerAlg.ViewNodeName = reco_seq.name()

    return (seqAND("UncTrkrecoSeqDJTrigDisp", [InputMakerAlg, reco_seq]), InputMakerAlg)

def DJDispStep():
    from TrigLongLivedParticlesHypo.TrigDJHypoConfig import TrigDJHypoDispToolFromDict
    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import (DisplacedJetDispHypoAlg)

    hypo_alg = DisplacedJetDispHypoAlg("DJTrigDispHypoAlg")

    lrtcfg = getInDetTrigConfig( 'DJetLRT' )
    fscfg = getInDetTrigConfig("jet")

    hypo_alg.lrtTracksKey = lrtcfg.tracks_FTF()
    hypo_alg.vtxKey = fscfg.vertex_jet
    hypo_alg.infoKey = "DispJetTrigger_Info"

    ( alg_seq ,im_alg) = RecoFragmentsPool.retrieve(DJDispFragment,ConfigFlags)

    return MenuSequence(Sequence    = alg_seq,
                        Maker       = im_alg,
                        Hypo        = hypo_alg,
                        HypoToolGen = TrigDJHypoDispToolFromDict,
                        )


def StdFTFTrackSequence(ConfigFlags):
    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
    from AthenaConfiguration.ComponentFactory import CompFactory
    from ..CommonSequences.FullScanDefs import caloFSRoI
    im_alg = conf2toConfigurable(CompFactory.InputMakerForRoI( "IM_DJTRIG_STDFTF" ))
    im_alg.RoITool = conf2toConfigurable(CompFactory.ViewCreatorInitialROITool())

    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( 'jet' )

    from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTrackingNoView
    TrkInputNoViewAlg = makeInDetTrigFastTrackingNoView( config=IDTrigConfig, rois=caloFSRoI )

    from TrigInDetConfig.InDetTrigVertices import makeInDetTrigVertices
    
    vtxAlgs = makeInDetTrigVertices( "jet", IDTrigConfig.tracks_FTF(), IDTrigConfig.vertex_jet, IDTrigConfig, adaptiveVertex=IDTrigConfig.adaptiveVertex_jet)
    prmVtx = vtxAlgs[-1]

    TrkSeq =  [im_alg,TrkInputNoViewAlg, prmVtx]
    sequenceOut = IDTrigConfig.tracks_FTF()

    return (TrkSeq, im_alg, sequenceOut)

def FTFRecoOnlySequence():
    from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlg
    from TrigStreamerHypo.TrigStreamerHypoConfig import StreamerHypoToolGenerator

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ( TrkSeq, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(StdFTFTrackSequence,ConfigFlags)

    HypoAlg = TrigStreamerHypoAlg("UncTrkDJDummyStream")


    log.debug("Building the Step dictinary for Displaced Jet Trigger!")
    return MenuSequence( Sequence    = seqAND("UncTrkDJStdFTFRecoSeq", TrkSeq),
                        Maker       = InputMakerAlg,
                        Hypo        = HypoAlg,
                        HypoToolGen = StreamerHypoToolGenerator
                        )