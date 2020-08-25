#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

# menu components
from AthenaCommon.CFElements import seqAND
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from TrigEDMConfig.TriggerEDMRun3 import recordable

#from AthenaCommon.Constants import DEBUG

# ====================================================================================================  
#    Get MenuSequences
# ==================================================================================================== 


def getBJetSequence():
    return bJetStep2Sequence()

# ==================================================================================================== 
#    step 1: This is Jet code. Not here!
# ==================================================================================================== 

# ==================================================================================================== 
#    step 2: Second stage of fast tracking, Precision tracking, and flavour tagging
# ====================================================================================================  

def bJetStep2Sequence():
    prmVtxKey = "HLT_IDVertex_FS"
    outputRoIName = "HLT_Roi_Bjet"

    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
    from DecisionHandling.DecisionHandlingConf import ViewCreatorCentredOnJetWithPVConstraintROITool
    InputMakerAlg = EventViewCreatorAlgorithm( "IMBJet_step2" )
    #
    newRoITool = ViewCreatorCentredOnJetWithPVConstraintROITool()
    newRoITool.RoisWriteHandleKey = recordable( outputRoIName )
    newRoITool.VertexReadHandleKey = prmVtxKey
    newRoITool.PrmVtxLink = prmVtxKey.replace( "HLT_","" )
    #
    InputMakerAlg.mergeUsingFeature = True 
    InputMakerAlg.RoITool = newRoITool
    #
    InputMakerAlg.Views = "BTagViews"
    InputMakerAlg.InViewRoIs = "InViewRoIs"
    #
    InputMakerAlg.RequireParentView = False
    InputMakerAlg.ViewFallThrough = True
    # BJet specific
    InputMakerAlg.PlaceJetInView = True
    InputMakerAlg.InViewJets = recordable( "HLT_bJets" )


    # Prepare data objects for view verifier
    viewDataObjects = [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+' + InputMakerAlg.InViewRoIs ),
                       ( 'xAOD::VertexContainer' , 'StoreGateSvc+' + prmVtxKey ),
                       ( 'xAOD::JetContainer' , 'StoreGateSvc+' + InputMakerAlg.InViewJets )]

    # Second stage of Fast Tracking and Precision Tracking
    from TriggerMenuMT.HLTMenuConfig.Bjet.BjetTrackingConfiguration import getSecondStageBjetTracking
    secondStageAlgs, PTTrackParticles = getSecondStageBjetTracking( inputRoI=InputMakerAlg.InViewRoIs, dataObjects=viewDataObjects )

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    # Flavour Tagging
    from TriggerMenuMT.HLTMenuConfig.Bjet.BjetFlavourTaggingConfiguration import getFlavourTagging
    acc_flavourTaggingAlgs,bTaggingContainerName = getFlavourTagging( inputJets=InputMakerAlg.InViewJets, inputVertex=prmVtxKey, inputTracks=PTTrackParticles[0] )
    
    Configurable.configurableRun3Behavior=0

    #Conversion of flavour-tagging algorithms from new to old-style
    from AthenaCommon.CFElements import findAllAlgorithms
    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
    AllFlavourTaggingAlgs = []
    for alg in findAllAlgorithms(acc_flavourTaggingAlgs.getSequence("AthAlgSeq")):
        AllFlavourTaggingAlgs.append(conf2toConfigurable(alg))

    acc_flavourTaggingAlgs.wasMerged() #Needed to remove error message; Next we add all algorithms to sequence so this is kind of an old-style merge
    bJetBtagSequence = seqAND( "bJetBtagSequence", secondStageAlgs + AllFlavourTaggingAlgs )
    InputMakerAlg.ViewNodeName = "bJetBtagSequence"
    
    # Sequence
    BjetAthSequence = seqAND( "BjetAthSequence_step2",[InputMakerAlg,bJetBtagSequence] )

    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetBtagHypoAlgMT
    hypo = TrigBjetBtagHypoAlgMT( "TrigBjetBtagHypoAlg" )
    # keys
    hypo.BTaggedJetKey = InputMakerAlg.InViewJets
    hypo.BTaggingKey = bTaggingContainerName
    hypo.TracksKey = PTTrackParticles[0]
    hypo.PrmVtxKey = newRoITool.VertexReadHandleKey
    # links for navigation
    hypo.BTaggingLink = bTaggingContainerName.replace( "HLT_","" )
    hypo.PrmVtxLink = newRoITool.PrmVtxLink

    from TrigBjetHypo.TrigBjetOnlineMonitoringMTConfig import TrigBjetOnlineMonitoring
    hypo.MonTool = TrigBjetOnlineMonitoring()

    from TrigBjetHypo.TrigBjetBtagHypoTool import TrigBjetBtagHypoToolFromDict
    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetBtagHypoToolFromDict)




 
