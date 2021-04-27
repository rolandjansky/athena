# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.CFElements import seqAND
from ..CommonSequences.FullScanDefs import caloFSRoI
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.Logging import logging

logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.IsoHighPtTrackTriggerConfiguration")


def FTFTrackSequence(ConfigFlags):

    from TriggerMenuMT.HLTMenuConfig.Jet.JetMenuSequences import getTrackingInputMaker
    InputMakerAlg=getTrackingInputMaker()

    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( 'jet' )

    from TrigInDetConfig.InDetSetup import makeInDetAlgsNoView
    TrkInputNoViewAlg = makeInDetAlgsNoView( config=IDTrigConfig, rois=caloFSRoI )

    from TrigInDetConfig.TrigInDetPriVtxConfig import makeVertices

    vtxAlgs = makeVertices( "jet", IDTrigConfig.tracks_FTF(), IDTrigConfig.vertex_jet, IDTrigConfig, adaptiveVertex=IDTrigConfig.adaptiveVertex_jet )
    prmVtx = vtxAlgs[-1]

    TrkSeq =  [InputMakerAlg,TrkInputNoViewAlg, prmVtx]
    sequenceOut = IDTrigConfig.tracks_FTF()

    return (TrkSeq, InputMakerAlg, sequenceOut)





def IsoHPtTrackTriggerHypoSequence():
        from TrigLongLivedParticlesHypo.TrigIsoHPtTrackTriggerHypoTool import TrigIsoHPtTrackTriggerHypoToolFromDict
        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import (TrigIsoHPtTrackTriggerHypoAlgMT)

        # Get sequence name
        from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
        IDTrigConfig = getInDetTrigConfig( 'jet' )
        sequenceOut = IDTrigConfig.tracks_FTF()

        #Setup the hypothesis algorithm
        theIsoHPtTrackTriggerHypo = TrigIsoHPtTrackTriggerHypoAlgMT("L2IsoHPtTrack")
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
        from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlgMT
        from TrigStreamerHypo.TrigStreamerHypoConfigMT import StreamerHypoToolMTgenerator

        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        ( TrkSeq, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(FTFTrackSequence,ConfigFlags)

        HypoAlg = TrigStreamerHypoAlgMT("UncTrkDummyStream")


        log.debug("Building the Step dictinary for IsoHPt!")
        return MenuSequence( Sequence    = seqAND("UncTrkrecoSeq", TrkSeq),
                            Maker       = InputMakerAlg,
                            Hypo        = HypoAlg,
                            HypoToolGen = StreamerHypoToolMTgenerator
                            )
