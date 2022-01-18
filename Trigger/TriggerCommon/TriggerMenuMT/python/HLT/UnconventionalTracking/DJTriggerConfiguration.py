# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import (seqAND, parOR)
from TriggerMenuMT.HLT.Menu.MenuComponents import MenuSequence
from AthenaCommon.Logging import logging

from ..Menu.MenuComponents import RecoFragmentsPool
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from TrigEDMConfig.TriggerEDMRun3 import recordable
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorCentredOnIParticleROITool
from TrigInDetConfig.ConfigSettings import getInDetTrigConfig

logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

def DJPromptStep():
    from TrigLongLivedParticlesHypo.TrigDJHypoConfig import (TrigDJHypoPromptToolFromDict, TrigDJPromptHypoAlgMonTool)
    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import (DisplacedJetPromptHypoAlg)

    hypo_alg = DisplacedJetPromptHypoAlg("DJTrigPromptHypoAlg")

    #get the jet tracking config to get the track collection name
    fscfg = getInDetTrigConfig("jet")

    hypo_alg.min_trk_pt = 2
    hypo_alg.min_evt_jet_pt = 200
    hypo_alg.stdTracksKey = fscfg.tracks_FTF()
    hypo_alg.jetContainerKey = recordable("HLT_AntiKt4EMTopoJets_subjesIS")
    hypo_alg.vtxKey = fscfg.vertex_jet
    hypo_alg.MonTool = TrigDJPromptHypoAlgMonTool()

    #run at the event level
    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
    from AthenaConfiguration.ComponentFactory import CompFactory
    DummyInputMakerAlg = conf2toConfigurable(CompFactory.InputMakerForRoI( "IM_DJTRIG_PromptDummy" ))
    DummyInputMakerAlg.RoITool = conf2toConfigurable(CompFactory.ViewCreatorInitialROITool())

    return MenuSequence( Sequence    = seqAND("DJTrigPromptEmptyStep",[DummyInputMakerAlg]),
                         Maker       = DummyInputMakerAlg,
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
    DummyInputMakerAlg = conf2toConfigurable(CompFactory.InputMakerForRoI( "IM_DJTRRIG_EDDummy" ))
    DummyInputMakerAlg.RoITool = conf2toConfigurable(CompFactory.ViewCreatorInitialROITool())

    return MenuSequence( Sequence    = seqAND("DJTrigEDEmptyStep",[DummyInputMakerAlg]),
                         Maker       = DummyInputMakerAlg,
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

    ( alg_seq ,im_alg) = RecoFragmentsPool.retrieve(DJDispFragment,ConfigFlags)

    return MenuSequence(Sequence    = alg_seq,
                        Maker       = im_alg,
                        Hypo        = hypo_alg,
                        HypoToolGen = TrigDJHypoDispToolFromDict,
                        )
