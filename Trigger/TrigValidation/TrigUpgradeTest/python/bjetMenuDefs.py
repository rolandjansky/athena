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
        return bJetStep1Sequence()
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
    InputMakerAlg.OutputLevel = DEBUG
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

    fastTrackingSequence = parOR("fastTrackingSequence",viewAlgs)
    bJetEtSequence = seqAND("bJetEtSequence",[ RoIBuilder,fastTrackingSequence] )

    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlgMT
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromName
    hypo = TrigBjetEtHypoAlgMT("TrigBjetEtHypoAlgMT")
    hypo.OutputLevel = DEBUG
    hypo.Jets = sequenceOut
    hypo.OutputJets = "SplitJets"
    # These two are only for temporary debug. Will be removed 
    hypo.TrackParticleContainerKey = TrackParticlesName
    hypo.RoiKey = RoIBuilder.RoIOutputKey

    # Sequence     
    BjetAthSequence = seqAND("BjetAthSequence",eventAlgs + [InputMakerAlg,recoSequence,bJetEtSequence])

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromName )


# ==================================================================================================== 
#    step 2: precision tracking, gsc calibration and cut on gsc-corrected threshold
# ==================================================================================================== 

# ==================================================================================================== 
#    step 3: secondary vertex and b-tagging
# ==================================================================================================== 

