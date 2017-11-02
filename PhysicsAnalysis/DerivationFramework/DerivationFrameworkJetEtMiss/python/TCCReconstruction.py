def runTCCReconstruction(sequence,ToolSvc,CaloClusterName="CaloCalTopoClusters",TrackParticleName="InDetTrackParticles"):
    #Configure the extrapolator
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    theAtlasExtrapolator=AtlasExtrapolator("AtlasExtrapolator")
    ToolSvc += theAtlasExtrapolator

    from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import ParticleToCaloExtrapolationTool
    ParticleToCaloExtrapolationTool = ParticleToCaloExtrapolationTool(name = "ParticleToCaloExtrapolationTool", 
                                                                      Extrapolator = theAtlasExtrapolator,
                                                                      ParticleType = "pion" )
    ToolSvc += ParticleToCaloExtrapolationTool
    print ParticleToCaloExtrapolationTool

    from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
    CaloClustersInCone = xAOD__CaloClustersInConeTool(name = "CaloClustersInCone", 
                                                      CaloClusterLocation = CaloClusterName)
    ToolSvc += CaloClustersInCone
    print CaloClustersInCone

    from TrackToCalo.TrackToCaloConf import Rec__ParticleCaloClusterAssociationTool
    ParticleCaloClusterAssociation = Rec__ParticleCaloClusterAssociationTool(name                      = "ParticleCaloClusterAssociationInDet",
                                                                       ParticleCaloExtensionTool    = ParticleToCaloExtrapolationTool,
                                                                       CaloClusterLocation          = CaloClusterName,
                                                                       ClustersInConeTool           = CaloClustersInCone,
                                                                       ConeSize                     = 0.1,
                                                                       UseCovariance                = True )
    ToolSvc+=ParticleCaloClusterAssociation
    print      ParticleCaloClusterAssociation

    from TrackParticleAssociationAlgs.TrackParticleAssociationAlgsConf import TrackParticleClusterAssociationAlg
    TrackParticleClusterAssociation = TrackParticleClusterAssociationAlg(name = "TrackParticleClusterAssociationInDet",
                                                                ParticleCaloClusterAssociationTool = ParticleCaloClusterAssociation,
                                                                TrackParticleContainerName = TrackParticleName,
                                                                PtCut = 400.,
                                                                OutputCollectionPostFix = "TCC",
                                                                CaloClusterLocation = CaloClusterName)
    sequence += TrackParticleClusterAssociation
    print TrackParticleClusterAssociation

    from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import TrackCaloClusterWeightsTool
    TrackCaloClusterWeights = TrackCaloClusterWeightsTool(name                 = "TrackCaloClusterWeights"  )
    ToolSvc+=TrackCaloClusterWeights
    print      TrackCaloClusterWeights

    from AthenaCommon import CfgMgr
    loosetrackvertexassotool=CfgMgr.CP__LooseTrackVertexAssociationTool("LooseTrackVertexAssociationTool", dzSinTheta_cut=3, d0_cut=2) 
    ToolSvc+=loosetrackvertexassotool 

    from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import TrackCaloClusterCreatorTool
    TrackCaloClusterCreator = TrackCaloClusterCreatorTool(name                      = "TrackCaloClusterCreator",
                                                          VertexContainerName       = "PrimaryVertices",
                                                          LooseTrackVertexAssoTool  = loosetrackvertexassotool,
                                                          ApplyClusterFilter        = False,
                                                          SaveDetectorEta           = True)
    ToolSvc+=TrackCaloClusterCreator
    print      TrackCaloClusterCreator

    from TrackCaloClusterRecAlgs.TrackCaloClusterRecAlgsConf import TrackCaloClusterRecAlg
    TrackCaloClusterReconstruction = TrackCaloClusterRecAlg(name                           = "TrackCaloClusterRecAlg",
                                                         CaloClustersLocation           = CaloClusterName,
                                                         TrackCaloClusterContainerName  = "TrackCaloClusters",
                                                         OutputCollectionPostFix        = "TCC",
                                                         TrackCaloClusterWeightsTool    = TrackCaloClusterWeights,
                                                         TrackCaloClusterCreatorTool    = TrackCaloClusterCreator)
    sequence += TrackCaloClusterReconstruction
    print TrackCaloClusterReconstruction