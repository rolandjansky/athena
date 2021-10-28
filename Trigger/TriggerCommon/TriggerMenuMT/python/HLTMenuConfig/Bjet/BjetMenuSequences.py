#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

# menu components
from AthenaCommon.CFElements import seqAND, findAllAlgorithms
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable, appendCAtoAthena
from AthenaCommon.Configurable import ConfigurableRun3Behavior
from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
from TrigEDMConfig.TriggerEDMRun3 import recordable

from ..Menu.MenuComponents import MenuSequence
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

    config=getInDetTrigConfig('jet')
    prmVtxKey = config.vertex
    outputRoIName = "HLT_Roi_Bjet"

    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
    from DecisionHandling.DecisionHandlingConf import ViewCreatorCentredOnJetWithPVConstraintROITool
    InputMakerAlg = EventViewCreatorAlgorithm( f"IMBJet_{jc_name}_step2" )
    #
    newRoITool = ViewCreatorCentredOnJetWithPVConstraintROITool()
    newRoITool.RoisWriteHandleKey  = recordable( outputRoIName )
    newRoITool.VertexReadHandleKey = prmVtxKey
    newRoITool.PrmVtxLink = prmVtxKey.replace( "HLT_","" )
    #
    InputMakerAlg.mergeUsingFeature = True
    InputMakerAlg.RoITool = newRoITool
    #
    InputMakerAlg.Views = f"BTagViews_{jc_name}"
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

    # Conversion of flavour-tagging algorithms from new to old-style
    # 1) We need to do the algorithms manually and then remove them from the CA
    #
    # Please see the discussion on
    # https://gitlab.cern.ch/atlas/athena/-/merge_requests/46951#note_4854474
    # and the description in that merge request.
    flavourTaggingAlgs = [conf2toConfigurable(alg)
                          for alg in findAllAlgorithms(acc_flavourTaggingAlgs._sequence)]
    bJetBtagSequence = seqAND( f"bJetBtagSequence_{jc_name}", secondStageAlgs + flavourTaggingAlgs )

    # you can't use accumulator.wasMerged() here because the above
    # code only merged the algorithms. Instead we rely on this hacky
    # looking construct.
    acc_flavourTaggingAlgs._sequence = []

    # 2) the rest is done by the generic helper
    # this part is needed to accomodate parts of flavor tagging that
    # aren't algorithms, e.g. JetTagCalibration.
    appendCAtoAthena(acc_flavourTaggingAlgs)

    InputMakerAlg.ViewNodeName = f"bJetBtagSequence_{jc_name}"

    # Sequence
    BjetAthSequence = seqAND( f"BjetAthSequence_{jc_name}_step2",[InputMakerAlg,bJetBtagSequence] )

    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetBtagHypoAlg
    hypo = TrigBjetBtagHypoAlg( f"TrigBjetBtagHypoAlg_{jc_name}" )
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
