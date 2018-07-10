#+++++++++++++++++ Beginning of WriteInDetAOD.py

from InDetRecExample.InDetKeys import InDetKeys
InDetAODList = []

# Add Vertices and Particles.
# -----------------------------
#if rec.doWriteAOD:
#   InDetAODList+=["VxContainer#"+InDetKeys.PrimaryVertices()]
#   InDetAODList+=["V0Container#"+InDetKeys.V0Candidates()]
#   InDetAODList+=["V0Container#"+InDetKeys.SimpleV0Candidates()]
#   InDetAODList+=["VxContainer#"+InDetKeys.SecVertices()]
#   InDetAODList+=["VxContainer#"+InDetKeys.Conversions()]
#   InDetAODList+=["Rec::TrackParticleContainer#"+InDetKeys.TrackParticles()]
#   InDetAODList+=["TrackParticleTruthCollection#"+InDetKeys.TrackParticlesTruth()]
#   InDetAODList+=["Rec::TrackParticleContainer#"+InDetKeys.ResolvedForwardTrackParticles()]
#   InDetAODList+=["TrackParticleTruthCollection#"+InDetKeys.ResolvedForwardTrackParticlesTruth()]

#if InDetFlags.doLowBetaFinder():
#   InDetAODList+=["InDet::InDetLowBetaContainer#"+InDetKeys.LowBetaCandidates()]

# if AODall is on the iPat and xKal (if on) are written to ESD/AOD
#if InDetFlags.AODall():
#   InDetAODList+=["VxContainer#"+InDetKeys.IPatPrimaryVertices()]
#   InDetAODList+=["Rec::TrackParticleContainer#"+InDetKeys.IPatParticles()]
#   InDetAODList+=["TrackParticleTruthCollection#"+InDetKeys.IPatParticlesTruth()]
#   InDetAODList+=["VxContainer#"+InDetKeys.XKalPrimaryVertices()]
#   InDetAODList+=["Rec::TrackParticleContainer#"+InDetKeys.XKalParticles()]
#   InDetAODList+=["TrackParticleTruthCollection#"+InDetKeys.XKalParticlesTruth()]

# Add bytestream errors (needed in order to re-run InDetTestBLayer tool)
#if globalflags.InputFormat()=='bytestream':
#   InDetAODList+=['InDetBSErrContainer#'+InDetKeys.PixelByteStreamErrs()]
#   InDetAODList+=['InDetBSErrContainer#'+InDetKeys.SCT_ByteStreamErrs()]
#   InDetAODList+=['TRT_BSErrContainer#'+InDetKeys.TRT_ByteStreamErrs()]
#   InDetAODList+=['TRT_BSIdErrContainer#'+InDetKeys.TRT_ByteStreamIdErrs()]

if InDetFlags.doxAOD():
  excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition"
  if InDetFlags.keepAdditionalHitsOnTrackParticle():
   excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation"  
  InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODTrackParticleContainer()]
  InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODTrackParticleContainer()+'Aux.' + excludedAuxData]
  if not InDetFlags.doSLHC():
   InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODForwardTrackParticleContainer()]
   InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODForwardTrackParticleContainer()+'Aux.' + excludedAuxData]
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.xAODVertexContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODVertexContainer()+'Aux.-vxTrackAtVertex']
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.xAODV0VertexContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODV0VertexContainer()+'Aux.-vxTrackAtVertex']
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.xAODKshortVertexContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODKshortVertexContainer()+'Aux.-vxTrackAtVertex']
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.xAODLambdaVertexContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODLambdaVertexContainer()+'Aux.-vxTrackAtVertex']
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.xAODLambdabarVertexContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODLambdabarVertexContainer()+'Aux.-vxTrackAtVertex']
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.Conversions()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.Conversions() +'Aux.-vxTrackAtVertex']
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
