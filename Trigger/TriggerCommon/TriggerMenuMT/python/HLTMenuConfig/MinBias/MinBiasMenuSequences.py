#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaConfiguration.AllConfigFlags import ConfigFlags

from TrigMissingETHypo.TrigMissingETHypoConfigMT import TrigMinBiasHypoToolFromDict
from TrigMissingETHypo.TrigMissingETHypoConf import MbCountHypoAlgMT


def minbiasSpacePointMenuSequence():
    # menu components
    # retrieve the reco seuqnece
    from TriggerMenuMT.HLTMenuConfig.MinBias.MinBiasSPRecoSequences import minbiasSpacePointAthSequence
    ( minbiasSpacePointSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(minbiasSpacePointAthSequence,ConfigFlags)

    #hypo
    mbHypoAlg = MbCountHypoAlgMT("MinBiasHypoAlg_sp")
    mbHypoAlg.MinBiasContainerKey=sequenceOut

    return  MenuSequence( Sequence    = minbiasSpacePointSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = mbHypoAlg,
                          HypoToolGen = TrigMinBiasHypoToolFromDict )


def minbiasTrackMenuSequence():
    # menu components
    # retrieve the reco seuqnece
    from TriggerMenuMT.HLTMenuConfig.MinBias.MinBiasTrkRecoSequences import minbiasTrackAthSequence
    (minbiasTrackSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(minbiasTrackAthSequence,ConfigFlags)

    #hypo
    mbHypoAlg = MbCountHypoAlgMT("MinBiasHypoAlg_trk")
    mbHypoAlg.MinBiasContainerKey=sequenceOut

    return  MenuSequence( Sequence    = minbiasTrackSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = mbHypoAlg,
                          HypoToolGen = TrigMinBiasHypoToolFromDict )
