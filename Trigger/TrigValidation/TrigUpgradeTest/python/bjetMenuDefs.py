#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO
import AthenaCommon.CfgMgr as CfgMgr

# Set InDet Flags
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doCaloSeededBrem = False
InDetFlags.InDet25nsec = True 
InDetFlags.doPrimaryVertex3DFinding = False 
InDetFlags.doPrintConfigurables = False
InDetFlags.doResolveBackTracks = True 
InDetFlags.doSiSPSeededTrackFinder = True
InDetFlags.doTRTPhaseCalculation = True
InDetFlags.doTRTSeededTrackFinder = True
InDetFlags.doTruth = False
InDetFlags.init()

from AthenaCommon.Include import include
include("InDetRecExample/InDetRecConditionsAccess.py")

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
    # menu components
    from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence

    # input maker
    from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
    InputMakerAlg = InputMakerForRoI("JetInputMaker", OutputLevel = DEBUG, LinkName="initialRoI")
    InputMakerAlg.RoIs='FSJETRoI'
    InputMakerAlg.OutputLevel = DEBUG
    InputMakerAlg.LinkName = "initialRoI"

    # Construct jets
    from TrigUpgradeTest.jetDefs import jetRecoSequence
    (recoSequence, sequenceOut) = jetRecoSequence( InputMakerAlg.RoIs )

    # Start with b-jet-specific algo sequence
    # Construct RoI. Needed input for Fast Tracking
    from TrigBjetHypo.TrigBjetHypoConf import TrigRoiBuilderMT
    RoIBuilder = TrigRoiBuilderMT("RoIBuilder")
    RoIBuilder.OutputLevel = DEBUG
    RoIBuilder.JetInputKey = sequenceOut
    RoIBuilder.RoIOutputKey = "EMViewRoIs" # Default for Fast Tracking Algs

    # Fast Tracking 
    from TrigUpgradeTest.InDetSetup import makeInDetAlgs
    (viewAlgs, eventAlgs) = makeInDetAlgs()

    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Jet    
    theFTF_Jet = TrigFastTrackFinder_Jet()
    theFTF_Jet.OutputLevel = DEBUG
    theFTF_Jet.isRoI_Seeded = True
    theFTF_Jet.RoIs = RoIBuilder.RoIOutputKey
    viewAlgs.append( theFTF_Jet )

    # Getting output track particle container name
    TrackParticlesName = ""
    for viewAlg in viewAlgs:
        if viewAlg.name() == "InDetTrigTrackParticleCreatorAlg":
            TrackParticlesName = viewAlg.TrackParticlesName

    # Primary Vertex goes here

    # Shortlis of jets
    from TrigBjetHypo.TrigBjetHypoConf import TrigJetSplitterMT
    jetSplitter = TrigJetSplitterMT("TrigJetSplitterMT")
    jetSplitter.OutputLevel = DEBUG
    jetSplitter.ImposeZconstraint = True 
    jetSplitter.Jets = sequenceOut
    jetSplitter.OutputJets = "SplitJets"
    jetSplitter.OutputRoi = "SplitJets"
    jetSplitter.OutputVertex = "PrimaryVertex" # TMP

    fastTrackingSequence = parOR("fastTrackingSequence",viewAlgs)
    bJetEtSequence = seqAND("bJetEtSequence",[ RoIBuilder,fastTrackingSequence,jetSplitter] )

    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlgMT
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromName_j
    hypo = TrigBjetEtHypoAlgMT("TrigBjetEtHypoAlgMT_step1")
    hypo.OutputLevel = DEBUG
    hypo.Jets = jetSplitter.OutputJets
    hypo.RoIs = jetSplitter.OutputRoi
    hypo.PrimaryVertex = jetSplitter.OutputVertex
    hypo.RoILink = "initialRoI" # To be used in following step EventView
    hypo.JetLink = "jets" # To be used in following step with EventView
    hypo.PrimaryVertexLink = "primaryVertex" # To be used in following step with EventView  
    hypo.MultipleDecisions = True # For creating multiple decisions in the next step  

    # Sequence     
    BjetAthSequence = seqAND("BjetAthSequence_step1",eventAlgs + [InputMakerAlg,recoSequence,bJetEtSequence])

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromName_j )

def bJetStep1SequenceALLTE():
    # menu components
    from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence

    # input maker
    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
    InputMakerAlg = HLTTest__TestInputMaker("BJetInputMaker_step1")
    InputMakerAlg.LinkName = "initialRoI"
    InputMakerAlg.Output = "FSJETRoIs"

    # Construct jets
    from TrigUpgradeTest.jetDefs import jetRecoSequence
    (recoSequence, sequenceOut) = jetRecoSequence( InputMakerAlg.Output )

    # Start with b-jet-specific algo sequence
    # Construct RoI. Needed input for Fast Tracking
    from TrigBjetHypo.TrigBjetHypoConf import TrigRoiBuilderMT
    RoIBuilder = TrigRoiBuilderMT("RoIBuilder")
    RoIBuilder.JetInputKey = sequenceOut
    RoIBuilder.RoIOutputKey = "EMViewRoIs" # Default for Fast Tracking Algs

    # Fast Tracking 
    from TrigUpgradeTest.InDetSetup import makeInDetAlgs
    (viewAlgs, eventAlgs) = makeInDetAlgs()

    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Jet    
    theFTF_Jet = TrigFastTrackFinder_Jet()
    theFTF_Jet.isRoI_Seeded = True
    theFTF_Jet.RoIs = RoIBuilder.RoIOutputKey
    viewAlgs.append( theFTF_Jet )

    # Getting output track particle container name
    TrackParticlesName = ""
    for viewAlg in viewAlgs:
        if viewAlg.name() == "InDetTrigTrackParticleCreatorAlg":
            TrackParticlesName = viewAlg.TrackParticlesName

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
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromName_j
    hypo = TrigBjetEtHypoAlgMT("TrigBjetEtHypoAlgMT_step1_ALLTE")
    hypo.OutputLevel = DEBUG
    hypo.Jets = jetSplitter.OutputJets
    hypo.RoIs = jetSplitter.OutputRoi
    hypo.PrimaryVertex = jetSplitter.OutputVertex
    hypo.RoILink = "initialRoI" # To be used in following step EventView
    hypo.JetLink = "jets" # To be used in following step with EventView
    hypo.PrimaryVertexLink = "primaryVertex" # To be used in following step with EventView  
    hypo.MultipleDecisions = False # For creating multiple decisions in the next step  

    # Sequence     
    BjetAthSequence = seqAND("BjetAthSequence_step1_ALLTE",eventAlgs + [InputMakerAlg,recoSequence,bJetEtSequence])

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromName_j )



