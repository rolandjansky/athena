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
  InDetAODList+=['xAOD::TrackParticleContainer_v1#'+InDetKeys.xAODTrackParticleContainer()]
  InDetAODList+=['xAOD::TrackParticleAuxContainer_v1#'+InDetKeys.xAODTrackParticleContainer()+'Aux.']
  InDetAODList+=['xAOD::TrackParticleAuxContainer_v1#'+InDetKeys.xAODTrackParticleContainer()+'Aux.-caloExtension']
  InDetAODList+=['xAOD::TrackParticleContainer_v1#'+InDetKeys.xAODForwardTrackParticleContainer()]
  InDetAODList+=['xAOD::TrackParticleAuxContainer_v1#'+InDetKeys.xAODForwardTrackParticleContainer()+'Aux.']
  InDetAODList+=['xAOD::TrackParticleAuxContainer_v1#'+InDetKeys.xAODForwardTrackParticleContainer()+'Aux.-caloExtension']
  InDetAODList+=['xAOD::VertexContainer_v1#'+InDetKeys.xAODVertexContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer_v1#'+InDetKeys.xAODVertexContainer()+'Aux.-vxTrackAtVertex']

# next is only for InDetRecExample stand alone! RecExCommon uses InDetAODList directly
StreamAOD.ItemList += InDetAODList 
#+++++++++++++++++ End of WriteInDetAOD.py
