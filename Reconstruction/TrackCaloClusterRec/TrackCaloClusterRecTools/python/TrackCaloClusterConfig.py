def setupTrackCaloAssoc(sequence,ToolSvc,caloClusterName="CaloCalTopoClusters",trackParticleName="InDetTrackParticles", assocPostfix = "TCC"):
    """ Schedule a TrackParticleClusterAssociationAlg in the top sequence, taking as input clusters and tracks defined 
    by the keys caloClusterName and trackParticleName.

    The output map is set in the evt store under the key : trackParticleName+"ClusterAssociations"+assocPostfix
    """
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
    trackParticleClusterAssociation = TrackParticleClusterAssociationAlg("TrackClusterAssociationAlg"+assocPostfix,
                                                                         ParticleCaloClusterAssociationTool = particleCaloClusterAssociation,
                                                                         TrackParticleContainerName = trackParticleName,
                                                                         PtCut = 400.,
                                                                         OutputCollectionPostFix = assocPostfix,
                                                                         CaloClusterLocation = caloClusterName)
    sequence += trackParticleClusterAssociation
    print trackParticleClusterAssociation

    if not hasattr(ToolSvc, "LooseTrackVertexAssociationTool"):
        from AthenaCommon import CfgMgr
        loosetrackvertexassotool=CfgMgr.CP__LooseTrackVertexAssociationTool("LooseTrackVertexAssociationTool", dzSinTheta_cut=3, d0_cut=2) 
        ToolSvc+=loosetrackvertexassotool 

    
def runTCCReconstruction(sequence,ToolSvc,caloClusterName="CaloCalTopoClusters",trackParticleName="InDetTrackParticles",
                         assocPostfix="TCC", doCombined=True, doNeutral=True, doCharged=False, outputTCCName="TrackCaloClusters"):
    """Create a TrackCaloCluster collection from clusters and tracks (caloClusterName and trackParticleName). 
    Depending on options, the collection contains combined, neutral and/or charged TCC.
    This functions schedules 2 algs : 
       * a TrackCaloClusterInfoAlg to build the TrackCaloClusterInfo object
       * a TrackCaloClusterAlg to build the TCC
    If no TrackClusterAssociationAlg is found in the sequence, setupTrackCaloAssoc() is called to create the needed association.
    """

    

    from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import TCCCombinedTool, TCCChargedTool, TCCNeutralTool

    if not hasattr(sequence, "TrackClusterAssociationAlg"+assocPostfix):
        setupTrackCaloAssoc(sequence, ToolSvc, caloClusterName, trackParticleName, assocPostfix)
    
    ###################################
    # Schedule the TrackCaloClusterInfoAlg to create the weights for clusters/tracks and store them in a TrackCaloClusterInfo object.
    from TrackCaloClusterRecAlgs.TrackCaloClusterRecAlgsConf import TrackCaloClusterAlg, TrackCaloClusterInfoAlg
    tccInfoAlg = TrackCaloClusterInfoAlg("TCCInfoAlg",
                                         TCCInfoName = "TCCInfo",
                                         InputTrackCaloAssoc = trackParticleName+"ClusterAssociations"+assocPostfix,
                                         InputTracks = trackParticleName,
                                         InputClusters = caloClusterName,
                                         VertexContainer = "PrimaryVertices",
    )

    sequence += tccInfoAlg

    ###################################
    # Create the TCC creator alg. TrackCaloClusterAlg makes use of the TrackCaloClusterInfo object
    # and a list of tools to build the various TCC types.
    tccTools = []
    if doCombined:
        tccCombined = TCCCombinedTool("TCCcombined",
                                      LooseTrackVertexAssoTool = ToolSvc.LooseTrackVertexAssociationTool,
                                      SaveDetectorEta           = True,)
        tccTools.append(tccCombined)
    if doCharged:
        tccCharged = TCCChargedTool("TCCCharged" )
        tccTools.append(tccCharged)
    if doNeutral:
        tccNeutral = TCCNeutralTool("TCCNeutral",
                                    ApplyClusterFilter        = False,)
        tccTools.append(tccNeutral)

    tccAlg = TrackCaloClusterAlg(name = "TrackCaloClusterAlg",
                                 OutputTCCName           = outputTCCName,
                                 TCCInfo = "TCCInfo",
                                 TCCTools = tccTools,
                                 )
    sequence += tccAlg

    return tccAlg


def runUFOReconstruction(sequence,ToolSvc, PFOPrefix="CSSK", caloClusterName="CaloCalTopoClusters", trackParticleName="InDetTrackParticles",
                         assocPostfix="TCC", ):
    """Create a TrackCaloCluster collection from PFlow and tracks (PFO retrieved from PFOPrefix and tracks directly from trackParticleName). 
    This functions schedules 2 algs : 
       * a TrackCaloClusterInfoUFOAlg to build the TrackCaloClusterInfo object
       * a TrackCaloClusterAlg to build the UFO
    If no TrackClusterAssociationAlg is found in the sequence, setupTrackCaloAssoc() is called to create the needed association.
    """

    if not hasattr(sequence, "TrackClusterAssociationAlg"+assocPostfix):
        setupTrackCaloAssoc(sequence, ToolSvc, caloClusterName, trackParticleName, assocPostfix)
    
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

