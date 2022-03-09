#AOD
MuonCombinedAODList = []

from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from InDetRecExample.InDetJobProperties import InDetFlags
from RecExConfig.RecFlags import rec as rec
from IsolationAlgs.IsoUpdatedTrackCones import iso_vars
from MuonCombinedRecExample.MuonCombinedAlgs import GetCombinedTrackParticles
if DetFlags.detdescr.Calo_on(): # FIXME - check if the objects below still make sense. EJWM
   MuonCombinedAODList+=["xAOD::CaloClusterContainer#MuonClusterCollection"]
   MuonCombinedAODList+=["xAOD::CaloClusterAuxContainer#MuonClusterCollectionAux."]
   MuonCombinedAODList+=["CaloClusterCellLinkContainer#MuonClusterCollection_links"]

if DetFlags.detdescr.Muon_on():
   excludedAuxData = '-clusterAssociation'
   common_flags = [ "DFCommonMuonsTight", "DFCommonGoodMuon","DFCommonMuonsMedium", "DFCommonMuonsLoose" ]
   excludedMuonAuxData = ".-"+".-".join(iso_vars() + common_flags)
   # Adding the xAOD content by default
   track_particles = GetCombinedTrackParticles()[0]
   for trk_cont in track_particles:
     MuonCombinedAODList += ["xAOD::TrackParticleContainer#"+trk_cont, "xAOD::TrackParticleAuxContainer#"+trk_cont+"Aux."+ excludedAuxData]   
  
   MuonCombinedAODList+=[ "xAOD::MuonContainer#Muons" ]
   MuonCombinedAODList+=[ "xAOD::MuonAuxContainer#MuonsAux" + excludedMuonAuxData ] # See note
   if muonRecFlags.runCommissioningChain():
      MuonCombinedAODList+=[ "xAOD::MuonContainer#EMEO_Muons" ]
      MuonCombinedAODList+=[ "xAOD::MuonAuxContainer#EMEO_MuonsAux" + excludedMuonAuxData ] # See note
      
   MuonCombinedAODList+=[ "xAOD::MuonContainer#"+MuonCbKeys.FinalMuonsLargeD0() ]
   MuonCombinedAODList+=[ "xAOD::MuonAuxContainer#"+MuonCbKeys.FinalMuonsLargeD0()+"Aux" + excludedMuonAuxData] # See note
  
   MuonCombinedAODList+=[ "xAOD::MuonContainer#Staus" ]
   MuonCombinedAODList+=[ "xAOD::MuonAuxContainer#StausAux." + excludedAuxData ]
   MuonCombinedAODList+=[ "xAOD::SlowMuonContainer#SlowMuons" ]
   MuonCombinedAODList+=[ "xAOD::SlowMuonAuxContainer#SlowMuonsAux." ]  
  
   #Slimmed Track Collection
   #MuonCombinedAODList+=["TrackCollection#MuonSlimmedTrackCollection"]
