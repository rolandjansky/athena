# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import seqAND, parOR
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.Logging import logging

logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

# ---------------------

def getCommonInDetFullScanSequence(ConfigFlag):

    from TriggerMenuMT.HLT.Jet.JetMenuSequences import getTrackingInputMaker
    InputMakerAlg=getTrackingInputMaker("ftf")

    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig("jet")

    from ..CommonSequences.FullScanDefs import trkFSRoI
    from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTrackingNoView
    TrkInputNoViewAlg = makeInDetTrigFastTrackingNoView(config=IDTrigConfig, rois=trkFSRoI)

    from TrigInDetConfig.InDetTrigVertices import makeInDetTrigVertices
    
    vtxAlgs = makeInDetTrigVertices("jet", IDTrigConfig.tracks_FTF(), IDTrigConfig.vertex_jet, IDTrigConfig, adaptiveVertex=IDTrigConfig.adaptiveVertex_jet)
    prmVtx = vtxAlgs[-1]

    TrkSeq = [InputMakerAlg,TrkInputNoViewAlg, prmVtx]
    sequenceOut = IDTrigConfig.tracks_FTF()

    return (TrkSeq, InputMakerAlg, sequenceOut)

# ---------------------

def getFullScanRecoOnlySequence():

    from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlg
    from TrigStreamerHypo.TrigStreamerHypoConfig import StreamerHypoToolGenerator

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ( TrkSeq, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(getCommonInDetFullScanSequence,ConfigFlags)

    HypoAlg = TrigStreamerHypoAlg("UncTrkDummyStream")

    log.debug("Building the menu sequence for FullScanRecoOnlySequence")
    return MenuSequence( Sequence    = seqAND("UncTrkrecoSeq", TrkSeq),
                         Maker       = InputMakerAlg,
                         Hypo        = HypoAlg,
                         HypoToolGen = StreamerHypoToolGenerator )

def getCommonInDetFullScanLRTSequence(ConfigFlags):
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    std_cfg = getInDetTrigConfig("jet" )
    lrt_cfg = getInDetTrigConfig("fullScanLRT")

    from TriggerMenuMT.HLT.Jet.JetMenuSequences import getTrackingInputMaker
    InputMakerAlg=getTrackingInputMaker("ftf")

    from ..CommonSequences.FullScanDefs import trkFSRoI
    from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTrackingNoView

    ftf_algs = makeInDetTrigFastTrackingNoView(config=std_cfg, secondStageConfig = lrt_cfg, rois=trkFSRoI)

    #create two sequencers first contains everything apart from the final two algs
    std_seq = seqAND("UncTrkrecoSeq", [InputMakerAlg, ftf_algs[0:-2]])
    #sequence for the lrt objects final two algs
    lrt_seq = seqAND("UncTrklrtstagerecoSeq", ftf_algs[-2:len(ftf_algs)])

    combined_seq = parOR("UncTrklrtrecoSeq", [std_seq, lrt_seq])

    return (combined_seq, InputMakerAlg, lrt_cfg.tracks_FTF())