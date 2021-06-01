# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.CFElements import seqAND
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from AthenaCommon.Logging import logging

logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)


def dEdxTriggerHypoSequence():
        from TrigLongLivedParticlesHypo.TrigdEdxTrackTriggerHypoTool   import TrigdEdxTrackTriggerHypoToolFromDict
        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import (TrigdEdxTrackTriggerHypoAlg)

        # Setup the hypothesis algorithm
        thedEdxTrackTriggerHypo = TrigdEdxTrackTriggerHypoAlg("dEdxTrack")

        from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
        from AthenaConfiguration.ComponentFactory import CompFactory
        DummyInputMakerAlg = conf2toConfigurable(CompFactory.InputMakerForRoI( "IM_dEdxTrack_HypoOnlyStep" ))
        DummyInputMakerAlg.RoITool = conf2toConfigurable(CompFactory.ViewCreatorInitialROITool())

        log.debug("Building the Step dictinary for dEdx")
        return MenuSequence( Sequence    = seqAND("dEdxEmptyStep",[DummyInputMakerAlg]),
                             Maker       = DummyInputMakerAlg,
                             Hypo        = thedEdxTrackTriggerHypo,
                             HypoToolGen = TrigdEdxTrackTriggerHypoToolFromDict,
                         )
