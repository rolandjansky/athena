#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaConfiguration.AllConfigFlags import ConfigFlags
    
def jetMenuSequence(jetdef):
    """ Function to create the jet Menu Sequence"""
   
    ConfigFlags.jetdef=jetdef
    ## RoIs = 'FSJETRoI'
    #reco sequence
    from TrigUpgradeTest.jetDefs import jetAthSequence
    #(JetAthSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(jetAthSequence,ConfigFlags)
    (JetAthSequence, InputMakerAlg, sequenceOut) = jetAthSequence(ConfigFlags)
     
    #hypo
    from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetHypoAlgMT
    from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetHypoToolFromDict
    hypo = TrigJetHypoAlgMT("TrigJetHypoAlgMT")
    hypo.Jets = sequenceOut


    return  MenuSequence( Sequence    = JetAthSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = hypo,
                          HypoToolGen = trigJetHypoToolFromDict )
