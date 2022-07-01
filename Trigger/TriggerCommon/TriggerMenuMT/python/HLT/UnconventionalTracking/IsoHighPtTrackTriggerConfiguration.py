# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import seqAND
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.Logging import logging

logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

def IsoHPtTrackTriggerHypoSequence():
        from TrigLongLivedParticlesHypo.TrigIsoHPtTrackTriggerHypoTool import TrigIsoHPtTrackTriggerHypoToolFromDict
        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import (TrigIsoHPtTrackTriggerHypoAlg)

        # Get sequence name
        from TriggerMenuMT.HLT.UnconventionalTracking.CommonConfiguration import getCommonInDetFullScanSequence
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        ( TrkSeq, InputMakerAlg, sequenceOut ) = RecoFragmentsPool.retrieve(getCommonInDetFullScanSequence,ConfigFlags)

        # Setup the hypothesis algorithm
        theIsoHPtTrackTriggerHypo = TrigIsoHPtTrackTriggerHypoAlg("L2IsoHPtTrack")
        theIsoHPtTrackTriggerHypo.trackKey = sequenceOut

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

