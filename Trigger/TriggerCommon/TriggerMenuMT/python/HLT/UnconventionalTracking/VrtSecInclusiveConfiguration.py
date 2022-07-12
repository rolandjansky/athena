# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.CFElements import parOR
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.Logging import logging

logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)


def VrtSecInclusiveSequence(ConfigFlags):
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    fscfg = getInDetTrigConfig("jet")
    lrtcfg = getInDetTrigConfig("fullScanLRT")

    from TriggerMenuMT.HLT.UnconventionalTracking.CommonConfiguration import getCommonInDetFullScanLRTSequence

    ftf_seqs, im_alg, seqOut = RecoFragmentsPool.retrieve(getCommonInDetFullScanLRTSequence,ConfigFlags)

    from TrigVrtSecInclusive.TrigVrtSecInclusiveConfig import TrigVrtSecInclusiveCfg
    theVSI = TrigVrtSecInclusiveCfg("TrigVrtSecInclusive", fscfg.tracks_FTF(), lrtcfg.tracks_FTF(), fscfg.vertex, "HLT_TrigVSIVertex", "HLT_TrigVSITrkPair")
    theVSI.recordTrkPair = False
    vtx_reco_algs = [theVSI]

    vsiseq = parOR("UncTrkrecoSeqVSI", [vtx_reco_algs])
    TrkSeq = parOR("UncTrkrecoSeqLRTVSI", [ftf_seqs, vsiseq])
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
    theHypoAlg.isViewBased = False

    log.info("Building the Step dictinary for TrigVSI!")
    return MenuSequence( Sequence    = TrkSeq,
                        Maker       = im_alg,
                        Hypo        = theHypoAlg,
                        HypoToolGen = TrigVSIHypoToolFromDict,
                        )
