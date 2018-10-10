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

    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlg
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromName
    hypo = TrigBjetEtHypoAlg("TrigBjetEtHypoAlg")
    hypo.OutputLevel = DEBUG
    hypo.Jets = sequenceOut
    hypo.OutputJets = "SplitJets"

    # Sequence     
    BjetAthSequence = seqAND("BjetAthSequence",[InputMakerAlg,recoSequence])

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

