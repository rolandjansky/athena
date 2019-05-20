def setupTrackCaloAssoc(sequence,ToolSvc,caloClusterName="CaloCalTopoClusters",trackParticleName="InDetTrackParticles"):
    ###################################

    #Configure the extrapolator
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    theAtlasExtrapolator=AtlasExtrapolator("AtlasExtrapolator")
    ToolSvc += theAtlasExtrapolator

    from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import ParticleToCaloExtrapolationTool
    particleToCaloExtrapolationTool = ParticleToCaloExtrapolationTool(name = "ParticleToCaloExtrapolationTool", 
                                                                      Extrapolator = theAtlasExtrapolator,
                                                                      ParticleType = "pion" )
    ToolSvc += particleToCaloExtrapolationTool
    print particleToCaloExtrapolationTool

    from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
    caloClustersInCone = xAOD__CaloClustersInConeTool(name = "CaloClustersInCone", 
                                                      CaloClusterLocation = caloClusterName)
    ToolSvc += caloClustersInCone
    print caloClustersInCone

    from TrackToCalo.TrackToCaloConf import Rec__ParticleCaloClusterAssociationTool
    particleCaloClusterAssociation = Rec__ParticleCaloClusterAssociationTool("ParticleCaloClusterAssociationInDet",
                                                                             ParticleCaloExtensionTool    = particleToCaloExtrapolationTool,
                                                                             CaloClusterLocation          = caloClusterName,
                                                                             ClustersInConeTool           = caloClustersInCone,
                                                                             ConeSize                     = 0.1,
                                                                             UseCovariance                = True )
    ToolSvc+=particleCaloClusterAssociation
    print      particleCaloClusterAssociation

    from TrackParticleAssociationAlgs.TrackParticleAssociationAlgsConf import TrackParticleClusterAssociationAlg
    trackParticleClusterAssociation = TrackParticleClusterAssociationAlg("TrackParticleClusterAssociationInDet",
                                                                         ParticleCaloClusterAssociationTool = particleCaloClusterAssociation,
                                                                         TrackParticleContainerName = trackParticleName,
                                                                         PtCut = 400.,
                                                                         OutputCollectionPostFix = "TCC",
                                                                         CaloClusterLocation = caloClusterName)
    sequence += trackParticleClusterAssociation
    print trackParticleClusterAssociation

    if not hasattr(ToolSvc, "LooseTrackVertexAssociationTool"):
        from AthenaCommon import CfgMgr
        loosetrackvertexassotool=CfgMgr.CP__LooseTrackVertexAssociationTool("LooseTrackVertexAssociationTool", dzSinTheta_cut=3, d0_cut=2) 
        ToolSvc+=loosetrackvertexassotool 

    
def runTCCReconstruction(sequence,ToolSvc,caloClusterName="CaloCalTopoClusters",trackParticleName="InDetTrackParticles"):

    

    from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import TCCCombinedTool, TCCChargedTool, TCCNeutralTool

    
    ###################################
    # Schedule the TrackCaloClusterInfoAlg to create the weights for clusters/tracks and store them in a TrackCaloClusterInfo object.
    from TrackCaloClusterRecAlgs.TrackCaloClusterRecAlgsConf import TrackCaloClusterAlg, TrackCaloClusterInfoAlg
    tccInfoAlg = TrackCaloClusterInfoAlg("TCCInfoAlg",
                                         TCCInfoName = "TCCInfo",
                                         InputTrackCaloAssoc = trackParticleName+"ClusterAssociationsTCC",
                                         InputTracks = trackParticleName,
                                         InputClusters = caloClusterName,
                                         VertexContainer = "PrimaryVertices",
    )

    sequence += tccInfoAlg

    ###################################
    # Create the TCC creator alg. TrackCaloClusterAlg makes use of the TrackCaloClusterInfo object
    # and a list of tools to build the various TCC types.
    tccCombined = TCCCombinedTool("TCCcombined",
                                  LooseTrackVertexAssoTool = ToolSvc.LooseTrackVertexAssociationTool,
                                  SaveDetectorEta           = True,
    )
    tccCharged = TCCChargedTool("TCCCharged" )
    tccNeutral = TCCNeutralTool("TCCNeutral",
                                ApplyClusterFilter        = False,
    )

    tccAlg = TrackCaloClusterAlg(name = "TrackCaloClusterAlg",
                                 OutputTCCName           = "TrackCaloClusters",
                                 TCCInfo = "TCCInfo",
                                 TCCTools = [tccCombined,  tccNeutral, ] #tccCharged,]
                                 )
    sequence += tccAlg

    print tccAlg    
    return tccAlg


def runUFOReconstruction(sequence,ToolSvc, PFOPrefix="CSSK", caloClusterName="CaloCalTopoClusters", trackParticleName="InDetTrackParticles"):

    from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import UFOTool

    from TrackCaloClusterRecAlgs.TrackCaloClusterRecAlgsConf import TrackCaloClusterAlg, TrackCaloClusterInfoUFOAlg
    tccInfoAlg = TrackCaloClusterInfoUFOAlg("UFOInfoAlg_"+PFOPrefix,
                                            TCCInfoName = PFOPrefix+"UFOInfo",
                                            InputTrackCaloAssoc = trackParticleName+"ClusterAssociationsTCC",
                                            InputTracks = trackParticleName,
                                            InputClusters = caloClusterName,
                                            VertexContainer = "PrimaryVertices",
                                            LooseTrackVertexAssoTool = ToolSvc.LooseTrackVertexAssociationTool,
                                            PFOPrefix = PFOPrefix,
                                            ClusterECut = 0.,
    )

    sequence += tccInfoAlg


    tccUFO = UFOTool("UFOtool",
                     LooseTrackVertexAssoTool = ToolSvc.LooseTrackVertexAssociationTool,
                     PFOPrefix = PFOPrefix,
                     ClusterECut = tccInfoAlg.ClusterECut,                     
    )

    tccAlg = TrackCaloClusterAlg(name = "TrackCaloClusterAlgUFO"+PFOPrefix,
                                 OutputTCCName = PFOPrefix+"UFO",
                                 TCCInfo = tccInfoAlg.TCCInfoName,
                                 TCCTools = [tccUFO,]
    )
    sequence += tccAlg
    return tccAlg

