#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from AthenaCommon.CFElements import parOR, seqAND
import AthenaCommon.CfgMgr as CfgMgr
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import  ViewCreatorPreviousROITool
#from TrigHypothesis.TrigEgammaHypo import TrigEgammaTLAPhotonFexMT

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool



def TLAPhotonSequence(flags, photonsIn):
    ''''Create TLA Photon Sequence'''


    # empty reco sequence sequence
    recoSeq = seqAND("PhotonTLASeq_"+photonsIn,  [])
    #this is the name of the output photons
    sequenceOut = photonsIn+"_TLA"
    # initializes and configure the TLA Selector Algorithm

    # this ensures data access to the HLT_egamma_Photons collection previously built in the precisionPhoton View
    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("TLAPhotonViewDataVerifier")
    ViewVerify.DataObjects = [( 'xAOD::PhotonContainer' , 'StoreGateSvc+HLT_egamma_Photons')]
    

    from TrigEgammaHypo import TrigEgammaTLAPhotonFexMTConfig
    # this has yet to be written, should be similar to the jet tla
    TLAPhotonAlg = TrigEgammaTLAPhotonFexMTConfig.getConfiguredTLAPhotonSelector(inputPhotonsKey=photonsIn, TLAPhotonsKey=sequenceOut)

    # The OR makes sure that TLAPhotonAlg can access the data dependencies specified by ViewVerify
    photonInViewAlgs = parOR("tlaPhotonInViewAlgs", [ViewVerify, TLAPhotonAlg])

    # adds the selector to the newborn sequence)
    recoSeq +=  photonInViewAlgs

    return ( recoSeq, sequenceOut )

def TLAPhotonAthSequence(flags, photonsIn):
    
    
    tlaPhotonViewsMakerAlg = EventViewCreatorAlgorithm("IM_TLAPhotons")
    tlaPhotonViewsMakerAlg.RoIsLink = "initialRoI"
    tlaPhotonViewsMakerAlg.RoITool = ViewCreatorPreviousROITool() 
    # ensure that the sequence runs within a view spawned from the precisionPhoton ROI
    tlaPhotonViewsMakerAlg.InViewRoIs = "precisionPhotonRoIs"
    tlaPhotonViewsMakerAlg.ViewFallThrough = True
    tlaPhotonViewsMakerAlg.RequireParentView = True
    tlaPhotonViewsMakerAlg.Views = "TLAPhotonsViews"
    tlaPhotonViewsMakerAlg.mergeUsingFeature = True

    tlaPhotonViewsMakerAlg.ViewNodeName = "tlaPhotonInViewAlgs"

    

    
    

    (tlaPhotonSequence, sequenceOut) = RecoFragmentsPool.retrieve( TLAPhotonSequence, flags, photonsIn=photonsIn )
    # the TLAPhoton sequence is now tlaPhotonViewsMakerAlg --> TrigEgammaTLAPhotonFexMT
    tlaPhotonAthSequence = seqAND( "TLAPhotonAthSequence_"+photonsIn, [tlaPhotonViewsMakerAlg, tlaPhotonSequence] )
    
    return (tlaPhotonAthSequence, tlaPhotonViewsMakerAlg, sequenceOut)

def TLAPhotonMenuSequence(flags, photonsIn):

    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaTLAPhotonHypoAlgMT
    from TrigEgammaHypo.TrigEgammaTLAPhotonHypoTool import TrigEgammaTLAPhotonHypoToolFromDict # JetTLA calls a function "FromDict" from a python, investigate later

    (tlaPhotonAthSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(TLAPhotonAthSequence, flags, photonsIn=photonsIn)
    hypo = TrigEgammaTLAPhotonHypoAlgMT("TrigEgammaTLAPhotonHypoAlgMT_"+photonsIn)
    hypo.Photons = sequenceOut


    return MenuSequence( Sequence    = tlaPhotonAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigEgammaTLAPhotonHypoToolFromDict
                         )
