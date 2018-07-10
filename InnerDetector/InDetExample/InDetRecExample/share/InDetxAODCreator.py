from InDetRecExample.InDetJobProperties import InDetFlags
from InDetRecExample.InDetKeys import InDetKeys

doCreation = ( InDetFlags.doNewTracking() or InDetFlags.doPseudoTracking() or InDetFlags.doLargeD0() or InDetFlags.doLowPtLargeD0() ) \
                    and InDetFlags.doParticleCreation()
doConversion = not InDetFlags.doNewTracking()  and not InDetFlags.doPseudoTracking() and not InDetFlags.doLargeD0() \
                    and not InDetFlags.doLowPtLargeD0() and InDetFlags.doParticleConversion()

if doCreation:
    print "Creating xAOD::TrackParticles from Trk::Tracks"
if doConversion:
    print "Converting Rec::TrackParticles to xAOD::TrackParticles"

_perigee_expression=InDetFlags.perigeeExpression()
# need to tread Vertex specifically because at the time of
# the track particle creation the primary vertex does not yet exist.
# The problem is solved by first creating track particles wrt. the beam line
# and correcting the parameters after the vertex finding.
if _perigee_expression == 'Vertex' :
    _perigee_expression = 'BeamLine'
    

from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
InDetxAODParticleCreatorTool = Trk__TrackParticleCreatorTool(name = "InDetxAODParticleCreatorTool", 
                                                             Extrapolator            = InDetExtrapolator,
                                                             TrackSummaryTool        = InDetTrackSummaryToolSharedHits,
                                                             BadClusterID            = InDetFlags.pixelClusterBadClusterID(),
                                                             ForceTrackSummaryUpdate = False,
                                                             KeepParameters          = True,
                                                             PerigeeExpression       = _perigee_expression)


ToolSvc += InDetxAODParticleCreatorTool 
if InDetFlags.doPrintConfigurables(): 
    print InDetxAODParticleCreatorTool 

if (doCreation or doConversion):# or InDetFlags.useExistingTracksAsInput()) : <---- [XXX JDC Should we included this?
                                #                                                    problems appear when nothing should
                                #                                                    be done but
                                #                                                    useExistinTracksAsInput...
    # [XXX JDC: to deal with the MergedTracks case, the truth collections are
    #           defined in the InputTrackCollectionTruth variable. To be deprecated
    #           if finally there is no need of the special "MergedTrack" name
    if 'InputTrackCollectionTruth' not in dir():
        InputTrackCollectionTruth = InDetKeys.TracksTruth()
    from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg 
    if not InDetFlags.doDBMstandalone(): 
        xAODTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(InDetKeys.xAODTrackParticleContainer()) 
        xAODTrackParticleCnvAlg.xAODContainerName = InDetKeys.xAODTrackParticleContainer() 
        xAODTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = InDetKeys.xAODTrackParticleContainer() 
        xAODTrackParticleCnvAlg.TrackContainerName = InputTrackCollection 
        xAODTrackParticleCnvAlg.TrackParticleCreator = InDetxAODParticleCreatorTool 
        xAODTrackParticleCnvAlg.AODContainerName = InDetKeys.TrackParticles() 
        xAODTrackParticleCnvAlg.AODTruthContainerName = InDetKeys.TrackParticlesTruth() 
        xAODTrackParticleCnvAlg.ConvertTrackParticles = doConversion 
        xAODTrackParticleCnvAlg.ConvertTracks = doCreation 
        xAODTrackParticleCnvAlg.AddTruthLink = InDetFlags.doTruth() 
        xAODTrackParticleCnvAlg.TrackTruthContainerName = InputTrackCollectionTruth
        xAODTrackParticleCnvAlg.PrintIDSummaryInfo = True 
        topSequence += xAODTrackParticleCnvAlg 
    if InDetFlags.doDBMstandalone() or InDetFlags.doDBM(): 
        xAODDBMTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(InDetKeys.xAODDBMTrackParticleContainer()) 
        xAODDBMTrackParticleCnvAlg.xAODContainerName = InDetKeys.xAODDBMTrackParticleContainer() 
        xAODDBMTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = InDetKeys.xAODDBMTrackParticleContainer() 
        xAODDBMTrackParticleCnvAlg.TrackContainerName = InDetKeys.DBMTracks() 
        xAODDBMTrackParticleCnvAlg.TrackTruthContainerName = InDetKeys.DBMDetailedTracksTruth() 
        xAODDBMTrackParticleCnvAlg.TrackParticleCreator = InDetxAODParticleCreatorTool 
        xAODDBMTrackParticleCnvAlg.AODContainerName = InDetKeys.TrackParticles() 
        xAODDBMTrackParticleCnvAlg.AODTruthContainerName = InDetKeys.TrackParticlesTruth() 
        xAODDBMTrackParticleCnvAlg.ConvertTrackParticles = False 
        xAODDBMTrackParticleCnvAlg.ConvertTracks = True 
        xAODDBMTrackParticleCnvAlg.AddTruthLink = InDetFlags.doTruth() 
        xAODDBMTrackParticleCnvAlg.TrackTruthContainerName = InDetKeys.DBMTracksTruth() 
        xAODDBMTrackParticleCnvAlg.PrintIDSummaryInfo = True 
        #xAODDBMTrackParticleCnvAlg.OutputLevel = VERBOSE 
        topSequence += xAODDBMTrackParticleCnvAlg 

