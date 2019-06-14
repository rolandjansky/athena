#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

# menu components
from AthenaCommon.CFElements import parOR, seqAND
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs


# ====================================================================================================  
#    Get MenuSequences
# ==================================================================================================== 

def getBJetSequence( step ):
    if step == "j":
        return bJetStep1Sequence()
    if step == "jALLTE":
        return bJetStep1SequenceALLTE()
    if step == "gsc":
        return bJetStep2Sequence()
    if step == "gscALLTE":
        return bJetStep2SequenceALLTE()
    if step == "bTag":
        return bJetStep1Sequence()
    return None

# ==================================================================================================== 
#    step 1: jet reco and cut on 'j' threshold 
# ==================================================================================================== 

def bJetStep1Sequence():

    from TrigUpgradeTest.jetMenuHelper import jetRecoSequenceFromString
    (recoSequence, InputMakerAlg, sequenceOut) = jetRecoSequenceFromString("a4_tc_em_subjes")
				 

    # Start with b-jet-specific algo sequence
    # Construct RoI. Needed input for Fast Tracking
    from TrigBjetHypo.TrigBjetHypoConf import TrigRoiBuilderMT
    RoIBuilder = TrigRoiBuilderMT("RoIBuilder")
    RoIBuilder.JetInputKey = sequenceOut
    RoIBuilder.RoIOutputKey = "BjetRoIs"
    RoIs=RoIBuilder.RoIOutputKey

    # Fast Tracking

    (viewAlgs, eventAlgs) = makeInDetAlgs(whichSignature='FS',separateTrackParticleCreator="_FS")

    for viewAlg in viewAlgs:
        if "RoIs" in viewAlg.properties():
            viewAlg.RoIs = RoIs
        if "roiCollectionName" in viewAlg.properties():
            viewAlg.roiCollectionName = RoIs
        if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
#            TrackParticlesName = viewAlg.TrackParticlesName # why we need this?
            TrackCollection = viewAlg.TrackName

    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Jet    
    theFTF_Jet = TrigFastTrackFinder_Jet()
    theFTF_Jet.isRoI_Seeded = True
    theFTF_Jet.RoIs = RoIs
    theFTF_Jet.TracksName=TrackCollection
    viewAlgs.append( theFTF_Jet )

    # Primary Vertex goes here

    # Shortlis of jets
    from TrigBjetHypo.TrigBjetHypoConf import TrigJetSplitterMT
    jetSplitter = TrigJetSplitterMT("TrigJetSplitterMT")
    jetSplitter.ImposeZconstraint = True
    jetSplitter.Jets = sequenceOut
    jetSplitter.OutputJets = "SplitJets"
    jetSplitter.OutputRoi = "SplitJets"
    jetSplitter.OutputVertex = "PrimaryVertex" # TMP

    fastTrackingSequence = parOR("fastTrackingSequence",viewAlgs)
    bJetEtSequence = seqAND("bJetEtSequence",[ RoIBuilder,fastTrackingSequence,jetSplitter] )

    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlgMT
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromDict_j
    hypo = TrigBjetEtHypoAlgMT("TrigBjetEtHypoAlgMT_step1")
    hypo.Jets = jetSplitter.OutputJets
    hypo.RoIs = jetSplitter.OutputRoi
    hypo.RoILink = "step1RoI" # To be used in following step EventView
    hypo.JetLink = "jets" # To be used in following step with EventView

    # Sequence     
    BjetAthSequence = seqAND("BjetAthSequence_step1",eventAlgs + [InputMakerAlg,recoSequence,bJetEtSequence])

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromDict_j )

def bJetStep1SequenceALLTE():

    # Construct jets
    from TrigUpgradeTest.jetMenuHelper import jetRecoSequenceFromString
	#from TrigUpgradeTest.jetDefs import jetAthSequence


    (recoSequence, InputMakerAlg, sequenceOut) = jetRecoSequenceFromString("a4_tc_em_subjes")
    #(recoSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(jetAthSequence,ConfigFlags)
    # Start with b-jet-specific algo sequence
    # Construct RoI. Needed input for Fast Tracking
    from TrigBjetHypo.TrigBjetHypoConf import TrigRoiBuilderMT
    RoIBuilder = TrigRoiBuilderMT("RoIBuilder")
    RoIBuilder.JetInputKey = sequenceOut
    RoIs="EMViewRoIs" # Default for Fast Tracking Algs
    RoIBuilder.RoIOutputKey = RoIs


    # Fast Tracking 
    (viewAlgs, eventAlgs) = makeInDetAlgs(whichSignature='FS',separateTrackParticleCreator="_FS")


    for viewAlg in viewAlgs:
        if "RoIs" in viewAlg.properties():
            viewAlg.RoIs = RoIs
        if "roiCollectionName" in viewAlg.properties():
            viewAlg.roiCollectionName = RoIs
        if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