# ==================================================================================================== 
#    step 2: precision tracking, gsc calibration and cut on gsc-corrected threshold
# ==================================================================================================== 

def bJetStep2Sequence():
    # menu components
    from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence

    # Event View Creator Algorithm
    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithmForBjet
    InputMakerAlg = EventViewCreatorAlgorithmForBjet("BJetInputMaker_step2")
    InputMakerAlg.OutputLevel = DEBUG
    InputMakerAlg.ViewFallThrough = True # Access Store Gate for retrieving data
    InputMakerAlg.ViewPerRoI = True # If True it creates one view per RoI
    InputMakerAlg.Views = "BJetViews" # Name of output view
    # RoIs
    InputMakerAlg.InViewRoIs = "InViewRoIs" # Name RoIs are inserted in the view
    InputMakerAlg.RoIsLink = "initialRoI" # RoIs linked to previous decision
    # Jets
    InputMakerAlg.InViewJets = "InViewJets" # Name Jets are inserted in the view
    InputMakerAlg.JetsLink = "jets" # Jets linked to previous decision
    # Primary Vertex
    InputMakerAlg.InViewPrimaryVertex = "InViewPrimaryVertex" # Name Primary Vertex are inserted in the view      
    InputMakerAlg.PrimaryVertexLink = "primaryVertex" # Primary Vertex linked to previous decision

    # Precision Tracking

    # gsc correction
    from TrigBjetHypo.TrigGSCFexMTConfig import getGSCFexSplitInstance
    theGSC = getGSCFexSplitInstance("GSCFexSplitInstance")
    theGSC.OutputLevel = DEBUG
    theGSC.RoIs = InputMakerAlg.InViewRoIs
    theGSC.JetKey = InputMakerAlg.InViewJets
    theGSC.PriVtxKey = InputMakerAlg.InViewPrimaryVertex
    theGSC.JetOutputKey = "GSCJets"

    step2Sequence = seqAND("step2Sequence",[theGSC]);
    InputMakerAlg.ViewNodeName = "step2Sequence"
    
    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlgMT
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromName_gsc
    hypo = TrigBjetEtHypoAlgMT("TrigBjetEtHypoAlg_step2")
    hypo.OutputLevel = DEBUG
    hypo.RoIs = "initialRoI" #InputMakerAlg.InViewRoIs
    hypo.Jets = theGSC.JetOutputKey
    hypo.PrimaryVertex = InputMakerAlg.InViewPrimaryVertex

    hypo.RoILink = InputMakerAlg.RoIsLink #"initialRoI" # To be used in following step EventView
    hypo.JetLink = InputMakerAlg.JetsLink #"jets" # To be used in following step with EventView
    hypo.PrimaryVertexLink = InputMakerAlg.PrimaryVertexLink
    hypo.UseView = True

    # Sequence
    BjetAthSequence = seqAND("BjetAthSequence_step2",[InputMakerAlg,step2Sequence])

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromName_gsc )


def bJetStep2SequenceALLTE():
    # menu components
    from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence

    # input maker
    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
    InputMakerAlg = HLTTest__TestInputMaker("BJetInputMaker_step2_ALLTE")
    InputMakerAlg.LinkName = "initialRoI"
    InputMakerAlg.Output = "SplitJets"
    
    # gsc correction
    from TrigBjetHypo.TrigGSCFexMTConfig import getGSCFexSplitInstance
    theGSC = getGSCFexSplitInstance("GSCFexSplitInstance_ALLTE")
    theGSC.OutputLevel = DEBUG
    theGSC.JetKey = "SplitJets"
    theGSC.JetOutputKey = "GSCJets"
    theGSC.PriVtxKey = "PrimaryVertex"

    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlgMT
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromName_gsc
    hypo = TrigBjetEtHypoAlgMT("TrigBjetEtHypoAlg_step2ALLTE")
    hypo.OutputLevel = DEBUG
    hypo.Jets = theGSC.JetOutputKey
    hypo.RoIs = InputMakerAlg.Output
    hypo.PrimaryVertex = "PrimaryVertex"
    hypo.RoILink = "initialRoI"
    hypo.JetLink = "jets"

    # Sequence
    BjetAthSequence = seqAND("BjetAthSequence_step2ALLTE",[InputMakerAlg,theGSC])

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromName_gsc )

# ==================================================================================================== 
#    step 3: secondary vertex and b-tagging
# ==================================================================================================== 


