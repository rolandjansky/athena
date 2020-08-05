from AthenaCommon import Logging
ufolog = Logging.logging.getLogger('TCCorUFO')

def associateAllTracks(trkClustAssocAlg):
    """ Make sure the given TrackParticleClusterAssociationAlg trkClustAssocAlg is scheduled to associate all tracks by removing the vtx selection tool"""
    trkClustAssocAlg.TrackVertexAssoTool = ""
    trkClustAssocAlg.VertexContainerName = ""
    
def setupTrackCaloAssoc(sequence,ToolSvc,caloClusterName="CaloCalTopoClusters",trackParticleName="InDetTrackParticles", assocPostfix = "TCC", onlyPV0Tracks=False):
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
                                                                             UseCovariance                = True)
    ToolSvc+=particleCaloClusterAssociation
    print      particleCaloClusterAssociation


    if not hasattr(ToolSvc, "LooseTrackVertexAssociationTool"):
        from AthenaCommon import CfgMgr
        loosetrackvertexassotool=CfgMgr.CP__LooseTrackVertexAssociationTool("LooseTrackVertexAssociationTool", dzSinTheta_cut=3, d0_cut=2) 
        ToolSvc+=loosetrackvertexassotool 
    
    from TrackParticleAssociationAlgs.TrackParticleAssociationAlgsConf import TrackParticleClusterAssociationAlg
    trackParticleClusterAssociation = TrackParticleClusterAssociationAlg("TrackClusterAssociationAlg"+assocPostfix,
                                                                         ParticleCaloClusterAssociationTool = particleCaloClusterAssociation,
                                                                         TrackParticleContainerName = trackParticleName,
                                                                         PtCut = 400.,
                                                                         OutputCollectionPostFix = assocPostfix,
                                                                         CaloClusterLocation = caloClusterName,
                                                                         TrackVertexAssoTool=ToolSvc.LooseTrackVertexAssociationTool, # will associate trks from PV0 only
                                                                         VertexContainerName ="PrimaryVertices",)
    if not onlyPV0Tracks:
        associateAllTracks( trackParticleClusterAssociation) # this removes the vtx selection tool.see above.

    sequence += trackParticleClusterAssociation
    print trackParticleClusterAssociation


    
def runTCCReconstruction(sequence,ToolSvc,caloClusterName="CaloCalTopoClusters",trackParticleName="InDetTrackParticles",
                         assocPostfix="TCC", doCombined=True, doNeutral=True, doCharged=False, outputTCCName="TrackCaloClusters"):
    """Create a TrackCaloCluster collection from clusters and tracks (caloClusterName and trackParticleName). 
    Depending on options, the collection contains combined, neutral and/or charged TCC.
    This functions schedules 2 algs : 
       * a TrackCaloClusterInfoAlg to build the TrackCaloClusterInfo object
       * a TrackCaloClusterAlg to build the TCC
    If no TrackClusterAssociationAlg is found in the sequence, setupTrackCaloAssoc() is called to create the needed association.
    """

    sequence = getTCCConstitSeq(sequence) # will insert in a dedicated sequence setup just before the original sequence
    if hasattr(sequence, "TrackCaloClusterAlg"):
        return getattr(sequence, "TrackCaloClusterAlg")


    from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import TCCCombinedTool, TCCChargedTool, TCCNeutralTool

    if not hasattr(sequence, "TrackClusterAssociationAlg"+assocPostfix):
        # make sure we run the TrackClusterAssociationAlg
        setupTrackCaloAssoc(sequence, ToolSvc, caloClusterName, trackParticleName, assocPostfix, onlyPV0Tracks=False)
    else: # make sure we use the same CaloCluster container as the TrackClusterAssociationAlg
        alg = getattr(sequence, "TrackClusterAssociationAlg"+assocPostfix)
        if alg.CaloClusterLocation != caloClusterName:
            ufolog.error("Requesting TCC build of "+caloClusterName+" with TrackCaloClusterAlg configured with "+alg.CaloClusterLocation)
            raise Exception("TCC config error")
        # make sure the TrackClusterAssociationAlg is configured to use *ALL* tracks.
        associateAllTracks(alg)
        
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

    sequence = getTCCConstitSeq(sequence) # will insert in a dedicated sequence setup just before the original sequence
    if hasattr(sequence, "TrackCaloClusterAlgUFO"+PFOPrefix):
        return getattr(sequence, "TrackCaloClusterAlgUFO"+PFOPrefix)

    if not hasattr(sequence, "TrackClusterAssociationAlg"+assocPostfix):
        setupTrackCaloAssoc(sequence, ToolSvc, caloClusterName, trackParticleName, assocPostfix, onlyPV0Tracks=True)
    else: # make sure we use the same CaloCluster container as the TrackClusterAssociationAlg
        alg = getattr(sequence, "TrackClusterAssociationAlg"+assocPostfix)
        if alg.CaloClusterLocation != caloClusterName:
            ufolog.error("Requesting UFO build of ",caloClusterName, " with TrackCaloClusterAlg configured with ",alg.CaloClusterLocation)
            raise Exception("UFO config error")
        
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
                                 OutputTCCName = "UFO"+PFOPrefix,
                                 TCCInfo = tccInfoAlg.TCCInfoName,
                                 TCCTools = [tccUFO,]
    )
    sequence += tccAlg
    return tccAlg

def findParentSeq(sequence):
    from AthenaCommon.AlgSequence import AlgSequence, iter_algseq
    topSequence = AlgSequence()
    if topSequence is sequence:
        return sequence

    for s in iter_algseq(topSequence):
        if sequence in s:
            return s
    
    # if not found, sequence is not yet part of main sequence
    return topSequence

def getTCCConstitSeq(sequence):
    parent = findParentSeq(sequence)

    jetConstSeq = getattr(parent, "JetTCCConstitSeq", None)

    if jetConstSeq != None:
        return jetConstSeq

    from AthenaCommon.AlgSequence import AlgSequence
    jetConstSeq = AlgSequence("JetTCCConstitSeq")

    if parent is sequence:
        sequence += jetConstSeq
    else:
        try:
            i=parent.getSequence().index(sequence.getFullJobOptName())
            parent.insert(i,jetConstSeq)
        except ValueError:
             parent += jetConstSeq
    return jetConstSeq
