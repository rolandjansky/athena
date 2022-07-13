# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.CFElements import parOR
from ..CommonSequences.FullScanDefs import trkFSRoI
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.Logging import logging

logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)


def FullScanLRTTriggerSequence(ConfigFlags):
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    lrtcfg = getInDetTrigConfig("fullScanLRT")

    from TriggerMenuMT.HLT.UnconventionalTracking.CommonConfiguration import getCommonInDetFullScanLRTSequence

    ftf_seqs, im_alg, seqOut = RecoFragmentsPool.retrieve(getCommonInDetFullScanLRTSequence,ConfigFlags)


    from TrigInDetConfig.InDetTrigPrecisionTracking import makeInDetTrigPrecisionTracking

    tracks_name, track_particles_names, pt_reco_algs = makeInDetTrigPrecisionTracking(config = lrtcfg, rois = trkFSRoI)


    pt_seq = parOR("UncTrkrecoSeqfslrtpt", [pt_reco_algs])

    TrkSeq = parOR("UncTrkrecoSeqFSLRT", [ftf_seqs, pt_seq])
    sequenceOut = track_particles_names[0]

    return (TrkSeq,im_alg, sequenceOut)





def FullScanLRTTriggerMenuSequence():
    from TrigLongLivedParticlesHypo.TrigFullScanLRTHypoTool import TrigLRTHypoToolFromDict
    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import (FastTrackFinderLRTHypoAlg)

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ( TrkSeq,im_alg, sequenceOut) = RecoFragmentsPool.retrieve(FullScanLRTTriggerSequence,ConfigFlags)

    theHypoAlg = FastTrackFinderLRTHypoAlg("FullScanLRTHypoAlg")

    from TrigEDMConfig.TriggerEDMRun3 import recordable
    theHypoAlg.trackCountKey=recordable("HLT_FSLRT_TrackCount")
    theHypoAlg.tracksKey =  recordable(sequenceOut)


    log.info("Building the Step dictinary for FullScanLRT!")
    return MenuSequence( Sequence    = TrkSeq,
                        Maker       = im_alg,
                        Hypo        = theHypoAlg,
                        HypoToolGen = TrigLRTHypoToolFromDict,
                        )