if not InDetFlags.doVertexFinding():
    if not InDetFlags.doDBMstandalone():
        from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__VertexCnvAlg
        xAODVertexCnvAlg = xAODMaker__VertexCnvAlg("VertexCnvAlg")
        xAODVertexCnvAlg.xAODContainerName = InDetKeys.xAODVertexContainer()
        xAODVertexCnvAlg.AODContainerName = InDetKeys.PrimaryVertices()
        xAODVertexCnvAlg.TPContainerName = InDetKeys.xAODTrackParticleContainer()
        topSequence += xAODVertexCnvAlg
    if InDetFlags.doDBMstandalone() or InDetFlags.doDBM():
        from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__VertexCnvAlg
        xAODVertexCnvAlgDBM = xAODMaker__VertexCnvAlg("VertexCnvAlgDBM")
        xAODVertexCnvAlgDBM.xAODContainerName = InDetKeys.xAODVertexContainer()
        xAODVertexCnvAlgDBM.AODContainerName = InDetKeys.PrimaryVertices()
        xAODVertexCnvAlgDBM.TPContainerName = InDetKeys.xAODDBMTrackParticleContainer()
        topSequence += xAODVertexCnvAlgDBM

#For forward tracks, no separate collection for ITK, since they are already merged
if (InDetFlags.doForwardTracks() and InDetFlags.doParticleCreation() and not InDetFlags.doSLHC()) or doConversion:       
 xAODForwardTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(InDetKeys.xAODForwardTrackParticleContainer())
 xAODForwardTrackParticleCnvAlg.xAODContainerName = InDetKeys.xAODForwardTrackParticleContainer()
 xAODForwardTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = InDetKeys.xAODForwardTrackParticleContainer()
 xAODForwardTrackParticleCnvAlg.AODContainerName = InDetKeys.ResolvedForwardTrackParticles()
 xAODForwardTrackParticleCnvAlg.AODTruthContainerName = InDetKeys.ResolvedForwardTrackParticlesTruth()
 xAODForwardTrackParticleCnvAlg.TrackParticleCreator = InDetxAODParticleCreatorTool
 xAODForwardTrackParticleCnvAlg.TrackContainerName = InDetKeys.ResolvedForwardTracks()
 xAODForwardTrackParticleCnvAlg.ConvertTrackParticles = doConversion
 xAODForwardTrackParticleCnvAlg.ConvertTracks = doCreation
 xAODForwardTrackParticleCnvAlg.AddTruthLink = InDetFlags.doTruth()
 xAODForwardTrackParticleCnvAlg.TrackTruthContainerName = InDetKeys.ResolvedForwardTracksTruth()
 xAODForwardTrackParticleCnvAlg.PrintIDSummaryInfo = True
 #xAODForwardTrackParticleCnvAlg.OutputLevel = VERBOSE
 topSequence += xAODForwardTrackParticleCnvAlg

if InDetFlags.doPseudoTracking():
 from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
 xAODPseudoTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(InDetKeys.xAODPseudoTrackParticleContainer())
 xAODPseudoTrackParticleCnvAlg.xAODContainerName = InDetKeys.xAODPseudoTrackParticleContainer()
 xAODPseudoTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = InDetKeys.xAODPseudoTrackParticleContainer()
 xAODPseudoTrackParticleCnvAlg.TrackContainerName = InDetKeys.PseudoTracks()
 xAODPseudoTrackParticleCnvAlg.TrackParticleCreator = InDetxAODParticleCreatorTool
 xAODPseudoTrackParticleCnvAlg.AODContainerName = InDetKeys.PseudoTracks()
 xAODPseudoTrackParticleCnvAlg.AODTruthContainerName = InDetKeys.TrackParticlesTruth()
 xAODPseudoTrackParticleCnvAlg.ConvertTrackParticles = doConversion
 xAODPseudoTrackParticleCnvAlg.ConvertTracks = doCreation
 xAODPseudoTrackParticleCnvAlg.AddTruthLink = InDetFlags.doTruth()
 xAODPseudoTrackParticleCnvAlg.TrackTruthContainerName = InDetKeys.PseudoTracksTruth()
 xAODPseudoTrackParticleCnvAlg.PrintIDSummaryInfo = True
 topSequence += xAODPseudoTrackParticleCnvAlg

 
