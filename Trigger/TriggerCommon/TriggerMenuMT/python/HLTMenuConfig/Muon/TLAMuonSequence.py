#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from AthenaCommon.CFElements import seqAND
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from AthenaConfiguration.ComponentFactory import CompFactory


def TLAMuonMenuSequence( flags, muonsIn):
    
    ## add the InputMaker (event context)    
    tlaMuonInputMakerAlg = CompFactory.InputMakerForRoI("IMTLAMuons", RoIsLink="initialRoI")
    tlaMuonInputMakerAlg.mergeUsingFeature = True
    
    tlaMuonAthSequence = seqAND( "TLAMuonAthSequence", [tlaMuonInputMakerAlg] )
     #  add the hypo
    from TrigMuonHypo.TrigMuonHypoConf import TrigMuonTLAHypoAlg
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFMSonlyHypoToolFromDict
    hypo = TrigMuonTLAHypoAlg("TrigMuonTLAHypoAlg")  #+muonsIn)

    sequenceOut = muonsIn+"_TLA"
    hypo.TLAMuons = sequenceOut

    return MenuSequence( Sequence    = tlaMuonAthSequence,
                         Maker       = tlaMuonInputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigMuonEFMSonlyHypoToolFromDict
                         )
