#AOD
MuonCombinedAODList = []

from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from RecExConfig.RecFlags import rec as rec

if DetFlags.detdescr.Calo_on(): # FIXME - check if the objects below still make sense. EJWM
   MuonCombinedAODList+=["CaloClusterContainer#MuonClusterCollection"]
   MuonCombinedAODList+=["CaloCellLinkContainer#MuonClusterCollection_Link"]

if DetFlags.detdescr.Muon_on():
   excludedAuxData = '-caloExtension.-cellAssociation.-clusterAssociation'
   # Adding the xAOD content by default
   MuonCombinedAODList+=[ "xAOD::TrackParticleContainer#"+MuonCbKeys.CombinedFitParticles()]
   MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer#"+MuonCbKeys.CombinedFitParticles()+"Aux." + excludedAuxData ]
   MuonCombinedAODList+=[ "xAOD::TrackParticleContainer#"+MuonCbKeys.ExtrapolatedMSParticles() ]
   MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer#"+MuonCbKeys.ExtrapolatedMSParticles()+"Aux." + excludedAuxData ]
   MuonCombinedAODList+=[ "xAOD::TrackParticleContainer#"+MuonCbKeys.SpectrometerParticles() ]
   MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer#"+MuonCbKeys.SpectrometerParticles()+"Aux." + excludedAuxData ]
   MuonCombinedAODList+=[ "xAOD::MuonContainer#Muons" ]
   MuonCombinedAODList+=[ "xAOD::MuonAuxContainer#MuonsAux." ]
   MuonCombinedAODList+=["MuonCaloEnergyContainer#MuonCaloEnergyCollection"]

   ### stau
   MuonCombinedAODList+=[ "xAOD::TrackParticleContainer#CombinedStauTrackParticles"]
   MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer#CombinedStauTrackParticlesAux." + excludedAuxData]
   MuonCombinedAODList+=[ "xAOD::TrackParticleContainer#ExtrapolatedStauTrackParticles"]
   MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer#ExtrapolatedStauTrackParticlesAux." + excludedAuxData]
   MuonCombinedAODList+=[ "xAOD::MuonContainer#Staus" ]
   MuonCombinedAODList+=[ "xAOD::MuonAuxContainer#StausAux." + excludedAuxData ]
   MuonCombinedAODList+=[ "xAOD::SlowMuonContainer#SlowMuons" ]
   MuonCombinedAODList+=[ "xAOD::SlowMuonAuxContainer#SlowMuonsAux." ]

   #Slimmed Track Collection
   #MuonCombinedAODList+=["TrackCollection#MuonSlimmedTrackCollection"]


