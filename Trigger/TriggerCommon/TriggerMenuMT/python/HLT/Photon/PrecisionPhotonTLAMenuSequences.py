#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from AthenaCommon.CFElements import seqAND
from AthenaConfiguration.ComponentFactory import CompFactory
#from TrigHypothesis.TrigEgammaHypo import TrigEgammaTLAPhotonFex

from TriggerMenuMT.HLT.Menu.MenuComponents import MenuSequence, RecoFragmentsPool


def TLAPhotonSequence (flags, photonsIn):
    
    ## add the InputMaker (event context)    
    tlaPhotonInputMakerAlg = CompFactory.InputMakerForRoI("IMTLAPhotons", RoIsLink="initialRoI")
    tlaPhotonInputMakerAlg.RoITool =  CompFactory.ViewCreatorPreviousROITool()
    tlaPhotonInputMakerAlg.mergeUsingFeature = True
    
    tlaPhotonAthSequence = seqAND( "TLAPhotonAthSequence", [tlaPhotonInputMakerAlg] )
    sequenceOut = photonsIn+"_TLA"
    return (tlaPhotonAthSequence, tlaPhotonInputMakerAlg, sequenceOut)

def TLAPhotonMenuSequence( flags, photonsIn ):
    
    # retrieves the sequence, uses RecoFragmentsPool to prevent a duplicate instantiation of the sequence (and the IM algorithm)    
    (tlaPhotonAthSequence, tlaPhotonInputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(TLAPhotonSequence, flags, photonsIn=photonsIn)  
     #  add the hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaTLAPhotonHypoAlg
    from TrigEgammaHypo.TrigEgammaTLAPhotonHypoTool import TrigEgammaTLAPhotonHypoToolFromDict 

    hypo = TrigEgammaTLAPhotonHypoAlg("TrigPhotonTLAHypoAlg") 


    hypo.TLAOutputName = sequenceOut

    return MenuSequence( Sequence    = tlaPhotonAthSequence,
                         Maker       = tlaPhotonInputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigEgammaTLAPhotonHypoToolFromDict
                         )

