#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from AthenaCommon.CFElements import seqAND
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaConfiguration.ComponentFactory import CompFactory


def TLAMuonSequence (flags, muons):
    
    ## add the InputMaker (event context)    
    tlaMuonInputMakerAlg = CompFactory.InputMakerForRoI("IMTLAMuons", RoIsLink="initialRoI")
    tlaMuonInputMakerAlg.mergeUsingFeature = True
    
    tlaMuonAthSequence = seqAND( "TLAMuonAthSequence", [tlaMuonInputMakerAlg] )
    sequenceOut = muons+"_TLA"
    return (tlaMuonAthSequence, tlaMuonInputMakerAlg, sequenceOut)


def TLAMuonMenuSequence( flags, muonsIn):
    # retrieve the sequence via RecoFragmentsPool
    (tlaMuonAthSequence, tlaMuonInputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(TLAMuonSequence, flags, muons=muonsIn)    
     #  add the hypo
    from TrigMuonHypo.TrigMuonHypoConf import TrigMuonTLAHypoAlg
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFMSonlyHypoToolFromDict
    hypo = TrigMuonTLAHypoAlg("TrigMuonTLAHypoAlg")  #+muonsIn)    

    return MenuSequence( Sequence  = tlaMuonAthSequence,
                         Maker       = tlaMuonInputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigMuonEFMSonlyHypoToolFromDict
                         )

