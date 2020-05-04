#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

# menu components
from AthenaCommon.CFElements import seqAND
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, InViewReco
from TrigEDMConfig.TriggerEDMRun3 import recordable

#from AthenaCommon.Constants import DEBUG

# ====================================================================================================  
#    Get MenuSequences
# ==================================================================================================== 

def getBJetSequence( step ):
    if step == "j":
        return bJetStep1Sequence()
    if step == "btag":
        return bJetStep2Sequence()
    return None

# ==================================================================================================== 
#    step 1: This is Jet code. Not here!
# ==================================================================================================== 

# ==================================================================================================== 
#    step 2: retrieving jets from jet-trigger code and optional first stage of fast tracking with primary vertex
# ==================================================================================================== 

def bJetStep1Sequence():
    jetsKey = "HLT_AntiKt4EMTopoJets_subjesgscIS_ftf"
    prmVtxKey = "HLT_EFHistoPrmVtx"

    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm, ViewCreatorInitialROITool
    InputMakerAlg = EventViewCreatorAlgorithm( "IMBJet_step2", RoIsLink="initialRoI" )
    InputMakerAlg.RoITool = ViewCreatorInitialROITool()
    InputMakerAlg.Views = "FullScanBjetView"
    InputMakerAlg.InViewRoIs = "FullScanRoI"
    InputMakerAlg.ViewFallThrough = True
    InputMakerAlg.mergeUsingFeature = True

    outputJetName = "HLT_GSCJet"
    outputRoIName = "HLT_GSCJet_RoIs"

    # Jet Selector
    from TrigBjetHypo.TrigBjetHypoConf import TrigJetSelectorMT
    jetSelector = TrigJetSelectorMT( "BJetSelector" )
    jetSelector.JetMaxEta = 2.9
    jetSelector.InputJets = jetsKey
    jetSelector.InputVertex = prmVtxKey
    jetSelector.OutputJets = recordable( outputJetName )
    jetSelector.OutputRoi = outputRoIName

    bJetEtSequence = seqAND( "bJetEtSequence",[jetSelector] )
    InputMakerAlg.ViewNodeName = "bJetEtSequence"

    # Sequence
    BjetAthSequence = seqAND("BjetAthSequence_step1",[InputMakerAlg,bJetEtSequence])

    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlgMT
    hypo = TrigBjetEtHypoAlgMT( "TrigBjetEtHypoAlgMT" )
    hypo.RoILink = "step1RoI"
    hypo.PrmVtxLink = prmVtxKey.replace( "HLT_","" )
    hypo.Jets = outputJetName
    hypo.RoIs = outputRoIName
    hypo.PrmVtx = prmVtxKey
    hypo.RetrieveVertexFromEventView = False

    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromDict
    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromDict )

# ==================================================================================================== 
#    step 3: Second stage of fast tracking, Precision tracking, and flavour tagging
# ==================================================================================================== 

def bJetStep2Sequence():
    roisLink = "step1RoI"
    prmVtxKey = "HLT_EFHistoPrmVtx"

    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithmWithJets, ViewCreatorInitialROITool
    InputMakerAlg = EventViewCreatorAlgorithmWithJets( "IMBJet_step3",RoIsLink=roisLink )
    InputMakerAlg.ViewFallThrough = True
    InputMakerAlg.RequireParentView = True
    InputMakerAlg.RoITool = ViewCreatorInitialROITool() # NOT USED! TO BE REPLACED WITH NEW TOOL ON CONVERTING EventViewCreatorAlgorithmWithJets -> EventViewCreatorAlgorithm
    InputMakerAlg.Views = "BTagViews"
    InputMakerAlg.InViewRoIs = "InViewRoIs"
    InputMakerAlg.InViewJets = recordable ( "HLT_InViewJets" )
    InputMakerAlg.ViewNodeName = "bJetBtagSequenceInView" 
    
    # Second stage of Fast Tracking and Precision Tracking
    from TriggerMenuMT.HLTMenuConfig.Bjet.BjetTrackingConfiguration import getSecondStageBjetTracking
    secondStageAlgs, PTTracks, PTTrackParticles = getSecondStageBjetTracking( inputRoI=InputMakerAlg.InViewRoIs )

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    
    # Flavour Tagging
    from TriggerMenuMT.HLTMenuConfig.Bjet.BjetFlavourTaggingConfiguration import getFlavourTagging
    acc_flavourTaggingAlgs = getFlavourTagging( inputJets=InputMakerAlg.InViewJets, inputVertex=prmVtxKey, inputTracks=PTTrackParticles[0] )
    
    inViewReco = InViewReco("bJetBtagSequence", viewMaker= InputMakerAlg)
    inViewReco.addRecoAlg(secondStageAlgs)
    inViewReco.mergeReco(acc_flavourTaggingAlgs)
    acc_flavourTaggingAlgs.wasMerged()
    inViewReco.wasMerged()

    Configurable.configurableRun3Behavior=0
    
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetBtagHypoAlgMT
    hypo = TrigBjetBtagHypoAlgMT( "TrigBjetBtagHypoAlg" )
    hypo.Tracks = PTTrackParticles[0]

    from TrigBjetHypo.TrigBjetBtagHypoTool import TrigBjetBtagHypoToolFromDict
    return MenuSequence( Sequence    = inViewReco.sequence(),
                         Maker       = inViewReco.inputMaker(),
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetBtagHypoToolFromDict)



