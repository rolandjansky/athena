# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import seqAND
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.Logging import logging

logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

# ---------------------

def getCommonInDetFullScanSequence(ConfigFlag):

    from TriggerMenuMT.HLT.Jet.JetMenuSequences import getTrackingInputMaker
    InputMakerAlg=getTrackingInputMaker("ftf")

    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( 'jet' )

    from ..CommonSequences.FullScanDefs import trkFSRoI
    from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTrackingNoView
    TrkInputNoViewAlg = makeInDetTrigFastTrackingNoView( config=IDTrigConfig, rois=trkFSRoI )

    from TrigInDetConfig.InDetTrigVertices import makeInDetTrigVertices
    
    vtxAlgs = makeInDetTrigVertices( "jet", IDTrigConfig.tracks_FTF(), IDTrigConfig.vertex_jet, IDTrigConfig, adaptiveVertex=IDTrigConfig.adaptiveVertex_jet)
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
