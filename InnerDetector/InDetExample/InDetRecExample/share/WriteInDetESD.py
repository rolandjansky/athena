#+++++++++++++++++ Beginning of WriteInDetESD.py
# Writes out all InDet ESD Data
from InDetRecExample.InDetKeys import InDetKeys
InDetESDList = []

# Save full and zero-suppressed BCM rdos
# (the latter is needed to allow writting to AOD and the former will hopefully be removed in future):
InDetESDList+=["BCM_RDO_Container#"+InDetKeys.BCM_RDOs()]
InDetESDList+=["BCM_RDO_Container#"+InDetKeys.BCM_CompactDOs()]

# In case of cosmics we save the RDOs as well
if InDetFlags.writeRDOs():
   InDetESDList+=['PixelRDO_Container#'+InDetKeys.PixelRDOs(),
                  'SCT_RDO_Container#'+InDetKeys.SCT_RDOs(),
                  'TRT_RDO_Container#'+InDetKeys.TRT_RDOs()]

# write phase calculation into ESD
if InDetFlags.doTRTPhaseCalculation():
   InDetESDList+=['ComTime#TRT_Phase']

# Save PRD.
# ---------
from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource == 'data' and InDetFlags.doHeavyIon():
   print "---- > Heavy Ions: No Pixel, SCT or TRT cluster output written for data"
elif InDetFlags.writePRDs():
   InDetESDList+=["InDet::SCT_ClusterContainer#"+InDetKeys.SCT_Clusters()]
   InDetESDList+=["InDet::PixelClusterContainer#"+InDetKeys.PixelClusters()]
   InDetESDList+=["InDet::TRT_DriftCircleContainer#"+InDetKeys.TRT_DriftCircles()]
   # Save PixelGangedAmbiguities
   # ---------------------------
   InDetESDList+=["InDet::PixelGangedClusterAmbiguities#"+InDetKeys.GangedPixelMap()]
   if InDetFlags.doPixelClusterSplitting():
      InDetESDList+=["InDet::PixelGangedClusterAmbiguities#"+InDetKeys.SplitClusterAmbiguityMap()]

# add tracks
# ----------
#
# this is a hack for validating the NewT Cosmics tracks!!!!
if InDetFlags.doCosmics():
   if InDetFlags.doNewTracking():
      InDetESDList+=["TrackCollection#"+InDetKeys.UnslimmedTracks()]

if InDetKeys.AliasToTracks() == 'none':
   InDetESDList+=["TrackCollection#"+InDetKeys.Tracks()]
else:
   InDetESDList+=["TrackCollection#"+InDetKeys.AliasToTracks()]

if InDetFlags.doTrackSegmentsPixel():
   InDetESDList+=["TrackCollection#"+InDetKeys.PixelTracks()]
   if InDetFlags.doTruth():
      InDetESDList += ["TrackTruthCollection#"+InDetKeys.PixelTracks()+'TruthCollection']
      InDetESDList += ["DetailedTrackTruthCollection#"+InDetKeys.PixelTracks()+'DetailedTruth']     
if InDetFlags.doTrackSegmentsSCT():
   InDetESDList+=["TrackCollection#"+InDetKeys.SCTTracks()]
   if InDetFlags.doTruth():
      InDetESDList += ["TrackTruthCollection#"+InDetKeys.SCTTracks()+'TruthCollection']
      InDetESDList += ["DetailedTrackTruthCollection#"+InDetKeys.SCTTracks()+'DetailedTruth']
if InDetFlags.doTrackSegmentsTRT():
   InDetESDList+=["TrackCollection#"+InDetKeys.TRTTracks()]
   if InDetFlags.doTruth():
      InDetESDList += ["TrackTruthCollection#"+InDetKeys.TRTTracks()+'TruthCollection']
      InDetESDList += ["DetailedTrackTruthCollection#"+InDetKeys.TRTTracks()+'DetailedTruth']

# add the forward tracks for combined muon reconstruction
if InDetFlags.doForwardTracks():
   InDetESDList+=["TrackCollection#"+InDetKeys.ResolvedForwardTracks()]
   if InDetFlags.doTruth():
      InDetESDList += ["TrackTruthCollection#"+InDetKeys.ResolvedForwardTracks()+'TruthCollection']
      InDetESDList += ["DetailedTrackTruthCollection#"+InDetKeys.ResolvedForwardTracks()+'DetailedTruth']
if InDetFlags.doBeamGas():
   InDetESDList+=["TrackCollection#"+InDetBeamGasTRTExtension.ForwardTrackCollection()] #ExtendedHaloTracks
if InDetFlags.doBeamHalo():
   InDetESDList+=["TrackCollection#"+InDetRecHaloTRTExtension.ForwardTrackCollection()] #ExtendedBeamGasTracks 
   
# Add TRT Segments (only if standalone is off).
# -----------------
if not InDetFlags.doTRTStandalone():
   InDetESDList+=["Trk::SegmentCollection#"+InDetKeys.TRT_Segments()]

if InDetFlags.doTruth():
   # Save (Detailed) Track Truth
   # ---------------------------
   InDetESDList+=["TrackTruthCollection#"+InDetKeys.TracksTruth()]
   InDetESDList+=["DetailedTrackTruthCollection#"+InDetKeys.DetailedTracksTruth()]

   # save PRD MultiTruth. 
   # --------------------
   InDetESDList+=["PRD_MultiTruthCollection#"+InDetKeys.PixelClustersTruth()] 
   InDetESDList+=["PRD_MultiTruthCollection#"+InDetKeys.SCT_ClustersTruth()]
   InDetESDList+=["PRD_MultiTruthCollection#"+InDetKeys.TRT_DriftCirclesTruth()]
   

if globalflags.InputFormat()=='bytestream':     
   InDetESDList+=['InDetBSErrContainer#'+InDetKeys.PixelByteStreamErrs()]
   InDetESDList+=['InDetBSErrContainer#'+InDetKeys.SCT_ByteStreamErrs()]
   InDetESDList+=['TRT_BSErrContainer#'+InDetKeys.TRT_ByteStreamErrs()]
   InDetESDList+=['TRT_BSIdErrContainer#'+InDetKeys.TRT_ByteStreamIdErrs()]

if InDetFlags.doxAOD():
  excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition"
  InDetESDList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODTrackParticleContainer()]
  InDetESDList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODTrackParticleContainer()+'Aux.' + excludedAuxData]
  InDetESDList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODForwardTrackParticleContainer()]
  InDetESDList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODForwardTrackParticleContainer()+'Aux.' + excludedAuxData ]
  InDetESDList+=['xAOD::VertexContainer#'+InDetKeys.xAODVertexContainer()]
  InDetESDList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODVertexContainer()+'Aux.-vxTrackAtVertex']
  if InDetFlags.doTrackSegmentsPixel():
    InDetESDList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODPixelTrackParticleContainer()]
    InDetESDList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODPixelTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsPixelPrdAssociation():
    InDetESDList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODPixelPrdAssociationTrackParticleContainer()]
    InDetESDList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODPixelPrdAssociationTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsSCT():
    InDetESDList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODSCTTrackParticleContainer()]
    InDetESDList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODSCTTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsTRT():
    InDetESDList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODTRTTrackParticleContainer()]
    InDetESDList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODTRTTrackParticleContainer()+'Aux.' + excludedAuxData]
# next is only for InDetRecExample stand alone! RecExCommon uses InDetESDList directly
# added to InDetRec_all.py after include WriteInDetESD!
#StreamESD.ItemList += InDetESDList
#+++++++++++++++++ End of WriteInDetESD.py
