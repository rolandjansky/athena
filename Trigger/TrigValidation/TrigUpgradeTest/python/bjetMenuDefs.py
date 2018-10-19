#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO

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
    if step == "gsc":
        return bJetStep2Sequence()
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
    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
    InputMakerAlg = HLTTest__TestInputMaker("BJetInputMaker_step1")
#    InputMakerAlg.OutputLevel = DEBUG
    InputMakerAlg.LinkName = "initialRoI"
    InputMakerAlg.Output = "FSJETRoIs"

    # Construct jets
    from TrigUpgradeTest.jetDefs import jetRecoSequence
    (recoSequence, sequenceOut) = jetRecoSequence( InputMakerAlg.Output )

    # Start with b-jet-specific algo sequence
    # Construct RoI. Needed input for Fast Tracking
    # WILL BE REMOVED IN THE FUTURE 
    from TrigBjetHypo.TrigBjetHypoConf import TrigRoiBuilderMT
    RoIBuilder = TrigRoiBuilderMT("RoIBuilder")
#    RoIBuilder.OutputLevel = DEBUG
    RoIBuilder.JetInputKey = sequenceOut
    RoIBuilder.RoIOutputKey = "EMViewRoIs" # Default for Fast Tracking Algs

    # Fast Tracking 
    from TrigUpgradeTest.InDetSetup import makeInDetAlgs
    (viewAlgs, eventAlgs) = makeInDetAlgs()

    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Jet    
    theFTF_Jet = TrigFastTrackFinder_Jet()
#    theFTF_Jet.OutputLevel = DEBUG
    theFTF_Jet.isRoI_Seeded = True
    theFTF_Jet.RoIs = RoIBuilder.RoIOutputKey
    viewAlgs.append( theFTF_Jet )

    # Getting output track particle container name
    TrackParticlesName = ""
    for viewAlg in viewAlgs:
        if viewAlg.name() == "InDetTrigTrackParticleCreatorAlg":
            TrackParticlesName = viewAlg.TrackParticlesName

    fastTrackingSequence = parOR("fastTrackingSequence",viewAlgs)
    bJetEtSequence = seqAND("bJetEtSequence",[ RoIBuilder,fastTrackingSequence] )

    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlg
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromName
    hypo = TrigBjetEtHypoAlg("TrigBjetEtHypoAlg_step1")
    hypo.OutputLevel = DEBUG
    hypo.Jets = sequenceOut
    hypo.OutputJets = "SplitJets"
    # These two are only for temporary debug. Will be removed 
    hypo.TrackParticleContainerKey = TrackParticlesName
    hypo.RoiKey = RoIBuilder.RoIOutputKey

    # Sequence     
    BjetAthSequence = seqAND("BjetAthSequence_step1",eventAlgs + [InputMakerAlg,recoSequence,bJetEtSequence])

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromName )


# ==================================================================================================== 
#    step 2: precision tracking, gsc calibration and cut on gsc-corrected threshold
# ==================================================================================================== 

def bJetStep2Sequence():
    # menu components
    from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence

    # Event View Creator Algorithm
#    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
#    InputMakerAlg = EventViewCreatorAlgorithm("l2ElectronViewsMaker")
#    InputMakerAlg.OutputLevel = DEBUG
#    InputMakerAlg.ViewFallThrough = True
#    InputMakerAlg.RoIsLink = "roi"
#    InputMakerAlg.InViewRoIs = "BJetStep2RoI"
#    InputMakerAlg.Views = "BJetViews"

    # input maker
    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
    InputMakerAlg = HLTTest__TestInputMaker("BJetInputMaker_step2")
    InputMakerAlg.OutputLevel = DEBUG
    InputMakerAlg.LinkName = "initialRoI"
    InputMakerAlg.Output = "SplitJet"

    # gsc correction
    from TrigBjetHypo.TrigGSCFexMTConfig import getGSCFexSplitInstance
    theGSC = getGSCFexSplitInstance("EF","2012","EFID")
    theGSC.OutputLevel = DEBUG
    theGSC.JetKey = "SplitJet"
    theGSC.JetOutputKey = "GSCJet"

    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlg
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromName
    hypo = TrigBjetEtHypoAlg("TrigBjetEtHypoAlg_step2")
    hypo.OutputLevel = DEBUG
    hypo.Jets = theGSC.JetOutputKey
    hypo.RoiKey = theGSC.JetOutputKey

    # Sequence
    BjetAthSequence = seqAND("BjetAthSequence_step2",[InputMakerAlg,theGSC])

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromName )

# ==================================================================================================== 
#    step 3: secondary vertex and b-tagging
# ==================================================================================================== 

