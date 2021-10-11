# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaCommon.CFElements import parOR, seqAND

#from GaudiKernel.Constants import (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
from GaudiKernel.Constants import WARNING

#this can eventually become a TLA/PEB sequence, but let's start with TLA only
def jetTLASequence(flags, jetsin):
        
    #make a new reco sequence, empty at this point now
    recoSeq = parOR("JetTLASeq_"+jetsin, [])

    #import the TLA algorithm that selects jets and makes a new collection to pass to [what used to be the ScoutingStreamWriter]
    from TrigHLTJetRec import TrigHLTJetTLASelector
    
    sequenceOut=jetsin+"_TLA"
    TLAAlg=TrigHLTJetTLASelector.getConfiguredTLASelector(inputJetsKey=jetsin, outputJetsKey=sequenceOut, outputLevel=WARNING)
    recoSeq+=TLAAlg

    return (recoSeq, sequenceOut)

#Configure an AthSequence for jet TLA - will eventually also include PEB
def jetTLAAthSequence(flags, jetsin):
    from AthenaConfiguration.ComponentFactory import CompFactory
    
    InputMakerAlg = CompFactory.InputMakerForRoI( "IM_Jet_TLAStep" )
    InputMakerAlg.RoITool = CompFactory.ViewCreatorInitialROITool()
    InputMakerAlg.mergeUsingFeature = True
    
    (JetTLASequence, sequenceOut) = RecoFragmentsPool.retrieve( jetTLASequence, flags, jetsin=jetsin )
    JetTLAAthSequence =  seqAND("jetTLAAthSequence_"+jetsin,[InputMakerAlg, JetTLASequence ])

    return (JetTLAAthSequence, InputMakerAlg, sequenceOut)

def jetTLAMenuSequence(flags, jetsin):

    from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetTLAHypoAlg
    from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetTLAHypoToolFromDict
    
    (JetTLAAthSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(jetTLAAthSequence,flags,jetsin=jetsin)
    
    hypo = TrigJetTLAHypoAlg("TrigJetTLAHypoAlg_"+jetsin)
    hypo.Jets = sequenceOut 

    return  MenuSequence( Sequence    = JetTLAAthSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = hypo,
                          HypoToolGen = trigJetTLAHypoToolFromDict )
    
