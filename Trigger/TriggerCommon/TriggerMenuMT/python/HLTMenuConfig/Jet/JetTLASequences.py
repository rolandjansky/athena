# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaCommon.CFElements import seqAND
from AthenaConfiguration.ComponentFactory import CompFactory

#from GaudiKernel.Constants import (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)



def TLAJetSequence (flags, jetsIn):
    
    ## add the InputMaker (event context)    
    tlaJetInputMakerAlg = CompFactory.InputMakerForRoI("IMTLAJets")#,RoIsLink="initialRoI")
    tlaJetInputMakerAlg.RoITool = CompFactory.ViewCreatorInitialROITool()
    tlaJetInputMakerAlg.mergeUsingFeature = True
    
    print("MARCO: inside TLAJetSequence")
    tlaJetAthSequence = seqAND( "TLAJetAthSequence", [tlaJetInputMakerAlg] )
    sequenceOut = jetsIn+"_TLA"
    return (tlaJetAthSequence, tlaJetInputMakerAlg, sequenceOut)


def TLAJetMenuSequence( flags, jetsIn ):
    
    # retrieves the sequence
    (tlaJetAthSequence, tlaJetInputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(TLAJetSequence, flags, jetsIn=jetsIn)  
    #  add the hypo
    from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetTLAHypoAlg
    from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetTLAHypoToolFromDict

    hypo = TrigJetTLAHypoAlg("TrigJetTLAHypoAlg") 

    
    hypo.TLAOutputName = sequenceOut

    return MenuSequence( Sequence    = tlaJetAthSequence,
                         Maker       = tlaJetInputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = trigJetTLAHypoToolFromDict
                         )

