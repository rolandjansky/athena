#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO

# My idea would be to create three steps, thus three different hypoTools
# 1 TFT + Jet Reco + j cut
# 2 Precision Tracking + GSC + gsc cut
# 3 BTagging

def getBJetSequence( step ):
    if step == "j":
        return bJetStep1Sequence()
    if step == "gsc":
        return bJetSequence()
    if step == "bTag":
        return bJetSequence()
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

    # Construct jets ( how do I impose split or non-split configuration ? ) 
    from TrigUpgradeTest.jetDefs import jetRecoSequence
    (recoSequence, sequenceOut) = jetRecoSequence( InputMakerAlg.Output )

    # Start with b-jet-specific algo sequence
    bJetEtSequence = parOR("bJetEtSequence")
    # Construct Super RoI
    from TrigBjetHypo.TrigBjetHypoConf import TrigSuperRoiBuilderMT
    SuperRoIBuilder = TrigSuperRoiBuilderMT("SuperRoIBuilder")
    SuperRoIBuilder.OutputLevel = DEBUG
    SuperRoIBuilder.JetInputKey = sequenceOut
    SuperRoIBuilder.JetOutputKey = "superRoi"
    SuperRoIBuilder.SuperRoIOutputKey = SuperRoIBuilder.JetOutputKey # Same as Output Jet Collection
    bJetEtSequence += SuperRoIBuilder

    # Fast Tracking 
    from TrigUpgradeTest.InDetSetup import makeInDetAlgs
    (viewAlgs, eventAlgs) = makeInDetAlgs()

    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Jet    
    theFTF = TrigFastTrackFinder_Jet()
    theFTF.RoIs = SuperRoIBuilder.SuperRoIOutputKey
    viewAlgs.append(theFTF)

    TrackParticlesName = ""
    for viewAlg in viewAlgs:
        print 'view Alg Name :',viewAlg.name()
        viewAlg.OutputLevel = DEBUG
        if viewAlg.name() == "InDetTrigTrackParticleCreatorAlg":
            print '   ** Setting TrigRoiDescriptorCollection to', SuperRoIBuilder.SuperRoIOutputKey
            viewAlg.roiCollectionName = SuperRoIBuilder.SuperRoIOutputKey
        if viewAlg.name() in ["InDetPixelRawDataProvider","InDetSCTRawDataProvider","InDetTRTRawDataProvider",
                              "InDetPixelClusterization","InDetSCT_Clusterization"]:
            print '   ** Setting TrigRoiDescriptorCollection to', SuperRoIBuilder.SuperRoIOutputKey
            viewAlg.RoIs = SuperRoIBuilder.SuperRoIOutputKey

            
    bJetEtSequence += eventAlgs
    bJetEtSequence += viewAlgs

    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlg
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromName
    hypo = TrigBjetEtHypoAlg("TrigBjetEtHypoAlg")
    hypo.OutputLevel = DEBUG
    hypo.Jets = sequenceOut
    hypo.OutputJets = "SplitJets"
    hypo.TrackParticles = "TrigFastTrackFinder_Tracks"

    # Sequence     
    BjetAthSequence = seqAND("BjetAthSequence",[InputMakerAlg,recoSequence,bJetEtSequence])

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

def bJetSequence():
    # menu components   
    from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence

    # input maker
    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
    InputMakerAlg = HLTTest__TestInputMaker("BJetInputMaker_step3")
    InputMakerAlg.OutputLevel = DEBUG
#    InputMakerAlg.LinkName = "initialRoI"
#    InputMakerAlg.Output = 'SplitJet' # not sure about this

    from TrigBjetHypo.TrigBtagFexMTConfig import getBtagFexSplitInstance
    bTagAlgo = getBtagFexSplitInstance("EF","2012","EFID")
    bTagAlgo.OutputLevel = DEBUG
    # Define Inputs
    bTagAlgo.JetKey = "SplitJet"
    bTagAlgo.PriVtxKey = "xPrimVx"
    bTagAlgo.PriVtxKeyBackup = "EFHistoPrmVtx"
    bTagAlgo.TrackKey = "InDetTrigTrackingxAODCnv_Bjet_IDTrig"
    # Define Outputs
    bTagAlgo.OutputBTagKey = "HLTBjetFex"
    bTagAlgo.OutputVtxKey = "HLT_BjetVertexFex"
    bTagAlgo.OutputBtagVtxKey = "HLT_BjetSecondaryVertexFex"

    # Hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetHypoAlg
    from TrigBjetHypo.TrigBjetHypoTool import TrigBjetHypoToolFromName
    hypo = TrigBjetHypoAlg("TrigBjetHypoAlg")
    hypo.OutputLevel = DEBUG
#    hypo.RoIsKey = sequenceOut

    # Sequence
    BjetAthSequence = seqAND("BjetAthSequence_step3",[InputMakerAlg,bTagAlgo])

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetHypoToolFromName )

