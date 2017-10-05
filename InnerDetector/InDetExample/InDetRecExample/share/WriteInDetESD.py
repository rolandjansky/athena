#+++++++++++++++++ Beginning of WriteInDetESD.py
# Writes out all InDet ESD Data
from InDetRecExample.InDetKeys import InDetKeys
InDetESDList = []

# Save full and zero-suppressed BCM rdos
# (the latter is needed to allow writting to AOD and the former will hopefully be removed in future):
if not InDetFlags.doDBMstandalone(): 
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
   if InDetFlags.doNewTracking() and InDetFlags.doWriteTracksToESD():
      InDetESDList+=["TrackCollection#"+InDetKeys.UnslimmedTracks()]

if InDetFlags.doStoreTrackSeeds() and ( InDetFlags.doWriteTracksToESD() or  not InDetFlags.doxAOD() ) :
   InDetESDList+=["TrackCollection#"+InDetKeys.SiSPSeedSegments()]
   #if InDetFlags.doTruth():
   #   InDetESDList += ["TrackTruthCollection#"+InDetKeys.SiSPSeedSegments()+'TruthCollection']
   #   InDetESDList += ["DetailedTrackTruthCollection#"+InDetKeys.SiSPSeedSegments()+'DetailedTruth']

if InDetFlags.doWriteTracksToESD() or not InDetFlags.doxAOD() :
   if InDetKeys.AliasToTracks() == 'none':
      InDetESDList+=["TrackCollection#"+InDetKeys.Tracks()]
   else:
      InDetESDList+=["TrackCollection#"+InDetKeys.AliasToTracks()]

if InDetFlags.doTrackSegmentsPixel():
   if InDetFlags.doWriteTracksToESD() or  not InDetFlags.doxAOD() :
      InDetESDList+=["TrackCollection#"+InDetKeys.PixelTracks()]
   if InDetFlags.doTruth():
      InDetESDList += ["TrackTruthCollection#"+InDetKeys.PixelTracks()+'TruthCollection']
      InDetESDList += ["DetailedTrackTruthCollection#"+InDetKeys.PixelTracks()+'DetailedTruth']     
if InDetFlags.doTrackSegmentsSCT():
   if InDetFlags.doWriteTracksToESD() or  not InDetFlags.doxAOD() :
      InDetESDList+=["TrackCollection#"+InDetKeys.SCTTracks()]
   if InDetFlags.doTruth():
      InDetESDList += ["TrackTruthCollection#"+InDetKeys.SCTTracks()+'TruthCollection']
      InDetESDList += ["DetailedTrackTruthCollection#"+InDetKeys.SCTTracks()+'DetailedTruth']
if InDetFlags.doTrackSegmentsTRT():
   if InDetFlags.doWriteTracksToESD() or  not InDetFlags.doxAOD() :
      InDetESDList+=["TrackCollection#"+InDetKeys.TRTTracks()]
   if InDetFlags.doTruth():
      InDetESDList += ["TrackTruthCollection#"+InDetKeys.TRTTracks()+'TruthCollection']
      InDetESDList += ["DetailedTrackTruthCollection#"+InDetKeys.TRTTracks()+'DetailedTruth']
if InDetFlags.doPseudoTracking():
   if InDetFlags.doWriteTracksToESD() or not InDetFlags.doxAOD():
      InDetESDList+=["TrackCollection#"+InDetKeys.PseudoTracks()]
   if InDetFlags.doTruth():
      InDetESDList += ["TrackTruthCollection#"+InDetKeys.PseudoTracks()+'TruthCollection']
      InDetESDList += ["DetailedTrackTruthCollection#"+InDetKeys.PseudoTracks()+'DetailedTruth']

if InDetFlags.doDBMstandalone() or InDetFlags.doDBM(): 
   if InDetFlags.doWriteTracksToESD() or InDetFlags.doDBMstandalone() or not InDetFlags.doxAOD() :
      InDetESDList+=["TrackCollection#"+InDetKeys.DBMTracks()] 
   if InDetFlags.doTruth(): 
      InDetESDList += ["TrackTruthCollection#"+InDetKeys.DBMTracks()+'TruthCollection'] 
      InDetESDList += ["DetailedTrackTruthCollection#"+InDetKeys.DBMTracks()+'DetailedTruth'] 
   if InDetFlags.doxAOD(): 
      excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation" 
      InDetESDList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODDBMTrackParticleContainer()] 
      InDetESDList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODDBMTrackParticleContainer()+'Aux.' + excludedAuxData] 

# add the forward tracks for combined muon reconstruction
if InDetFlags.doForwardTracks():
   if InDetFlags.doWriteTracksToESD() or not InDetFlags.doxAOD():
      InDetESDList+=["TrackCollection#"+InDetKeys.ResolvedForwardTracks()]
   if InDetFlags.doTruth():
      InDetESDList += ["TrackTruthCollection#"+InDetKeys.ResolvedForwardTracks()+'TruthCollection']
      InDetESDList += ["DetailedTrackTruthCollection#"+InDetKeys.ResolvedForwardTracks()+'DetailedTruth']
if InDetFlags.doBeamGas():
   InDetESDList+=["TrackCollection#"+InDetBeamGasTRTExtension.ForwardTrackCollection()] #ExtendedHaloTracks
if InDetFlags.doBeamHalo():
   InDetESDList+=["TrackCollection#"+InDetRecHaloTRTExtension.ForwardTrackCollection()] #ExtendedBeamGasTracks 