if InDetFlags.doTrackSegmentsPixel() and InDetFlags.doParticleCreation():
 xAODPixelTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(InDetKeys.xAODPixelTrackParticleContainer())
 xAODPixelTrackParticleCnvAlg.xAODContainerName = InDetKeys.xAODPixelTrackParticleContainer()
 xAODPixelTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = InDetKeys.xAODPixelTrackParticleContainer()
 xAODPixelTrackParticleCnvAlg.TrackParticleCreator = InDetxAODParticleCreatorTool
 xAODPixelTrackParticleCnvAlg.TrackContainerName = InDetKeys.PixelTracks()
 xAODPixelTrackParticleCnvAlg.ConvertTrackParticles = False
 xAODPixelTrackParticleCnvAlg.ConvertTracks = True
 xAODPixelTrackParticleCnvAlg.TrackTruthContainerName = InDetKeys.PixelTracksTruth()
 xAODPixelTrackParticleCnvAlg.AddTruthLink = InDetFlags.doTruth()
 xAODPixelTrackParticleCnvAlg.PrintIDSummaryInfo = True
 #xAODPixelTrackParticleCnvAlg.OutputLevel = VERBOSE
 topSequence += xAODPixelTrackParticleCnvAlg

if InDetFlags.doTrackSegmentsPixelPrdAssociation() and InDetFlags.doParticleCreation():
 xAODPixelPrdAssociationTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(InDetKeys.xAODPixelPrdAssociationTrackParticleContainer())
 xAODPixelPrdAssociationTrackParticleCnvAlg.xAODContainerName = InDetKeys.xAODPixelPrdAssociationTrackParticleContainer()
 xAODPixelPrdAssociationTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = InDetKeys.xAODPixelPrdAssociationTrackParticleContainer()
 xAODPixelPrdAssociationTrackParticleCnvAlg.TrackParticleCreator = InDetxAODParticleCreatorTool
 xAODPixelPrdAssociationTrackParticleCnvAlg.TrackContainerName = InDetKeys.PixelPrdAssociationTracks()
 xAODPixelPrdAssociationTrackParticleCnvAlg.TrackTruthContainerName = InDetKeys.PixelPrdAssociationTracksTruth()
 xAODPixelPrdAssociationTrackParticleCnvAlg.ConvertTrackParticles = False
 xAODPixelPrdAssociationTrackParticleCnvAlg.ConvertTracks = True
 xAODPixelPrdAssociationTrackParticleCnvAlg.AddTruthLink = InDetFlags.doTruth()
 xAODPixelPrdAssociationTrackParticleCnvAlg.PrintIDSummaryInfo = True
 #xAODPixelPrdAssociationTrackParticleCnvAlg.OutputLevel = VERBOSE
 topSequence += xAODPixelPrdAssociationTrackParticleCnvAlg

if InDetFlags.doTrackSegmentsPixelFourLayer() and InDetFlags.doParticleCreation():
 xAODPixelFourLayerTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(InDetKeys.xAODPixelFourLayerTrackParticleContainer())
 xAODPixelFourLayerTrackParticleCnvAlg.xAODContainerName = InDetKeys.xAODPixelFourLayerTrackParticleContainer()
 xAODPixelFourLayerTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = InDetKeys.xAODPixelFourLayerTrackParticleContainer()
 xAODPixelFourLayerTrackParticleCnvAlg.TrackParticleCreator = InDetxAODParticleCreatorTool
 xAODPixelFourLayerTrackParticleCnvAlg.TrackContainerName = InDetKeys.PixelFourLayerTracks()
 xAODPixelFourLayerTrackParticleCnvAlg.TrackTruthContainerName = InDetKeys.PixelFourLayerTracksTruth()
 xAODPixelFourLayerTrackParticleCnvAlg.ConvertTrackParticles = False
 xAODPixelFourLayerTrackParticleCnvAlg.ConvertTracks = True
 xAODPixelFourLayerTrackParticleCnvAlg.AddTruthLink = InDetFlags.doTruth()
 xAODPixelFourLayerTrackParticleCnvAlg.PrintIDSummaryInfo = True
 #xAODPixelFourLayerTrackParticleCnvAlg.OutputLevel = VERBOSE
 topSequence += xAODPixelFourLayerTrackParticleCnvAlg

