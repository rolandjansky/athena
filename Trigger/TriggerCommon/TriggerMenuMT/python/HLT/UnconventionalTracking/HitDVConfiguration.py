# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import seqAND
from TriggerMenuMT.HLT.Menu.MenuComponents import MenuSequence
from AthenaCommon.Logging import logging

from ..Menu.MenuComponents import RecoFragmentsPool
from AthenaConfiguration.AllConfigFlags import ConfigFlags

logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)


def UTTJetRecoSequence():
        from TrigStreamerHypo.TrigStreamerHypoConf   import TrigStreamerHypoAlg
        from TrigStreamerHypo.TrigStreamerHypoConfig import StreamerHypoToolGenerator

        from ..Jet.JetRecoSequences     import jetRecoSequence
        from ..Jet.JetRecoCommon import extractRecoDict
        from ..Menu.SignatureDicts      import JetChainParts_Default
        
        jetRecoDict = extractRecoDict([JetChainParts_Default])
        jetRecoDict.update( 
                {'recoAlg': 'a4', 'constitType': 'tc', 'clusterCalib': 'em', 'constitMod': '', 'trkopt': 'notrk'}
        )

        trkcolls = {}

        JetSeq, jetName, jetDef = RecoFragmentsPool.retrieve(
            jetRecoSequence,
            ConfigFlags,
            clustersKey='HLT_TopoCaloClustersFS',
            **trkcolls,
            **jetRecoDict,
        )

        HypoAlg = TrigStreamerHypoAlg("UTTJetRecDummyStream")

        from TrigT2CaloCommon.CaloDef import clusterFSInputMaker

        IMalg = clusterFSInputMaker()
        return MenuSequence( Sequence    = seqAND("UTTJetRecoSeq", [IMalg,JetSeq]),
                             Maker       = IMalg,
                             Hypo        = HypoAlg,
                             HypoToolGen = StreamerHypoToolGenerator
                     )


def HitDVHypoSequence():
        from TrigLongLivedParticlesHypo.TrigHitDVHypoConfig import TrigHitDVHypoToolFromDict
        from TrigLongLivedParticlesHypo.TrigHitDVHypoConfig import createTrigHitDVHypoAlg

        theHitDVHypo = createTrigHitDVHypoAlg("HitDV")

        from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
        from AthenaConfiguration.ComponentFactory import CompFactory
        DummyInputMakerAlg = conf2toConfigurable(CompFactory.InputMakerForRoI( "IM_HitDV_HypoOnlyStep" ))
        DummyInputMakerAlg.RoITool = conf2toConfigurable(CompFactory.ViewCreatorInitialROITool())

        return MenuSequence( Sequence    = seqAND("HitDVEmptyStep",[DummyInputMakerAlg]),
                             Maker       = DummyInputMakerAlg,
                             Hypo        = theHitDVHypo,
                             HypoToolGen = TrigHitDVHypoToolFromDict,
                         )
