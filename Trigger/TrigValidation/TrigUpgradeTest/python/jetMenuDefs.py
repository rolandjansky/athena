#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
from AthenaConfiguration.AllConfigFlags import ConfigFlags
    
def jetMenuSequence():
    """ Function to create the jet Menu Sequence"""
    
    ## RoIs = 'FSJETRoI'
    
    ## #input maker
    ## from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
    ## InputMakerAlg = InputMakerForRoI("JetInputMaker", OutputLevel = DEBUG, RoIsLink="initialRoI")
    ## InputMakerAlg.RoIs=RoIs
    
    #reco sequence
    from TrigUpgradeTest.jetDefs import jetAthSequence

    #(recoSequence, sequenceOut) = jetRecoSequence(InputMakerAlg.RoIs)
#    (recoSequence, sequenceOut, InputMakerAlg) = RecoFragmentsPool.retrieve(jetRecoSequence, ConfigFlags, RoIs=jetIM.RoIs)

    (JetAthSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(jetAthSequence,ConfigFlags)
     
    #hypo
    from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetHypoAlgMT
    from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetHypoToolFromName
    hypo = TrigJetHypoAlgMT("TrigJetHypoAlgMT")
    hypo.OutputLevel = DEBUG
    hypo.Jets = sequenceOut

    # sequence
    #JetAthSequence =  seqAND("jetAthSequence",[InputMakerAlg, recoSequence ])

    return  MenuSequence( Sequence    = JetAthSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = hypo,
                          HypoToolGen = trigJetHypoToolFromName )
