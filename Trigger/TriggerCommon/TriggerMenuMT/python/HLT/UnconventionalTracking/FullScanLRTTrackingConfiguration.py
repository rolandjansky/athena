# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.CFElements import parOR
from ..CommonSequences.FullScanDefs import trkFSRoI
from TriggerMenuMT.HLT.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.Logging import logging

logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)


def FullScanLRTTriggerSequence(ConfigFlags):
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    fscfg = getInDetTrigConfig("jet")
    lrtcfg = getInDetTrigConfig( 'fullScanLRT' )

    from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTrackingNoView
    ft_reco_algs = makeInDetTrigFastTrackingNoView( config = fscfg, rois=trkFSRoI, secondStageConfig = lrtcfg)

    from TriggerMenuMT.HLT.Jet.JetMenuSequences import getTrackingInputMaker
    im_alg = getTrackingInputMaker("ftf")

    from TrigInDetConfig.InDetTrigPrecisionTracking import makeInDetTrigPrecisionTracking

    tracks_name, track_particles_names, pt_reco_algs = makeInDetTrigPrecisionTracking(config = lrtcfg, rois = trkFSRoI)


    TrkSeq = parOR("UncTrkrecoSeqFSLRT", [im_alg, ft_reco_algs, pt_reco_algs])
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
