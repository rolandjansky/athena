from AthenaCommon.DetFlags import DetFlags
from RecExConfig.RecFlags import rec
from MuonRecExample.MuonRecFlags import muonRecFlags
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys
from AthenaCommon.BeamFlags import jobproperties

MuonCombinedESDList = []

if DetFlags.detdescr.Muon_on():
  if muonRecFlags.doStandalone():    
    # ========================Muons===============================================
    MuonCombinedESDList+=["TrackCollection#"+MuonCbKeys.ExtrapolatedMSTracks()]
    MuonCombinedESDList+=["TrackCollection#"+MuonCbKeys.CombinedFitTracks()]
    MuonCombinedESDList+=["TrackCollection#"+MuonCbKeys.ExtrapolatedMSOnlyTracks()]
    MuonCombinedESDList+=["TrackCollection#EMEO_"+MuonCbKeys.ExtrapolatedMSOnlyTracks()]
  
    ### Combined muon track particles
    MuonCombinedESDList+=["Combined"+MuonCbKeys.FinalMuonsLargeD0()+"Tracks"]
    MuonCombinedESDList+=["ExtraPolated"+MuonCbKeys.FinalMuonsLargeD0()+"Tracks"]
    
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
