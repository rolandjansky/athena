from AthenaCommon.CFElements import parOR, seqAND
from GaudiKernel.Constants import WARNING
#from TrigHypothesis.TrigEgammaHypo import TrigEgammaTLAPhotonFexMT

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool



def TLAPhotonSequence(flags, photonsIn):
    ''''Create TLA Photon Sequence'''


    # empty reco sequence sequence acting on fastphotons?
    recoSeq = seqAND("PhotonTLASeq_"+photonsIn,  [])
    #this is the name of the output photons
    sequenceOut = photonsIn+"_TLA"
    # initializes and configure the TLA Selector Algorithm

    from TrigEgammaHypo import TrigEgammaTLAPhotonFexMTConfig
    # this has yet to be written, should be similar to the jet tla
    TLAPhotonAlg = TrigEgammaTLAPhotonFexMTConfig.getConfiguredTLAPhotonSelector(inputPhotonsKey=photonsIn, TLAPhotonsKey=sequenceOut, outputLevel=WARNING)


    # adds the selector to the newborn sequence)
    recoSeq += TLAPhotonAlg

    return ( recoSeq, sequenceOut )

def TLAPhotonAthSequence(flags, photonsIn):
    from AthenaConfiguration.ComponentFactory import CompFactory

    #creates the alg that creates the input for the TLA Algorithm. Is this necessary?
    #
    InputMakerAlg = CompFactory.InputMakerForRoI( "IM_HLT_RoI_FastPhotons" ) # what goes here?

    InputMakerAlg.RoITool = CompFactory.ViewCreatorInitialROITool() # what's the difference with ViewCreatorPreviousROITool?
    InputMakerAlg.mergeUsingFeature = True

    (tlaPhotonSequence, sequenceOut) = RecoFragmentsPool.retrieve( TLAPhotonSequence, flags, photonsIn=photonsIn )
    # the TLAPhoton sequence is now InputMakerAlg --> TrigEgammaTLAPhotonFexMT
    tlaPhotonAthSequence = seqAND( "TLAPhotonAthSequence_"+photonsIn, [InputMakerAlg, tlaPhotonSequence] )

    return (tlaPhotonAthSequence, InputMakerAlg, sequenceOut)

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
