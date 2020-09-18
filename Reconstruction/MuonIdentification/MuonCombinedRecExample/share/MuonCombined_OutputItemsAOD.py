#AOD
MuonCombinedAODList = []

from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from InDetRecExample.InDetJobProperties import InDetFlags
from RecExConfig.RecFlags import rec as rec


if DetFlags.detdescr.Calo_on(): # FIXME - check if the objects below still make sense. EJWM
   MuonCombinedAODList+=["xAOD::CaloClusterContainer#MuonClusterCollection"]
   MuonCombinedAODList+=["xAOD::CaloClusterAuxContainer#MuonClusterCollectionAux."]
   MuonCombinedAODList+=["CaloClusterCellLinkContainer#MuonClusterCollection_links"]

if DetFlags.detdescr.Muon_on():
   excludedAuxData = '-caloExtension.-cellAssociation.-clusterAssociation'
   # Adding the xAOD content by default
   MuonCombinedAODList+=[ "xAOD::TrackParticleContainer#"+MuonCbKeys.CombinedFitParticles()]
   MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer#"+MuonCbKeys.CombinedFitParticles()+"Aux." + excludedAuxData ]
   MuonCombinedAODList+=[ "xAOD::TrackParticleContainer#"+MuonCbKeys.ExtrapolatedMSParticles() ]
   MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer#"+MuonCbKeys.ExtrapolatedMSParticles()+"Aux." + excludedAuxData ]
   MuonCombinedAODList+=[ "xAOD::TrackParticleContainer#"+MuonCbKeys.ExtrapolatedMSOnlyParticles() ]
   MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer#"+MuonCbKeys.ExtrapolatedMSOnlyParticles()+"Aux." + excludedAuxData ]
   MuonCombinedAODList+=[ "xAOD::TrackParticleContainer#"+MuonCbKeys.SpectrometerParticles() ]
   MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer#"+MuonCbKeys.SpectrometerParticles()+"Aux." + excludedAuxData ]
   MuonCombinedAODList+=[ "xAOD::MuonContainer#Muons" ]
   MuonCombinedAODList+=[ "xAOD::MuonAuxContainer#MuonsAux.-DFCommonMuonsTight.-DFCommonGoodMuon.-DFCommonMuonsMedium.-DFCommonMuonsLoose" ] # See note
   if InDetFlags.doR3LargeD0():
       MuonCombinedAODList+=[ "xAOD::MuonContainer#"+MuonCbKeys.FinalMuonsLargeD0() ]
       MuonCombinedAODList+=[ "xAOD::MuonAuxContainer#"+MuonCbKeys.FinalMuonsLargeD0()+"Aux.-DFCommonMuonsTight.-DFCommonGoodMuon.-DFCommonMuonsMedium.-DFCommonMuonsLoose" ] # See note
      
       ### Combined muon track particles
       MuonCombinedAODList+=[ "xAOD::TrackParticleContainer#Combined"+MuonCbKeys.FinalMuonsLargeD0()+"TrackParticles"]
       MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer#Combined"+MuonCbKeys.FinalMuonsLargeD0()+"TrackParticlesAux." + excludedAuxData]
       ### ME trackParticles
       MuonCombinedAODList+=[ "xAOD::TrackParticleContainer#ExtraPolated"+MuonCbKeys.FinalMuonsLargeD0()+"TrackParticles"]
       MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer#ExtraPolated"+MuonCbKeys.FinalMuonsLargeD0()+"TrackParticlesAux." + excludedAuxData]
       MuonCombinedAODList+=[ "xAOD::TrackParticleContainer#MSOnlyExtraPolated"+MuonCbKeys.FinalMuonsLargeD0()+"TrackParticles"]
       MuonCombinedAODList+=[ "xAOD::TrackParticleAuxContainer#MSOnlyExtraPolated"+MuonCbKeys.FinalMuonsLargeD0()+"TrackParticlesAux." + excludedAuxData]

   # Note: hack to remove derivation framework variables that are added by DRAW building and are supposed to be transient

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
