#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
    
def jetMenuSequence(dummyFlags,**recoDict):
    """ Function to create the jet Menu Sequence"""
    
    ## RoIs = 'FSJETRoI'
    #reco sequence
    from TriggerMenuMT.HLTMenuConfig.Jet.JetRecoSequences import jetAthSequence, jetRecoDictToString
    (JetAthSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(jetAthSequence,None,**recoDict)
    
    #hypo
    from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetHypoAlgMT
    from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetHypoToolFromDict
    hypo = TrigJetHypoAlgMT("TrigJetHypoAlgMT_"+jetRecoDictToString(recoDict))
    hypo.Jets = sequenceOut

    return  MenuSequence( Sequence    = JetAthSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = hypo,
                          HypoToolGen = trigJetHypoToolFromDict )
