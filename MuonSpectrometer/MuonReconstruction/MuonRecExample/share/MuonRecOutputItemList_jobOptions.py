from AthenaCommon.DetFlags import DetFlags
from RecExConfig.RecFlags import rec
from MuonRecExample.MuonRecFlags import muonRecFlags
from AthenaCommon.BeamFlags import jobproperties

#AOD list, also added to the ESD
MuonAODList = []

if DetFlags.detdescr.Muon_on() and (rec.doWriteAOD() or rec.doWriteESD()):
   
   # Segments 
   MuonAODList+=[ "xAOD::MuonSegmentContainer#MuonSegments" ]
   MuonAODList+=[ "xAOD::MuonSegmentAuxContainer#MuonSegmentsAux." ]

   MuonAODList+=[ "xAOD::MuonSegmentContainer#NCB_MuonSegments" ]
   MuonAODList+=[ "xAOD::MuonSegmentAuxContainer#NCB_MuonSegmentsAux." ]

   # TrackParticles 
   MuonAODList+=[ "xAOD::TrackParticleContainer#MuonSpectrometerTrackParticles" ]
   MuonAODList+=[ "xAOD::TrackParticleAuxContainer#MuonSpectrometerTrackParticlesAux." ]
   MuonAODList+=[ "xAOD::TrackParticleContainer#MuonSpectrometerOnlyTrackParticles" ]
   MuonAODList+=[ "xAOD::TrackParticleAuxContainer#MuonSpectrometerOnlyTrackParticlesAux." ]

   MuonAODList+=[ "xAOD::TrackParticleContainer#MSonlyTracklets" ]
   MuonAODList+=[ "xAOD::TrackParticleAuxContainer#MSonlyTrackletsAux." ]
   MuonAODList+=[ "xAOD::VertexContainer#MSDisplacedVertex" ]
   MuonAODList+=[ "xAOD::VertexAuxContainer#MSDisplacedVertexAux." ]

   if rec.doTruth():
      # Truth Particle Container
      MuonAODList+=["xAOD::TruthParticleContainer#MuonTruthParticles"]
      MuonAODList+=["xAOD::TruthParticleAuxContainer#MuonTruthParticlesAux."]

      # Truth Segment Container
      MuonAODList+=["xAOD::MuonSegmentContainer#MuonTruthSegments"]
      MuonAODList+=["xAOD::MuonSegmentAuxContainer#MuonTruthSegmentsAux."]

   if muonRecFlags.prdToxAOD():
      MuonAODList += [ "xAOD::TrackMeasurementValidationContainer#MDT_DriftCircles"] 
      MuonAODList += [ "xAOD::TrackMeasurementValidationAuxContainer#MDT_DriftCirclesAux."] 
      MuonAODList += [ "xAOD::TrackMeasurementValidationContainer#RPC_Measurements"] 
      MuonAODList += [ "xAOD::TrackMeasurementValidationAuxContainer#RPC_MeasurementsAux."] 
      MuonAODList += [ "xAOD::TrackMeasurementValidationContainer#TGC_MeasurementsAllBCs"] 
      MuonAODList += [ "xAOD::TrackMeasurementValidationAuxContainer#TGC_MeasurementsAllBCsAux."] 
      MuonAODList += [ "xAOD::TrackMeasurementValidationContainer#CSC_Clusters"] 
      MuonAODList += [ "xAOD::TrackMeasurementValidationAuxContainer#CSC_ClustersAux."] 
      if muonRecFlags.doCreateClusters:
         MuonAODList += [ "xAOD::TrackMeasurementValidationContainer#RPC_Clusters"] 
         MuonAODList += [ "xAOD::TrackMeasurementValidationAuxContainer#RPC_ClustersAux."] 
         MuonAODList += [ "xAOD::TrackMeasurementValidationContainer#TGC_Clusters"] 
         MuonAODList += [ "xAOD::TrackMeasurementValidationAuxContainer#TGC_ClustersAux."] 

   if muonRecFlags.rpcRawToxAOD():
      MuonAODList += [ "xAOD::TrackMeasurementValidationContainer#RPC_RDO_Measurements"] 
      MuonAODList += [ "xAOD::TrackMeasurementValidationAuxContainer#RPC_RDO_MeasurementsAux."] 

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

   #trigger related info for offline DQA
   MuonESDList+=["Muon::TgcCoinDataContainer#TrigT1CoinDataCollection"]
   MuonESDList+=["Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionPriorBC"]
   MuonESDList+=["Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionNextBC"]
   MuonESDList+=["Muon::RpcCoinDataContainer#RPC_triggerHits"]
   MuonESDList+=["RpcSectorLogicContainer#RPC_SECTORLOGIC"]

   # Segments
   MuonESDList+=["Trk::SegmentCollection#MuonSegments"]
   MuonESDList+=["Trk::SegmentCollection#NCB_MuonSegments"]

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

   # commenting if-statement since mandatory for e.g. RPC calibration
   # Write out CSC, RPC, and MDT RDOs.
   # I'm wondering if we should be doing something in MuonCnvExample to ensure we have the right
   # configuration? See bug#59676
   if ( muonRecFlags.writeRDOs() or muonRecFlags.writeMdtRDOs() ):
      import MuonCnvExample.MuonCablingConfig
      MuonESDList += [ "MdtCsmContainer#MDTCSM" ]
   if ( muonRecFlags.writeRDOs() or muonRecFlags.writeCscRDOs() ):
      import MuonCnvExample.MuonCablingConfig
      MuonESDList += [ "CscRawDataContainer#CSCRDO" ] 
   if ( muonRecFlags.writeRDOs() or muonRecFlags.writeRpcRDOs() ):
      import MuonCnvExample.MuonCablingConfig
      MuonESDList += [ "RpcPadContainer#RPCPAD" ] 