if InDetFlags.doTrackSegmentsPixelPrdAssociation():
  if InDetFlags.doWriteTracksToESD() or not InDetFlags.doxAOD():
      InDetESDList+=["TrackCollection#"+InDetKeys.PixelPrdAssociationTracks()]
  if InDetFlags.doTruth():
      InDetESDList += ["TrackTruthCollection#"+InDetKeys.PixelPrdAssociationTracks()+'TruthCollection']
      InDetESDList += ["DetailedTrackTruthCollection#"+InDetKeys.PixelPrdAssociationTracks()+'DetailedTruth']   
   
if InDetFlags.doTrackSegmentsPixelFourLayer():
  if InDetFlags.doWriteTracksToESD() or not InDetFlags.doxAOD():
      InDetESDList+=["TrackCollection#"+InDetKeys.PixelFourLayerTracks()]
  if InDetFlags.doTruth():
      InDetESDList += ["TrackTruthCollection#"+InDetKeys.PixelFourLayerTracks()+'TruthCollection']
      InDetESDList += ["DetailedTrackTruthCollection#"+InDetKeys.PixelFourLayerTracks()+'DetailedTruth']   
   
if InDetFlags.doTrackSegmentsPixelThreeLayer():
  if InDetFlags.doWriteTracksToESD() or not InDetFlags.doxAOD():
      InDetESDList+=["TrackCollection#"+InDetKeys.PixelThreeLayerTracks()]
  if InDetFlags.doTruth():
      InDetESDList += ["TrackTruthCollection#"+InDetKeys.PixelThreeLayerTracks()+'TruthCollection']
      InDetESDList += ["DetailedTrackTruthCollection#"+InDetKeys.PixelThreeLayerTracks()+'DetailedTruth']   
   
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
  excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation."
  if not InDetFlags.KeepFirstParameters() :
      excludedAuxData += '-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition'

  if InDetFlags.keepAdditionalHitsOnTrackParticle():
   excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation"
  InDetESDList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODTrackParticleContainer()]
  InDetESDList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODTrackParticleContainer()+'Aux.' + excludedAuxData]

  if InDetFlags.doStoreTrackSeeds():
   InDetESDList+=['xAOD::TrackParticleContainer#'+InDetKeys.SiSPSeedSegments()+"TrackParticle"]
   InDetESDList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.SiSPSeedSegments()+"TrackParticle"+'Aux.' + excludedAuxData]

  if not InDetFlags.doSLHC():
     InDetESDList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODForwardTrackParticleContainer()]
     InDetESDList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODForwardTrackParticleContainer()+'Aux.' + excludedAuxData ]
  InDetESDList+=['xAOD::VertexContainer#'+InDetKeys.xAODVertexContainer()]
  InDetESDList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODVertexContainer()+'Aux.-vxTrackAtVertex']

  if InDetFlags.doV0Finder() and InDetFlags.doSimpleV0Finder():
    InDetESDList+=['xAOD::VertexContainer#'+InDetKeys.xAODV0VertexContainer()]
    InDetESDList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODV0VertexContainer()+'Aux.-vxTrackAtVertex']
  elif InDetFlags.doV0Finder(): 
    InDetESDList+=['xAOD::VertexContainer#'+InDetKeys.xAODKshortVertexContainer()]
    InDetESDList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODKshortVertexContainer()+'Aux.-vxTrackAtVertex']
    InDetESDList+=['xAOD::VertexContainer#'+InDetKeys.xAODLambdaVertexContainer()]
    InDetESDList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODLambdaVertexContainer()+'Aux.-vxTrackAtVertex']
    InDetESDList+=['xAOD::VertexContainer#'+InDetKeys.xAODLambdabarVertexContainer()]
    InDetESDList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODLambdabarVertexContainer()+'Aux.-vxTrackAtVertex']
  if InDetFlags.doConversions():
    InDetESDList+=['xAOD::VertexContainer#'+InDetKeys.Conversions()]
    InDetESDList+=['xAOD::VertexAuxContainer#'+InDetKeys.Conversions() +'Aux.-vxTrackAtVertex']

  if InDetFlags.doTrackSegmentsPixel():
    InDetESDList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODPixelTrackParticleContainer()]
    InDetESDList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODPixelTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsPixelPrdAssociation():
    InDetESDList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODPixelPrdAssociationTrackParticleContainer()]
    InDetESDList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODPixelPrdAssociationTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsPixelFourLayer():
    InDetESDList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODPixelFourLayerTrackParticleContainer()]
    InDetESDList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODPixelFourLayerTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsPixelThreeLayer():
    InDetESDList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODPixelThreeLayerTrackParticleContainer()]
    InDetESDList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODPixelThreeLayerTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsSCT():
    InDetESDList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODSCTTrackParticleContainer()]
    InDetESDList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODSCTTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsTRT():
    InDetESDList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODTRTTrackParticleContainer()]
    InDetESDList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODTRTTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doPseudoTracking():
    InDetESDList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODPseudoTrackParticleContainer()]
    InDetESDList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODPseudoTrackParticleContainer()+'Aux.' + excludedAuxData]
# next is only for InDetRecExample stand alone! RecExCommon uses InDetESDList directly
# added to InDetRec_all.py after include WriteInDetESD!
#StreamESD.ItemList += InDetESDList
#+++++++++++++++++ End of WriteInDetESD.py
