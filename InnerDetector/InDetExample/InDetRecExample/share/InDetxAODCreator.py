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
