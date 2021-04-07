from AthenaCommon.CFElements import parOR, seqAND
from TrigHypothesis.TrigEgammaHypo import TrigEgammaTLAPhotonFexMT
from AthenaConfiguration.ComponentFactory import CompFactory


def TLAPhotonSequence(flags, photonsIn):
    ''''Create TLA Photon Sequence'''


    # empty reco sequence sequence acting on fastphotons?
    recoSeq = seqAND("PhotonTLASeq_"+photonsIn,  [])
    #this is the name of the output photons
    sequenceOut = photonsIn+"_TLA"
    # initializes and configure the TLA Selector Algorithm

    # this has yet to be written, should be similar to the jet tla
    TLAPhotonAlg = getConfiguredTLAPhotonSelector(inputPhotonsKey=photonsIn, TLAPhotonsKey=sequenceOut, outputLevel=WARNING)


    # adds the selector to the newborn sequence)
    recoSeq += TLAPhotonAlg

    return ( recoSeq, sequenceOut )

def TLAPhotonAthSequence(flags, photonsIn):

    #creates the alg that creates the input for the TLA Algorithm. Is this necessary?
    #
    InputMakerAlg = CompFactory.InputMakerForRoI( "XXXX" ) # what goes here?

    InputMakerAlg.RoITool = CompFactory.ViewCreatorInitialRoITool() # what's the difference with ViewCreatorPreviousROITool?
    InputMakerAlg.mergeUsingFeature = True

    (TLAPhotonSequence, sequenceOut) = RecoFragmentsPool.retireve( TLAPhotonSequence, flags, photonsIn=photonsIn )
    # the TLAPhoton sequence is now InputMakerAlg --> TrigEgammaTLAPhotonFexMT
    TLAPhotonAthSequence = seqAND( "TLAPhotonAthSequence_"+photonsIn, [InputMakerAlg, TLAPhotonSequence] )

    return

def TLAPhotonMenuSequence(flags, photonsIn):

    from XXXX import TrigEgammaTLAPhotonHypoAlgMT
    from XXXX import TrigEgammaTLAPhotonHypoToolFromDict # JetTLA calls a function "FromDict" from a python, investigate later

    (TLAPhotonAthSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(TLAPhotonAthSequence, flags, photonsIn=photonsIn)
    hypo = TrigEgammaTLAPhotonHypoAlgMT("XXXXX_"+photonsIn)
    hypo.Photons = sequenceOut


    return MenuSequence( Sequence    = TLAPhotonAthSequence
                         Maker       = InputMakerAlg
                         Hypo        = hypo
                         HypoToolGen = TrigEgammaTLAPhotonHypoToolFromDict
                         )
