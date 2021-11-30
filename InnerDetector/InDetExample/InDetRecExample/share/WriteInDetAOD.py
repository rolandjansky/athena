#+++++++++++++++++ Beginning of WriteInDetAOD.py

from InDetRecExample.InDetKeys import InDetKeys
InDetAODList = []

# Add Vertices and Particles.
# -----------------------------

if InDetFlags.doxAOD():

  excludedAuxData = "-clusterAssociation.-TTVA_AMVFVertices_forReco.-TTVA_AMVFWeights_forReco"

  if not (InDetFlags.KeepFirstParameters() or InDetFlags.keepAdditionalHitsOnTrackParticle()):
    excludedAuxData += '.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition'

  excludedVertexAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"

  # remove track decorations used internally by FTAG software
  excludedAuxData += '.-TrackCompatibility.-JetFitter_TrackCompatibility_antikt4emtopo.-JetFitter_TrackCompatibility_antikt4empflow' \
                   + '.-btagIp_d0Uncertainty.-btagIp_z0SinThetaUncertainty.-btagIp_z0SinTheta.-btagIp_d0.-btagIp_trackMomentum.-btagIp_trackDisplacement' \
                   + '.-VxTrackAtVertex'
  # @TODO the exclusion of VxTrackAtVertex for InDetTrackParticles is presumably not necessary

  # exclude IDTIDE decorations
  excludedAuxData += '.-IDTIDE1_biased_PVd0Sigma.-IDTIDE1_biased_PVz0Sigma.-IDTIDE1_biased_PVz0SigmaSinTheta.-IDTIDE1_biased_d0.-IDTIDE1_biased_d0Sigma' \
  +'.-IDTIDE1_biased_z0.-IDTIDE1_biased_z0Sigma.-IDTIDE1_biased_z0SigmaSinTheta.-IDTIDE1_biased_z0SinTheta.-IDTIDE1_unbiased_PVd0Sigma.-IDTIDE1_unbiased_PVz0Sigma' \
  +'.-IDTIDE1_unbiased_PVz0SigmaSinTheta.-IDTIDE1_unbiased_d0.-IDTIDE1_unbiased_d0Sigma.-IDTIDE1_unbiased_z0.-IDTIDE1_unbiased_z0Sigma.-IDTIDE1_unbiased_z0SigmaSinTheta' \
  +'.-IDTIDE1_unbiased_z0SinTheta'

  # exclude TTVA decorations 
  excludedAuxData += '.-TTVA_AMVFVertices.-TTVA_AMVFWeights'

  # exclude IDTIDE/IDTRKVALID decorations
  excludedAuxData += '.-TrkBLX.-TrkBLY.-TrkBLZ.-TrkIBLX.-TrkIBLY.-TrkIBLZ.-TrkL1X.-TrkL1Y.-TrkL1Z.-TrkL2X.-TrkL2Y.-TrkL2Z.-msosLink'

  InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODTrackParticleContainer()]
  InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODTrackParticleContainer()+'Aux.' + excludedAuxData]
  from  InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags
  from  InDetPhysValMonitoring.ConfigUtils import extractCollectionPrefix
  for col in InDetPhysValFlags.validateExtraTrackCollections() :
    prefix=extractCollectionPrefix(col)
    InDetAODList+=['xAOD::TrackParticleContainer#'+prefix+'TrackParticles']
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+prefix+'TrackParticlesAux.' + excludedAuxData]
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
  InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODLargeD0TrackParticleContainer()]
  InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODLargeD0TrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsPixel():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODPixelTrackParticleContainer()]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODPixelTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsDisappearing():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODDisappearingTrackParticleContainer()]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODDisappearingTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsSCT():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODSCTTrackParticleContainer()]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODSCTTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsTRT():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODTRTTrackParticleContainer()]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODTRTTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doDBMstandalone() or InDetFlags.doDBM(): 
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODDBMTrackParticleContainer()] 
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODDBMTrackParticleContainer()+'Aux.' + excludedAuxData] 
    InDetAODList+=["TrackCollection#"+InDetKeys.DBMTracks()] 
    if InDetFlags.doTruth(): 
      InDetAODList += ["TrackTruthCollection#"+InDetKeys.DBMTracks()+'TruthCollection'] 
      InDetAODList += ["DetailedTrackTruthCollection#"+InDetKeys.DBMTracks()+'DetailedTruth'] 
  if InDetFlags.doPseudoTracking():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODPseudoTrackParticleContainer()]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODPseudoTrackParticleContainer()+'Aux.' + excludedAuxData]
    if InDetFlags.doTruth():
        InDetAODList += ["TrackTruthCollection#"+InDetKeys.PseudoTracksTruth()]
        InDetAODList += ["DetailedTrackTruthCollection#"+InDetKeys.PseudoDetailedTracksTruth()]
  if InDetFlags.doTIDE_Ambi():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODObservedTrackParticleContainer()]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODObservedTrackParticleContainer()+'Aux.' + excludedAuxData]
    if InDetFlags.doTruth():
      InDetAODList += ["TrackTruthCollection#"+InDetKeys.ObservedTracksTruth()]
      InDetAODList += ["DetailedTrackTruthCollection#"+InDetKeys.ObservedDetailedTracksTruth()]

# next is only for InDetRecExample stand alone! RecExCommon uses InDetAODList directly
StreamAOD.ItemList += InDetAODList 
#+++++++++++++++++ End of WriteInDetAOD.py
