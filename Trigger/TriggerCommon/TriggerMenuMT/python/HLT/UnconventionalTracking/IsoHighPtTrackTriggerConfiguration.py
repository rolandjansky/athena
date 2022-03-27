# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.CFElements import seqAND
from ..CommonSequences.FullScanDefs import caloFSRoI
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.Logging import logging

logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)


def FTFTrackSequence(ConfigFlags):

    from TriggerMenuMT.HLT.Jet.JetMenuSequences import getTrackingInputMaker
    InputMakerAlg=getTrackingInputMaker("ftf")

    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( 'jet' )

    from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTrackingNoView
    TrkInputNoViewAlg = makeInDetTrigFastTrackingNoView( config=IDTrigConfig, rois=caloFSRoI )

    from TrigInDetConfig.InDetTrigVertices import makeInDetTrigVertices
    
    vtxAlgs = makeInDetTrigVertices( "jet", IDTrigConfig.tracks_FTF(), IDTrigConfig.vertex_jet, IDTrigConfig, adaptiveVertex=IDTrigConfig.adaptiveVertex_jet)
    prmVtx = vtxAlgs[-1]

    TrkSeq =  [InputMakerAlg,TrkInputNoViewAlg, prmVtx]
    sequenceOut = IDTrigConfig.tracks_FTF()

    return (TrkSeq, InputMakerAlg, sequenceOut)





def IsoHPtTrackTriggerHypoSequence():
        from TrigLongLivedParticlesHypo.TrigIsoHPtTrackTriggerHypoTool import TrigIsoHPtTrackTriggerHypoToolFromDict
        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import (TrigIsoHPtTrackTriggerHypoAlg)

        # Get sequence name
        from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
        IDTrigConfig = getInDetTrigConfig( 'jet' )
        sequenceOut = IDTrigConfig.tracks_FTF()

        #Setup the hypothesis algorithm
        theIsoHPtTrackTriggerHypo = TrigIsoHPtTrackTriggerHypoAlg("L2IsoHPtTrack")
        theIsoHPtTrackTriggerHypo.trackKey =  sequenceOut

        from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
        from AthenaConfiguration.ComponentFactory import CompFactory
        DummyInputMakerAlg = conf2toConfigurable(CompFactory.InputMakerForRoI( "IM_IsoHPtTrack_HypoOnlyStep" ))
        DummyInputMakerAlg.RoITool = conf2toConfigurable(CompFactory.ViewCreatorInitialROITool())

        log.debug("Building the Step dictinary for IsoHPt!")
        return MenuSequence( Sequence    = seqAND("UncTrkEmptySeq",[DummyInputMakerAlg]),
                            Maker       = DummyInputMakerAlg,
                            Hypo        = theIsoHPtTrackTriggerHypo,
                            HypoToolGen = TrigIsoHPtTrackTriggerHypoToolFromDict,
                            )

def FTFRecoOnlySequence():
        from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlg
        from TrigStreamerHypo.TrigStreamerHypoConfig import StreamerHypoToolGenerator

        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        ( TrkSeq, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(FTFTrackSequence,ConfigFlags)

        HypoAlg = TrigStreamerHypoAlg("UncTrkDummyStream")


        log.debug("Building the Step dictinary for IsoHPt!")
        return MenuSequence( Sequence    = seqAND("UncTrkrecoSeq", TrkSeq),
                            Maker       = InputMakerAlg,
                            Hypo        = HypoAlg,
                            HypoToolGen = StreamerHypoToolGenerator
                            )
