#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO
import AthenaCommon.CfgMgr as CfgMgr
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.Include import include

# Set InDet Flags
def inDetSetup():
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
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool

    from TrigUpgradeTest.jetDefs import jetAthSequence
    (recoSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(jetAthSequence,ConfigFlags)

     
    ## # input maker
    ## from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
    ## InputMakerAlg = InputMakerForRoI("JetInputMaker", OutputLevel = DEBUG, RoIsLink="initialRoI")
    ## InputMakerAlg.RoIs='FSJETRoI'
    ## InputMakerAlg.OutputLevel = DEBUG


    # Construct jets
    #from TrigUpgradeTest.jetDefs import jetRecoSequence    
    #(recoSequence, sequenceOut) = jetRecoSequence( InputMakerAlg.RoIs )

    # Start with b-jet-specific algo sequence
    # Construct RoI. Needed input for Fast Tracking
    from TrigBjetHypo.TrigBjetHypoConf import TrigRoiBuilderMT
    RoIBuilder = TrigRoiBuilderMT("RoIBuilder")
    RoIBuilder.OutputLevel = DEBUG
    RoIBuilder.JetInputKey = sequenceOut
    RoIBuilder.RoIOutputKey = "EMViewRoIs" # Default for Fast Tracking Algs

    # Fast Tracking 
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
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
        viewAlg.OutputLevel = INFO
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
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromDict_j
    hypo = TrigBjetEtHypoAlgMT("TrigBjetEtHypoAlgMT_step1")
    hypo.OutputLevel = DEBUG
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
    # menu components
    from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool

    # Construct jets
    from TrigUpgradeTest.jetDefs import jetAthSequence
    (recoSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(jetAthSequence,ConfigFlags)

    ## # input maker
    ## from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
    ## InputMakerAlg = InputMakerForRoI("JetInputMaker",OutputLevel=INFO, RoIsLink="initialRoI")
    ## InputMakerAlg.RoIs='FSJETRoI'

    ## # Construct jets
    ## from TrigUpgradeTest.jetDefs import jetRecoSequence
    ## (recoSequence, sequenceOut) = jetRecoSequence( InputMakerAlg.RoIs )

    # Start with b-jet-specific algo sequence
    # Construct RoI. Needed input for Fast Tracking
    from TrigBjetHypo.TrigBjetHypoConf import TrigRoiBuilderMT
    RoIBuilder = TrigRoiBuilderMT("RoIBuilder")
    RoIBuilder.JetInputKey = sequenceOut
    RoIBuilder.RoIOutputKey = "EMViewRoIs" # Default for Fast Tracking Algs

    # Fast Tracking 
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
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
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromDict_j
    hypo = TrigBjetEtHypoAlgMT("TrigBjetEtHypoAlgMT_step1_ALLTE")
    hypo.OutputLevel = DEBUG
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
    # menu components
    from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence

    # Event View Creator Algorithm
    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithmWithJets
    InputMakerAlg = EventViewCreatorAlgorithmWithJets("BJetInputMaker_step2", RoIsLink="initialRoI")
    InputMakerAlg.OutputLevel = DEBUG
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

    # gsc correction
    from TrigBjetHypo.TrigGSCFexMTConfig import getGSCFexSplitInstance
    theGSC = getGSCFexSplitInstance("GSCFexSplitInstance")
    theGSC.OutputLevel = DEBUG
    theGSC.RoIs = InputMakerAlg.InViewRoIs
    theGSC.JetKey = InputMakerAlg.InViewJets
    theGSC.PriVtxKey = "PrimaryVertex"
    theGSC.JetOutputKey = "GSCJets"

    step2Sequence = seqAND("step2Sequence",[theGSC]);
    InputMakerAlg.ViewNodeName = "step2Sequence"
    
    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlgMT
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromDict_gsc
    hypo = TrigBjetEtHypoAlgMT("TrigBjetEtHypoAlg_step2")
    hypo.OutputLevel = DEBUG
    hypo.RoIs = "step1RoI"
    hypo.Jets = theGSC.JetOutputKey
    hypo.RoILink = InputMakerAlg.RoIsLink # To be used in following step EventView
    hypo.JetLink = InputMakerAlg.JetsLink # To be used in following step with EventView
    hypo.ReadFromView = True

    # Sequence
    BjetAthSequence = seqAND("BjetAthSequence_step2",[InputMakerAlg,step2Sequence])

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromDict_gsc )


def bJetStep2SequenceALLTE():
    # menu components
    from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence

    # input maker
    from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
    InputMakerAlg = InputMakerForRoI("BJetInputMaker_step2_ALLTE", RoIsLink="initialRoI")
    InputMakerAlg.OutputLevel = DEBUG
#    InputMakerAlg.RoIs="SplitJets" # TMP commenting
    
    # gsc correction
    from TrigBjetHypo.TrigGSCFexMTConfig import getGSCFexSplitInstance
    theGSC = getGSCFexSplitInstance("GSCFexSplitInstance_ALLTE")
    theGSC.OutputLevel = DEBUG
    theGSC.JetKey = "SplitJets"
    theGSC.JetOutputKey = "GSCJets"
    theGSC.PriVtxKey = "PrimaryVertex"

    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlgMT
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromDict_gsc
    hypo = TrigBjetEtHypoAlgMT("TrigBjetEtHypoAlg_step2ALLTE")
    hypo.OutputLevel = DEBUG
    hypo.Jets = theGSC.JetOutputKey
    hypo.RoIs = "SplitJets"
    hypo.RoILink = "initialRoI"
    hypo.JetLink = "jets"

    # Sequence
    BjetAthSequence = seqAND("BjetAthSequence_step2ALLTE",[InputMakerAlg,theGSC])

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromDict_gsc )

# ==================================================================================================== 
#    step 3: secondary vertex and b-tagging
# ==================================================================================================== 


