#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
from TriggerMenuMT.HLTMenuConfig.Jet.JetRecoSequences import jetAthSequence, jetRecoDictToString
    
def jetCFSequence(configFlags,**recoDict):
    """ Function to create the jet Menu Sequence"""
    
    #reco sequence
    (JetAthSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(jetAthSequence,configFlags,**recoDict)
    return (JetAthSequence, InputMakerAlg, sequenceOut) 

def jetMenuSequence(configFlags,**recoDict):
    (JetAthSequence, InputMakerAlg, sequenceOut)= jetCFSequence(configFlags,**recoDict)    
    #hypo
    from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetHypoAlgMT
    from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetHypoToolFromDict
    hypo = TrigJetHypoAlgMT("TrigJetHypoAlgMT_"+jetRecoDictToString(recoDict))
    hypo.Jets = sequenceOut

    return  MenuSequence(   Sequence    = JetAthSequence,
                            Maker       = InputMakerAlg,
                            Hypo        = hypo,
                            HypoToolGen = trigJetHypoToolFromDict )
