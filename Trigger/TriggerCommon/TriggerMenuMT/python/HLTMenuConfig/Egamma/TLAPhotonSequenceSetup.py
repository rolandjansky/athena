from AthenaCommon.CFElements import parOR, seqAND
from GaudiKernel.Constants import WARNING
import AthenaCommon.CfgMgr as CfgMgr
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import  ViewCreatorPreviousROITool
#from TrigHypothesis.TrigEgammaHypo import TrigEgammaTLAPhotonFexMT

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool



def TLAPhotonSequence(flags, photonsIn):
    ''''Create TLA Photon Sequence'''


    # empty reco sequence sequence acting on fastphotons?
    recoSeq = seqAND("PhotonTLASeq_"+photonsIn,  [])
    #this is the name of the output photons
    sequenceOut = photonsIn+"_TLA"
    # initializes and configure the TLA Selector Algorithm

    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("TLAPhotonViewDataVerifier")
    ViewVerify.DataObjects = [( 'xAOD::PhotonContainer' , 'StoreGateSvc+HLT_egamma_Photons')]
    

    from TrigEgammaHypo import TrigEgammaTLAPhotonFexMTConfig
    # this has yet to be written, should be similar to the jet tla
    TLAPhotonAlg = TrigEgammaTLAPhotonFexMTConfig.getConfiguredTLAPhotonSelector(inputPhotonsKey=photonsIn, TLAPhotonsKey=sequenceOut, outputLevel=WARNING)

    photonInViewAlgs = parOR("tlaPhotonInViewAlgs", [ViewVerify, TLAPhotonAlg])

    # adds the selector to the newborn sequence)
    recoSeq +=  photonInViewAlgs

    return ( recoSeq, sequenceOut )

def TLAPhotonAthSequence(flags, photonsIn):
    from AthenaConfiguration.ComponentFactory import CompFactory

    #creates the alg that creates the input for the TLA Algorithm. Is this necessary?
    #
    tlaPhotonViewsMakerAlg = EventViewCreatorAlgorithm("IM_TLAPhotons")
    tlaPhotonViewsMakerAlg.RoIsLink = "initialRoI"
    tlaPhotonViewsMakerAlg.RoITool = ViewCreatorPreviousROITool() # what's the difference with ViewCreatorPreviousROITool?
    tlaPhotonViewsMakerAlg.InViewRoIs = "precisionPhotonRoIs"
    tlaPhotonViewsMakerAlg.ViewFallThrough = True
    tlaPhotonViewsMakerAlg.RequireParentView = True
    tlaPhotonViewsMakerAlg.Views = "TLAPhotonsViews"
    tlaPhotonViewsMakerAlg.mergeUsingFeature = True

    tlaPhotonViewsMakerAlg.ViewNodeName = "tlaPhotonInViewAlgs"

    #InputMakerAlg = CompFactory.InputMakerForRoI( "IM_HLT_RoI_FastPhotons" ) # what goes here?

    
    

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
