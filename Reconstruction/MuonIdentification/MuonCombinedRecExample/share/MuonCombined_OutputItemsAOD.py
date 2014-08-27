#AOD
MuonCombinedAODList = []

from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from RecExConfig.RecFlags import rec as rec

if DetFlags.detdescr.Calo_on(): # FIXME - check if the objects below still make sense. EJWM
   MuonCombinedAODList+=["CaloClusterContainer#MuonClusterCollection"]
   MuonCombinedAODList+=["CaloCellLinkContainer#MuonClusterCollection_Link"]

if DetFlags.detdescr.Muon_on():
   # Adding the xAOD content by default
   MuonCombinedAODList+=[ "xAOD::TrackParticleContainer_v1#"+MuonCbKeys.CombinedFitParticles()]
   MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer_v1#"+MuonCbKeys.CombinedFitParticles()+"Aux." ]
   MuonCombinedAODList+=[ "xAOD::TrackParticleContainer_v1#"+MuonCbKeys.ExtrapolatedMSParticles() ]
   MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer_v1#"+MuonCbKeys.ExtrapolatedMSParticles()+"Aux." ]
   MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer_v1#"+MuonCbKeys.SpectrometerParticles() ]
   MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer_v1#"+MuonCbKeys.SpectrometerParticles()+"Aux." ]
   MuonCombinedAODList+=[ "xAOD::MuonContainer_v1#Muons" ]
   MuonCombinedAODList+=[ "xAOD::MuonAuxContainer_v1#MuonsAux." ]

   MuonCombinedAODList+=["MuonCaloEnergyContainer#MuonCaloEnergyCollection"]

   ### stau
   MuonCombinedAODList+=[ "xAOD::TrackParticleContainer_v1#CombinedStauTrackParticles"]
   MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer_v1#CombinedStauTrackParticlesAux." ]
   MuonCombinedAODList+=[ "xAOD::TrackParticleContainer_v1#ExtrapolatedStauTrackParticles"]
   MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer_v1#ExtrapolatedStauTrackParticlesAux." ]
   MuonCombinedAODList+=[ "xAOD::MuonContainer_v1#Staus" ]
   MuonCombinedAODList+=[ "xAOD::MuonAuxContainer_v1#StausAux." ]

   #Slimmed Track Collection
   #MuonCombinedAODList+=["TrackCollection#MuonSlimmedTrackCollection"]


