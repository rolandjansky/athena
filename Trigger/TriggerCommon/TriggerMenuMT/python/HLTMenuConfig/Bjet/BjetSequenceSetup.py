#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

# menu components
from AthenaCommon.CFElements import seqAND
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, InViewReco
from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaConfiguration.ComponentFactory import CompFactory

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
    prmVtxKey = "HLT_EFHistoPrmVtx"
    outputRoIName = "HLT_Roi_Bjet"

    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
    from ViewAlgs.ViewAlgsConf import ViewCreatorCentredOnJetWithPVConstraintROITool
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
    #
    InputMakerAlg.ViewNodeName = "bJetBtagSequence"


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
    
#======================This is running on Release 22.0.11, but not after Gaudi2 updates===================
    inViewReco = InViewReco("bJetBtagSequence", viewMaker=InputMakerAlg)
    inViewReco.addRecoAlg(secondStageAlgs)
    inViewReco.mergeReco(acc_flavourTaggingAlgs)
    acc_flavourTaggingAlgs.wasMerged()
    inViewReco.wasMerged()

    Configurable.configurableRun3Behavior=0

#======================This is a new idea, since the function "conf2toConfigurable" was added recently, but is crashing when converting ATLASExtrapolator=====
#    Configurable.configurableRun3Behavior=0

#    from AthenaCommon.CFElements import findAllAlgorithms
#    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
#    AllFlavourTaggingAlgs = []
#    for alg in findAllAlgorithms(acc_flavourTaggingAlgs.getSequence("AthAlgSeq")):
#        AllFlavourTaggingAlgs.append(conf2toConfigurable(alg))

#    bJetBtagSequence = seqAND( "bJetBtagSequence", secondStageAlgs + AllFlavourTaggingAlgs )
#    InputMakerAlg.ViewNodeName = "bJetBtagSequence"

#    # Sequence
#    BjetAthSequence = seqAND( "BjetAthSequence_step2",[InputMakerAlg,bJetBtagSequence] )
#=============================================================================================================

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
    return MenuSequence( Sequence    = inViewReco.sequence(),
                         Maker       = inViewReco.inputMaker(),
#    return MenuSequence( Sequence    = BjetAthSequence,
#                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetBtagHypoToolFromDict)




 
