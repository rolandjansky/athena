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

def DJDispFragment(ConfigFlags):
    lrtcfg = getInDetTrigConfig( 'DJetLRT' )
    roiTool = ViewCreatorCentredOnIParticleROITool('ViewCreatorDJRoI', RoisWriteHandleKey = recordable(lrtcfg.roi), RoIEtaWidth = lrtcfg.etaHalfWidth, RoIPhiWidth = lrtcfg.phiHalfWidth)
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

    ( alg_seq ,im_alg) = RecoFragmentsPool.retrieve(DJDispFragment,ConfigFlags)

    return MenuSequence(Sequence    = alg_seq,
                        Maker       = im_alg,
                        Hypo        = hypo_alg,
                        HypoToolGen = TrigDJHypoDispToolFromDict,
                        )
