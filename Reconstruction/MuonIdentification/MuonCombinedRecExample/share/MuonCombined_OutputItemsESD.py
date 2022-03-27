from AthenaCommon.DetFlags import DetFlags
from RecExConfig.RecFlags import rec
from MuonRecExample.MuonRecFlags import muonRecFlags
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys
from AthenaCommon.BeamFlags import jobproperties
from MuonCombinedRecExample.MuonCombinedAlgs import GetCombinedTrackParticles
MuonCombinedESDList = []

if DetFlags.detdescr.Muon_on():
  if muonRecFlags.doStandalone():    
    # ========================Muons===============================================
    cmb_trk_container = GetCombinedTrackParticles()[1] 
    MuonCombinedESDList+=["TrackCollection#"+ trk_col for trk_col in cmb_trk_container]   
    
  # muon track in muon spectrometer
  if rec.doTruth():
    #Combined Tracks
  
    MuonCombinedESDList+=["DetailedTrackTruthCollection#"+MuonCbKeys.ExtrapolatedMSTracks()+"Truth"]
    MuonCombinedESDList+=["DetailedTrackTruthCollection#"+MuonCbKeys.CombinedFitTracks()+"Truth"]
  
    MuonCombinedESDList+=["TrackParticleTruthCollection#"+MuonCbKeys.ExtrapolatedMSParticles()+"Truth"]
    MuonCombinedESDList+=["TrackParticleTruthCollection#"+MuonCbKeys.CombinedFitParticles()+"Truth"]
  

if muonCombinedRecFlags.doCosmicSplitTracks() and muonCombinedRecFlags.doAnyMuons():
   MuonCombinedESDList+=["TrackCollection#Combined_Tracks_split"]
   MuonCombinedESDList+=["TrackCollection#Tracks_split"]
   MuonCombinedESDList+=["Rec::TrackParticleContainer#TrackParticleCandidate_split"]

# Add AOD content
include("MuonCombinedRecExample/MuonCombined_OutputItemsAOD.py")
MuonCombinedESDList+=MuonCombinedAODList
