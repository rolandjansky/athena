from AthenaCommon.DetFlags import DetFlags
from RecExConfig.RecFlags import rec
from MuonRecExample.MuonRecFlags import muonRecFlags
from AthenaCommon.BeamFlags import jobproperties

#AOD list, also added to the ESD
MuonAODList = []

if DetFlags.detdescr.Muon_on() and (rec.doWriteAOD() or rec.doWriteESD()):
   
   # Segments 
   MuonAODList+=[ "xAOD::MuonSegmentContainer_v1#MuonSegments" ]
   MuonAODList+=[ "xAOD::MuonSegmentAuxContainer_v1#MuonSegmentsAux." ]

   # TrackParticles 
   MuonAODList+=[ "xAOD::TrackParticleContainer_v1#MuonSpectrometerTrackParticles" ]
   MuonAODList+=[ "xAOD::TrackParticleAuxContainer_v1#MuonSpectrometerTrackParticlesAux." ]

   if rec.doTruth():
      # Truth Particle Container
      MuonAODList+=["xAOD::TruthParticleContainer_v1#MuonTruthParticle"]
      MuonAODList+=["xAOD::TruthParticleAuxContainer_v1#MuonTruthParticleAux."]

      # Truth Segment Container
      MuonAODList+=["xAOD::MuonSegmentContainer_v1#MuonTruthSegments"]
      MuonAODList+=["xAOD::MuonSegmentAuxContainer_v1#MuonTruthSegmentsAux."]

   if muonRecFlags.prdToxAOD():
      MuonAODList += [ "xAOD::PrepRawDataContainer_v1#MDT_DriftCircles"] 
      MuonAODList += [ "xAOD::PrepRawDataAuxContainer_v1#MDT_DriftCirclesAux."] 
      MuonAODList += [ "xAOD::PrepRawDataContainer_v1#RPC_Measurements"] 
      MuonAODList += [ "xAOD::PrepRawDataAuxContainer_v1#RPC_MeasurementsAux."] 
      MuonAODList += [ "xAOD::PrepRawDataContainer_v1#TGC_MeasurementsAllBCs"] 
      MuonAODList += [ "xAOD::PrepRawDataAuxContainer_v1#TGC_MeasurementsAllBCsAux."] 
      MuonAODList += [ "xAOD::PrepRawDataContainer_v1#CSC_Clusters"] 
      MuonAODList += [ "xAOD::PrepRawDataAuxContainer_v1#CSC_ClustersAux."] 
      if muonRecFlags.doCreateClusters:
         MuonAODList += [ "xAOD::PrepRawDataContainer_v1#RPC_Clusters"] 
         MuonAODList += [ "xAOD::PrepRawDataAuxContainer_v1#RPC_ClustersAux."] 
         MuonAODList += [ "xAOD::PrepRawDataContainer_v1#TGC_Clusters"] 
         MuonAODList += [ "xAOD::PrepRawDataAuxContainer_v1#TGC_ClustersAux."] 
         
# ESD list includes all AOD items
MuonESDList = []
MuonESDList += MuonAODList

if DetFlags.detdescr.Muon_on() and rec.doWriteESD():


   #PRDs
   MuonESDList+=["Muon::CscPrepDataContainer#CSC_Clusters"]
   MuonESDList+=["Muon::CscStripPrepDataContainer#CSC_Measurements"]
   MuonESDList+=["Muon::RpcPrepDataContainer#RPC_Measurements"]
   MuonESDList+=["Muon::TgcPrepDataContainer#TGC_MeasurementsAllBCs"]
   MuonESDList+=["Muon::MdtPrepDataContainer#MDT_DriftCircles"]
   MuonESDList+=["Muon::MMPrepDataContainer#MM_Measurements"]
   MuonESDList+=["Muon::sTgcPrepDataContainer#STGC_Measurements"]

   # Segments
   MuonESDList+=["Trk::SegmentCollection#MuonSegments"]

   # Tracks
   MuonESDList+=["TrackCollection#MuonSpectrometerTracks"] 

   # Truth
   if rec.doTruth():
      MuonESDList += [ "TrackRecordCollection#MuonEntryLayerFilter" ]
      MuonESDList += [ "TrackRecordCollection#MuonExitLayerFilter" ]
               
      MuonESDList += ["PRD_MultiTruthCollection#MDT_TruthMap","PRD_MultiTruthCollection#CSC_TruthMap","PRD_MultiTruthCollection#RPC_TruthMap",
                      "PRD_MultiTruthCollection#TGC_TruthMap","PRD_MultiTruthCollection#STGC_TruthMap","PRD_MultiTruthCollection#MM_TruthMap"]

      #Track truth
      MuonESDList+=["DetailedTrackTruthCollection#MuonSpectrometerTracksTruth"]
      MuonESDList+=["TrackTruthCollection#MuonSpectrometerTracksTruth"]

      if muonRecFlags.writeSDOs():
         MuonESDList+=["CscSimDataCollection#CSC_SDO"]
         MuonESDList+=["MuonSimDataCollection#MDT_SDO"]
         MuonESDList+=["MuonSimDataCollection#RPC_SDO"]
         MuonESDList+=["MuonSimDataCollection#TGC_SDO"]
         MuonESDList+=["MuonSimDataCollection#STGC_SDO"]
         MuonESDList+=["MuonSimDataCollection#MM_SDO"]

   if muonRecFlags.Commissioning():
      # Write out CSC, RPC, and MDT RDOs.
      # I'm wondering if we should be doing something in MuonCnvExample to ensure we have the right
      # configuration? See bug#59676
      MuonESDList += [ "MdtCsmContainer#MDTCSM", "CscRawDataContainer#CSCRDO", "RpcPadContainer#RPCPAD"] 
      
# Muon Clusters
#try:
#   topSequence.StreamESD.ItemList += MuonESDList
#except AttributeError:
#   pass # not writing ESD

