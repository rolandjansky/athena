#+++++++++++++++++ Beginning of WriteInDetESD.py
# Writes out all InDet ESD Data
StreamESD.ItemList+=["TrackCollection#*"]
StreamESD.ItemList+=["VxContainer#*"]
StreamESD.ItemList+=["Rec::TrackParticleContainer#*"]
StreamESD.ItemList+=["Rec::TrackParticleContainer#*"]
                                                                            
# Add truth to stream - it simply won't be written out if truth is turned off
if doTruth:
   StreamESD.ItemList+=["TrackTruthCollection#*"]
   StreamESD.ItemList+=["TrackParticleTruthCollection#*"]

if InDetTrigFlags.restrictedESD():
   print ''
   print ''
   print ' ESD content incomplete for Inner Detector for this run'
   print ''
   print ''
                                                                                                                                                            
else:

   # Save PRD.
   # ---------
   StreamESD.ItemList+=["InDet::SCT_ClusterContainer#*"]
   StreamESD.ItemList+=["InDet::PixelClusterContainer#*"]
   StreamESD.ItemList+=["InDet::TRT_DriftCircleContainer#*"]
   
   # Add TRT Segments.
   # -----------------
   StreamESD.ItemList+=["Trk::SegmentCollection#*"]
   
   if doTruth:	
    	#Save PRD MultiTruth.
      # --------------------
      StreamESD.ItemList+=["PRD_MultiTruthCollection#*"]
      StreamESD.ItemList+=["PRD_MultiTruthCollection#*"]
      StreamESD.ItemList+=["PRD_MultiTruthCollection#*"]
      
      StreamESD.ItemList+=["TrackParticleTruthCollection#*"]
#+++++++++++++++++ End of WriteInDetESD.py
