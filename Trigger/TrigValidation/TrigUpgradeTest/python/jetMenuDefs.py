#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO


def jetSequence():
    # menu components   
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
    from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq

    #input maker
    from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
    InputMakerAlg = InputMakerForRoI("JetInputMaker", OutputLevel = DEBUG, RoIsLink="initialRoI")
    InputMakerAlg.RoIs='FSJETRoI'
    
    #reco sequence
    from TrigUpgradeTest.jetDefs import jetRecoSequence
    (recoSequence, sequenceOut) = jetRecoSequence(InputMakerAlg.RoIs)

    #hypo
    from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetHypoAlgMT
    from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetHypoToolFromName
    hypo = TrigJetHypoAlgMT("TrigJetHypoAlgMT")
    hypo.OutputLevel = DEBUG
    hypo.Jets = sequenceOut

    # sequence
    JetAthSequence =  seqAND("jetAthSequence",[InputMakerAlg, recoSequence ])

    return  MenuSequence( Sequence    = JetAthSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = hypo,
                          HypoToolGen = trigJetHypoToolFromName )
