# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.CFElements import seqAND
from TriggerMenuMT.HLT.Menu.MenuComponents import MenuSequence
from AthenaCommon.Logging import logging

logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)


def dEdxTriggerHypoSequence():
        from TrigLongLivedParticlesHypo.TrigdEdxTrackHypoConfig import TrigdEdxTrackHypoToolFromDict
        from TrigLongLivedParticlesHypo.TrigdEdxTrackHypoConfig import createTrigdEdxTrackHypoAlg
        
        thedEdxTrackHypo = createTrigdEdxTrackHypoAlg("dEdxTrack")

        from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
        from AthenaConfiguration.ComponentFactory import CompFactory
        DummyInputMakerAlg = conf2toConfigurable(CompFactory.InputMakerForRoI( "IM_dEdxTrack_HypoOnlyStep" ))
        DummyInputMakerAlg.RoITool = conf2toConfigurable(CompFactory.ViewCreatorInitialROITool())

        return MenuSequence( Sequence    = seqAND("dEdxEmptyStep",[DummyInputMakerAlg]),
                             Maker       = DummyInputMakerAlg,
                             Hypo        = thedEdxTrackHypo,
                             HypoToolGen = TrigdEdxTrackHypoToolFromDict,
                     )
