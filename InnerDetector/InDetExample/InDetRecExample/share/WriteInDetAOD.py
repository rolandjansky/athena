#+++++++++++++++++ Beginning of WriteInDetAOD.py

from InDetRecExample.InDetKeys import InDetKeys
InDetAODList = []

# Add Vertices and Particles.
# -----------------------------

if InDetFlags.doxAOD():
  excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation."
  if not InDetFlags.KeepFirstParameters() :
          excludedAuxData += '-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition'

  excludedVertexAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"

  if InDetFlags.keepAdditionalHitsOnTrackParticle():
   excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation"  
  InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODTrackParticleContainer()]
  InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODTrackParticleContainer()+'Aux.' + excludedAuxData]
  from  InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags
  from  InDetPhysValMonitoring.ConfigUtils import extractCollectionPrefix
  for col in InDetPhysValFlags.validateExtraTrackCollections() :
    prefix=extractCollectionPrefix(col)
    InDetAODList+=['xAOD::TrackParticleContainer#'+prefix+'TrackParticles']
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+prefix+'TrackParticlesAux.' + excludedAuxData]
  if not InDetFlags.doSLHC():
   InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODForwardTrackParticleContainer()]
   InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODForwardTrackParticleContainer()+'Aux.' + excludedAuxData]
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.xAODVertexContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODVertexContainer()+'Aux.' + excludedVertexAuxData]
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.xAODV0VertexContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODV0VertexContainer()+'Aux.' + excludedVertexAuxData]
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.xAODKshortVertexContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODKshortVertexContainer()+'Aux.' + excludedVertexAuxData]
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.xAODLambdaVertexContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODLambdaVertexContainer()+'Aux.' + excludedVertexAuxData]
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.xAODLambdabarVertexContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODLambdabarVertexContainer()+'Aux.' + excludedVertexAuxData]
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.Conversions()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.Conversions() +'Aux.' + excludedVertexAuxData]
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.xAODSecVertexDecayContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODSecVertexDecayContainer()+'Aux.' + excludedVertexAuxData]

  if InDetFlags.doTrackSegmentsPixel():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODPixelTrackParticleContainer()]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODPixelTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsPixelPrdAssociation():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODPixelPrdAssociationTrackParticleContainer()]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODPixelPrdAssociationTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsPixelFourLayer():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODPixelFourLayerTrackParticleContainer()]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODPixelFourLayerTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsPixelThreeLayer():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODPixelThreeLayerTrackParticleContainer()]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODPixelThreeLayerTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsSCT():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODSCTTrackParticleContainer()]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODSCTTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsTRT():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODTRTTrackParticleContainer()]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODTRTTrackParticleContainer()+'Aux.' + excludedAuxData]
  InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODLowPtRoITrackParticleContainer()]
  InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODLowPtRoITrackParticleContainer()+'Aux.' + excludedAuxData]
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.xAODLowPtRoIVertexContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODLowPtRoIVertexContainer()+'Aux.' + excludedVertexAuxData]
  if InDetFlags.doStoreTrackSeeds():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.SiSPSeedSegments()+"TrackParticle"]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.SiSPSeedSegments()+"TrackParticle"+'Aux.' + excludedAuxData]
  if InDetFlags.doStoreTrackSeeds() and InDetFlags.doLowPtRoI():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.SiSPLowPtRoISeedSegments()+"TrackParticle"]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.SiSPLowPtRoISeedSegments()+"TrackParticle"+'Aux.' + excludedAuxData]
  if InDetFlags.doStoreTrackCandidates():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODSiSPTrackCandidates()+"TrackParticle"]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODSiSPTrackCandidates()+"TrackParticle"+'Aux.' + excludedAuxData]
  if InDetFlags.doStoreTrackCandidates() and InDetFlags.doLowPtRoI():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODSiSPLowPtRoITrackCandidates()+"TrackParticle"]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODSiSPLowPtRoITrackCandidates()+"TrackParticle"+'Aux.' + excludedAuxData]
   
  if InDetFlags.doDBMstandalone() or InDetFlags.doDBM(): 
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODDBMTrackParticleContainer()] 
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODDBMTrackParticleContainer()+'Aux.' + excludedAuxData] 
    InDetAODList+=["TrackCollection#"+InDetKeys.DBMTracks()] 
    if InDetFlags.doTruth(): 
      InDetAODList += ["TrackTruthCollection#"+InDetKeys.DBMTracks()+'TruthCollection'] 
      InDetAODList += ["DetailedTrackTruthCollection#"+InDetKeys.DBMTracks()+'DetailedTruth'] 

# next is only for InDetRecExample stand alone! RecExCommon uses InDetAODList directly
StreamAOD.ItemList += InDetAODList 
#+++++++++++++++++ End of WriteInDetAOD.py