if InDetFlags.doTrackSegmentsPixelThreeLayer() and InDetFlags.doParticleCreation():
 xAODPixelThreeLayerTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(InDetKeys.xAODPixelThreeLayerTrackParticleContainer())
 xAODPixelThreeLayerTrackParticleCnvAlg.xAODContainerName = InDetKeys.xAODPixelThreeLayerTrackParticleContainer()
 xAODPixelThreeLayerTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = InDetKeys.xAODPixelThreeLayerTrackParticleContainer()
 xAODPixelThreeLayerTrackParticleCnvAlg.TrackParticleCreator = InDetxAODParticleCreatorTool
 xAODPixelThreeLayerTrackParticleCnvAlg.TrackContainerName = InDetKeys.PixelThreeLayerTracks()
 xAODPixelThreeLayerTrackParticleCnvAlg.TrackTruthContainerName = InDetKeys.PixelThreeLayerTracksTruth()
 xAODPixelThreeLayerTrackParticleCnvAlg.ConvertTrackParticles = False
 xAODPixelThreeLayerTrackParticleCnvAlg.ConvertTracks = True
 xAODPixelThreeLayerTrackParticleCnvAlg.AddTruthLink = InDetFlags.doTruth()
 xAODPixelThreeLayerTrackParticleCnvAlg.PrintIDSummaryInfo = True
 #xAODPixelThreeLayerTrackParticleCnvAlg.OutputLevel = VERBOSE
 topSequence += xAODPixelThreeLayerTrackParticleCnvAlg

if InDetFlags.doTrackSegmentsSCT() and InDetFlags.doParticleCreation():
 xAODSCTTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(InDetKeys.xAODSCTTrackParticleContainer())
 xAODSCTTrackParticleCnvAlg.xAODContainerName = InDetKeys.xAODSCTTrackParticleContainer()
 xAODSCTTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = InDetKeys.xAODSCTTrackParticleContainer()
 xAODSCTTrackParticleCnvAlg.TrackParticleCreator = InDetxAODParticleCreatorTool
 xAODSCTTrackParticleCnvAlg.TrackContainerName = InDetKeys.SCTTracks()
 xAODSCTTrackParticleCnvAlg.ConvertTrackParticles = False
 xAODSCTTrackParticleCnvAlg.ConvertTracks = True
 xAODSCTTrackParticleCnvAlg.AddTruthLink = False
 xAODSCTTrackParticleCnvAlg.PrintIDSummaryInfo = True
 #xAODSCTTrackParticleCnvAlg.OutputLevel = VERBOSE
 topSequence += xAODSCTTrackParticleCnvAlg

if InDetFlags.doTrackSegmentsTRT() and InDetFlags.doParticleCreation():
 xAODTRTTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(InDetKeys.xAODTRTTrackParticleContainer())
 xAODTRTTrackParticleCnvAlg.xAODContainerName = InDetKeys.xAODTRTTrackParticleContainer()
 xAODTRTTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = InDetKeys.xAODTRTTrackParticleContainer()
 xAODTRTTrackParticleCnvAlg.TrackParticleCreator = InDetxAODParticleCreatorTool
 xAODTRTTrackParticleCnvAlg.TrackContainerName = InDetKeys.TRTTracks()
 xAODTRTTrackParticleCnvAlg.ConvertTrackParticles = False
 xAODTRTTrackParticleCnvAlg.ConvertTracks = True
 xAODTRTTrackParticleCnvAlg.AddTruthLink = False
 xAODTRTTrackParticleCnvAlg.PrintIDSummaryInfo = True
 #xAODTRTTrackParticleCnvAlg.OutputLevel = VERBOSE
 topSequence += xAODTRTTrackParticleCnvAlg

if InDetFlags.doStoreTrackSeeds() and InDetFlags.doParticleCreation():
 from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
 xAODSeedsTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg( InDetKeys.SiSPSeedSegments()+"TrackParticle" )
 xAODSeedsTrackParticleCnvAlg.xAODContainerName = InDetKeys.SiSPSeedSegments()+"TrackParticle"
 xAODSeedsTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = InDetKeys.SiSPSeedSegments()+"TrackParticle"
 xAODSeedsTrackParticleCnvAlg.TrackParticleCreator = InDetxAODParticleCreatorTool
 xAODSeedsTrackParticleCnvAlg.TrackContainerName = InDetKeys.SiSPSeedSegments()
 xAODSeedsTrackParticleCnvAlg.ConvertTrackParticles = False
 xAODSeedsTrackParticleCnvAlg.ConvertTracks = True
 xAODSeedsTrackParticleCnvAlg.AddTruthLink = False
 xAODSeedsTrackParticleCnvAlg.PrintIDSummaryInfo = True
 #xAODSeedsTrackParticleCnvAlg.OutputLevel = VERBOSE
 topSequence += xAODSeedsTrackParticleCnvAlg
