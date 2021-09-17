#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from AthenaCommon.CFElements import parOR, seqAND
import AthenaCommon.CfgMgr as CfgMgr
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import  ViewCreatorPreviousROITool
#from TrigHypothesis.TrigEgammaHypo import TrigEgammaTLAPhotonFex

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool



def TLAPhotonSequence(flags, photonsIn, HLT_threshold):
    ''''Create TLA Photon Sequence'''


    # empty reco sequence sequence
    recoSeq = seqAND("PhotonTLASeq_"+photonsIn,  [])
    #this is the name of the output photons
    sequenceOut = photonsIn+"_TLA"
    # initializes and configure the TLA Selector Algorithm

    # this ensures data access to the HLT_egamma_Photons collection previously built in the precisionPhoton View
    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("TLAPhotonViewDataVerifier")
    ViewVerify.DataObjects = [( 'xAOD::PhotonContainer' , 'StoreGateSvc+HLT_egamma_Photons')]
    

    from TrigEgammaRec.TrigEgammaTLAPhotonConfig import getConfiguredTLAPhotonSelector
   

    # check from chain dictionary the threshold of the photon part
   
    # set the TLA threshold to HLT_threshold - 20 GeV, or 0 GeV if HLT_threshold is < 20 
    # at this point the threshold is in GeV, provide it in MeV to the selector
    TLA_threshold = max(HLT_threshold - 20, 0)*1000 # MeV

    TLAPhotonAlg = getConfiguredTLAPhotonSelector(photonPtThreshold=TLA_threshold, inputPhotonsKey=photonsIn, TLAPhotonsKey=sequenceOut)

    # The OR makes sure that TLAPhotonAlg can access the data dependencies specified by ViewVerify
    photonInViewAlgs = parOR("tlaPhotonInViewAlgs", [ViewVerify, TLAPhotonAlg])

    # adds the selector to the newborn sequence)
    recoSeq +=  photonInViewAlgs

    return ( recoSeq, sequenceOut )

def TLAPhotonAthSequence(flags, photonsIn, HLT_threshold):
    
    tlaPhotonViewsMakerAlg = EventViewCreatorAlgorithm("IM_TLAPhotons")
    tlaPhotonViewsMakerAlg.RoIsLink = "initialRoI"
    tlaPhotonViewsMakerAlg.RoITool = ViewCreatorPreviousROITool() 
    # ensure that the sequence runs within a view spawned from the precisionPhoton ROI
    tlaPhotonViewsMakerAlg.InViewRoIs = "precisionPhotonRoIs"
    tlaPhotonViewsMakerAlg.ViewFallThrough = True
    tlaPhotonViewsMakerAlg.RequireParentView = True
    tlaPhotonViewsMakerAlg.Views = "TLAPhotonsViews"
    tlaPhotonViewsMakerAlg.mergeUsingFeature = False

    tlaPhotonViewsMakerAlg.ViewNodeName = "tlaPhotonInViewAlgs"


    (tlaPhotonSequence, sequenceOut) = RecoFragmentsPool.retrieve( TLAPhotonSequence,  flags, photonsIn=photonsIn,  HLT_threshold=HLT_threshold)
    # the TLAPhoton sequence is now tlaPhotonViewsMakerAlg --> TrigEgammaTLAPhotonFexMT
    tlaPhotonAthSequence = seqAND( "TLAPhotonAthSequence_"+photonsIn, [tlaPhotonViewsMakerAlg, tlaPhotonSequence] )
    
    return (tlaPhotonAthSequence, tlaPhotonViewsMakerAlg, sequenceOut)

def TLAPhotonMenuSequence( flags, photonsIn, HLT_threshold):

    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaTLAPhotonHypoAlg
    from TrigEgammaHypo.TrigEgammaTLAPhotonHypoTool import TrigEgammaTLAPhotonHypoToolFromDict # JetTLA calls a function "FromDict" from a python, investigate later

    (tlaPhotonAthSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(TLAPhotonAthSequence, flags, photonsIn=photonsIn, HLT_threshold=HLT_threshold)
    hypo = TrigEgammaTLAPhotonHypoAlg("TrigEgammaTLAPhotonHypoAlgMT_"+photonsIn)

    hypo.Photons = sequenceOut
    hypo.OriginalPhotons = photonsIn


    return MenuSequence( Sequence    = tlaPhotonAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigEgammaTLAPhotonHypoToolFromDict
                         )
