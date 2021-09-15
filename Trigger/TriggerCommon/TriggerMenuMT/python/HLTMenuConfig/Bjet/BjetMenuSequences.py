#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

# menu components
from AthenaCommon.CFElements import seqAND
from ..Menu.MenuComponents import MenuSequence
from TrigEDMConfig.TriggerEDMRun3 import recordable

#from AthenaCommon.Constants import DEBUG

# ====================================================================================================
#    Get MenuSequences
# ====================================================================================================


# ====================================================================================================
#    step 1: This is Jet code. Not here!
# ====================================================================================================

# ====================================================================================================
#    step 2: Second stage of fast tracking, Precision tracking, and flavour tagging
# ====================================================================================================

# todo: pass in more information, i.e. jet collection name
def getBJetSequence(jc_name):

    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig

    config=getInDetTrigConfig('jet')
    prmVtxKey = config.vertex
    outputRoIName = "HLT_Roi_Bjet"

    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
    from DecisionHandling.DecisionHandlingConf import ViewCreatorCentredOnJetWithPVConstraintROITool
    InputMakerAlg = EventViewCreatorAlgorithm( "IMBJet_step2" )
    #
    newRoITool = ViewCreatorCentredOnJetWithPVConstraintROITool()
    newRoITool.RoisWriteHandleKey  = recordable( outputRoIName )
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

    # Output container names as defined in TriggerEDMRun3
    if not jc_name:
        raise ValueError("jet collection name is empty - pass the full HLT jet collection name to getBJetSequence().")
    jc_key = f'{jc_name}_'
    InputMakerAlg.InViewJets = recordable( f'{jc_key}bJets' )
    BTagName = recordable(f'{jc_key}BTagging')

    # Prepare data objects for view verifier
    viewDataObjects = [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % InputMakerAlg.InViewRoIs ),
                       ( 'xAOD::VertexContainer' , 'StoreGateSvc+%s' % prmVtxKey ),
                       ( 'xAOD::JetContainer' , 'StoreGateSvc+%s' % InputMakerAlg.InViewJets )]

    # Second stage of Fast Tracking and Precision Tracking
    from TriggerMenuMT.HLTMenuConfig.Bjet.BjetTrackingConfiguration import getSecondStageBjetTracking
    secondStageAlgs, PTTrackParticles = getSecondStageBjetTracking( inputRoI=InputMakerAlg.InViewRoIs, dataObjects=viewDataObjects )

    from AthenaCommon.Configurable import ConfigurableRun3Behavior
    with ConfigurableRun3Behavior():
        # Flavour Tagging
        from TriggerMenuMT.HLTMenuConfig.Bjet.BjetFlavourTaggingConfiguration import getFlavourTagging
        acc_flavourTaggingAlgs,bTaggingContainerName = getFlavourTagging(
            inputJets=str(InputMakerAlg.InViewJets),
            inputVertex=prmVtxKey,
            inputTracks=PTTrackParticles[0],
            BTagName=BTagName,
            inputMuons=None
        )

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

    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetBtagHypoAlg
    hypo = TrigBjetBtagHypoAlg( "TrigBjetBtagHypoAlg" )
    # keys
    hypo.BTaggedJetKey = InputMakerAlg.InViewJets
    hypo.BTaggingKey = bTaggingContainerName
    hypo.TracksKey = PTTrackParticles[0]
    hypo.PrmVtxKey = newRoITool.VertexReadHandleKey
    # links for navigation
    hypo.BTaggingLink = bTaggingContainerName.replace( "HLT_","" )
    hypo.PrmVtxLink = newRoITool.PrmVtxLink

    from TrigBjetHypo.TrigBjetMonitoringConfig import TrigBjetOnlineMonitoring
    hypo.MonTool = TrigBjetOnlineMonitoring()

    from TrigBjetHypo.TrigBjetBtagHypoTool import TrigBjetBtagHypoToolFromDict
    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetBtagHypoToolFromDict)
