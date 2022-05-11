# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.CFElements import parOR
from ..CommonSequences.FullScanDefs import trkFSRoI
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.Logging import logging

logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)


def VrtSecInclusiveSequence(ConfigFlags):
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    fscfg = getInDetTrigConfig("jet")
    lrtcfg = getInDetTrigConfig( 'fullScanLRT' )

    from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTrackingNoView
    ft_reco_algs = makeInDetTrigFastTrackingNoView( config = fscfg, rois=trkFSRoI, secondStageConfig = lrtcfg)

    from TriggerMenuMT.HLT.Jet.JetMenuSequences import getTrackingInputMaker
    im_alg = getTrackingInputMaker("ftf")

    from TrigVrtSecInclusive.TrigVrtSecInclusiveConfig import TrigVrtSecInclusiveCfg
    theVSI = TrigVrtSecInclusiveCfg("TrigVrtSecInclusive", fscfg.tracks_FTF(), lrtcfg.tracks_FTF(), fscfg.vertex, "HLT_TrigVSIVertex", "HLT_TrigVSITrkPair")
    vtx_reco_algs = [theVSI]

    TrkSeq = parOR("UncTrkrecoSeqVSI", [im_alg, ft_reco_algs, vtx_reco_algs])
    sequenceOut = "HLT_TrigVSIVertex"

    return (TrkSeq, im_alg, sequenceOut)




def VrtSecInclusiveMenuSequence():
    from TrigLongLivedParticlesHypo.TrigVrtSecInclusiveHypoConfig import TrigVSIHypoToolFromDict
    from TrigLongLivedParticlesHypo.TrigVrtSecInclusiveHypoConfig import createTrigVSIHypoAlg

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ( TrkSeq, im_alg, sequenceOut) = RecoFragmentsPool.retrieve(VrtSecInclusiveSequence,ConfigFlags)

    theHypoAlg = createTrigVSIHypoAlg("TrigVSIHypoAlg")

    from TrigEDMConfig.TriggerEDMRun3 import recordable
    theHypoAlg.verticesKey = recordable(sequenceOut)

    log.info("Building the Step dictinary for TrigVSI!")
    return MenuSequence( Sequence    = TrkSeq,
                        Maker       = im_alg,
                        Hypo        = theHypoAlg,
                        HypoToolGen = TrigVSIHypoToolFromDict,
                        )