#            TrackParticlesName = viewAlg.TrackParticlesName # why we need this?
            TrackCollection = viewAlg.TrackName

    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Jet    
    theFTF_Jet = TrigFastTrackFinder_Jet()
    theFTF_Jet.isRoI_Seeded = True
    theFTF_Jet.RoIs = RoIs
    theFTF_Jet.TracksName=TrackCollection
    viewAlgs.append( theFTF_Jet )

    # Primary Vertex goes here

    # Shortlis of jets
    from TrigBjetHypo.TrigBjetHypoConf import TrigJetSplitterMT
    jetSplitter = TrigJetSplitterMT("TrigJetSplitterMT")
    jetSplitter.ImposeZconstraint = True 
    jetSplitter.Jets = sequenceOut
    jetSplitter.OutputJets = "SplitJets"
    jetSplitter.OutputRoi = "SplitJets"
    jetSplitter.OutputVertex = "PrimaryVertex" # TMP

    fastTrackingSequence = parOR("fastTrackingSequence",viewAlgs)
    bJetEtSequence = seqAND("bJetEtSequence",[ RoIBuilder,fastTrackingSequence,jetSplitter] )

    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlgMT
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromDict_j
    hypo = TrigBjetEtHypoAlgMT("TrigBjetEtHypoAlgMT_step1_ALLTE")
    hypo.Jets = jetSplitter.OutputJets
    hypo.RoIs = jetSplitter.OutputRoi
    hypo.RoILink = "initialRoI" # To be used in following step EventView
    hypo.JetLink = "jets" # To be used in following step with EventView

    # Sequence     
    BjetAthSequence = seqAND("BjetAthSequence_step1_ALLTE",eventAlgs + [InputMakerAlg,recoSequence,bJetEtSequence])

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromDict_j )



# ==================================================================================================== 
#    step 2: precision tracking, gsc calibration and cut on gsc-corrected threshold
# ==================================================================================================== 

def bJetStep2Sequence():

    # Event View Creator Algorithm
    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithmWithJets
    InputMakerAlg = EventViewCreatorAlgorithmWithJets("BJetInputMaker_step2", RoIsLink="initialRoI")
    InputMakerAlg.ViewFallThrough = True # Access Store Gate for retrieving data
    InputMakerAlg.ViewPerRoI = True # If True it creates one view per RoI
    InputMakerAlg.Views = "BJetViews" # Name of output view
    # RoIs
    InputMakerAlg.InViewRoIs = "InViewRoIs" # Name RoIs are inserted in the view
    InputMakerAlg.RoIsLink = "step1RoI" # RoIs linked to previous decision
    # Jets
    InputMakerAlg.InViewJets = "InViewJets" # Name Jets are inserted in the view
    InputMakerAlg.JetsLink = "jets" # Jets linked to previous decision

    # Precision Tracking
    #def makeInDetPrecisionTracking( whichSignature, verifier = False, inputFTFtracks='TrigFastTrackFinder_Tracks', outputTrackPrefixName = "InDetTrigPT" ):
    from TrigUpgradeTest.InDetPT import makeInDetPrecisionTracking
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "bjets", inputFTFtracks="TrigFastTrackFinder_Tracks_FS" )

    # gsc correction
    from TrigBjetHypo.TrigGSCFexMTConfig import getGSCFexSplitInstance
    theGSC = getGSCFexSplitInstance("GSCFexSplitInstance")
    theGSC.RoIs = InputMakerAlg.InViewRoIs
    theGSC.JetKey = InputMakerAlg.InViewJets
    theGSC.TrackKey = PTTrackParticles[0]
    theGSC.PriVtxKey = "PrimaryVertex"
    theGSC.JetOutputKey = "GSCJets"

    step2Sequence = seqAND("step2Sequence",[theGSC])
    InputMakerAlg.ViewNodeName = "step2Sequence"

    # to debug the View contnets  
    import AthenaCommon.CfgMgr as CfgMgr
    step2Sequence += CfgMgr.AthViews__ViewTestAlg("view_testBjet2")
    
    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlgMT
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromDict_gsc
    hypo = TrigBjetEtHypoAlgMT("TrigBjetEtHypoAlg_step2")
    hypo.RoIs = "step1RoI"
    hypo.Jets = theGSC.JetOutputKey
    hypo.RoILink = InputMakerAlg.RoIsLink # To be used in following step EventView
    hypo.JetLink = InputMakerAlg.JetsLink # To be used in following step with EventView
    hypo.ReadFromView = True

    # Sequence
    BjetAthSequence = seqAND("BjetAthSequence_step2",[InputMakerAlg] + PTAlgs + [step2Sequence])

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromDict_gsc )


def bJetStep2SequenceALLTE():
    # input maker
    from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
    InputMakerAlg = InputMakerForRoI("BJetInputMaker_step2_ALLTE", RoIsLink="initialRoI")
#    InputMakerAlg.RoIs="SplitJets" # TMP commenting

    from TrigUpgradeTest.InDetPT import makeInDetPrecisionTracking
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "bjets" )  
    
    # gsc correction
    from TrigBjetHypo.TrigGSCFexMTConfig import getGSCFexSplitInstance
    theGSC = getGSCFexSplitInstance("GSCFexSplitInstance_ALLTE")
    theGSC.TrackKey = PTTrackParticles[0]
    theGSC.JetKey = "SplitJets"
    theGSC.JetOutputKey = "GSCJets"
    theGSC.PriVtxKey = "PrimaryVertex"

    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlgMT
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromDict_gsc
    hypo = TrigBjetEtHypoAlgMT("TrigBjetEtHypoAlg_step2ALLTE")
    hypo.Jets = theGSC.JetOutputKey
    hypo.RoIs = "SplitJets"
    hypo.RoILink = "initialRoI"
    hypo.JetLink = "jets"


    # Sequence
    BjetAthSequence = seqAND("BjetAthSequence_step2ALLTE",[InputMakerAlg] + PTAlgs + [theGSC] )

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromDict_gsc )

# ==================================================================================================== 
#    step 3: secondary vertex and b-tagging
# ==================================================================================================== 


