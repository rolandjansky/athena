from InDetRecExample.InDetJobProperties import InDetFlags
from InDetRecExample.InDetKeys import InDetKeys

doCreation = ( InDetFlags.doNewTracking() or InDetFlags.doPseudoTracking() ) and InDetFlags.doParticleCreation()
doConversion = not InDetFlags.doNewTracking()  and not InDetFlags.doPseudoTracking()  and  InDetFlags.doParticleCreation()

if doCreation:
    print "Creating xAOD::TrackParticles from Trk::Tracks"
if doConversion:
    print "Converting Rec::TrackParticles to xAOD::TrackParticles"


from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
InDetxAODParticleCreatorTool = Trk__TrackParticleCreatorTool(name = "InDetxAODParticleCreatorTool", 
                                                             Extrapolator            = InDetExtrapolator,
                                                             TrackSummaryTool        = InDetTrackSummaryToolSharedHits,
                                                             ForceTrackSummaryUpdate = False,
                                                             KeepParameters          = True)


ToolSvc += InDetxAODParticleCreatorTool 
if InDetFlags.doPrintConfigurables(): 
    print InDetxAODParticleCreatorTool 


if doCreation or doConversion:
 from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
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
 xAODTrackParticleCnvAlg.TrackTruthContainerName = InDetKeys.TracksTruth()
 xAODTrackParticleCnvAlg.PrintIDSummaryInfo = True
 #xAODTrackParticleCnvAlg.OutputLevel = VERBOSE
 topSequence += xAODTrackParticleCnvAlg

if not InDetFlags.doVertexFinding():
 from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__VertexCnvAlg
 xAODVertexCnvAlg = xAODMaker__VertexCnvAlg()
 xAODVertexCnvAlg.xAODContainerName = InDetKeys.xAODVertexContainer()
 xAODVertexCnvAlg.AODContainerName = InDetKeys.PrimaryVertices()
 xAODVertexCnvAlg.TPContainerName = InDetKeys.xAODTrackParticleContainer()
 topSequence += xAODVertexCnvAlg

if (InDetFlags.doForwardTracks() and InDetFlags.doParticleCreation()) or doConversion:       
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
 xAODPixelTrackParticleCnvAlg.AddTruthLink = False
 xAODPixelTrackParticleCnvAlg.PrintIDSummaryInfo = True
 #xAODPixelTrackParticleCnvAlg.OutputLevel = VERBOSE
 topSequence += xAODPixelTrackParticleCnvAlg

if InDetFlags.doTrackSegmentsPixelPrdAssociation() and InDetFlags.doParticleCreation():
 xAODPixelPrdAssociationTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(InDetKeys.xAODPixelPrdAssociationTrackParticleContainer())
 xAODPixelPrdAssociationTrackParticleCnvAlg.xAODContainerName = InDetKeys.xAODPixelPrdAssociationTrackParticleContainer()
 xAODPixelPrdAssociationTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = InDetKeys.xAODPixelPrdAssociationTrackParticleContainer()
 xAODPixelPrdAssociationTrackParticleCnvAlg.TrackParticleCreator = InDetxAODParticleCreatorTool
 xAODPixelPrdAssociationTrackParticleCnvAlg.TrackContainerName = InDetKeys.PixelPrdAssociationTracks()
 xAODPixelPrdAssociationTrackParticleCnvAlg.ConvertTrackParticles = False
 xAODPixelPrdAssociationTrackParticleCnvAlg.ConvertTracks = True
 xAODPixelPrdAssociationTrackParticleCnvAlg.AddTruthLink = False
 xAODPixelPrdAssociationTrackParticleCnvAlg.PrintIDSummaryInfo = True
 #xAODPixelTrackParticleCnvAlg.OutputLevel = VERBOSE
 topSequence += xAODPixelPrdAssociationTrackParticleCnvAlg

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
