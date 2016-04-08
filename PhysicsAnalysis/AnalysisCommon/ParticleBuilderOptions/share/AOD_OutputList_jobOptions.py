from ParticleBuilderOptions.AODFlags import AODFlags
from AthenaCommon.DetFlags import DetFlags

print "ERROR ERROR ERROR ERROR ERROR ERROR"
print "ERROR AOD_OutputList_joboptins.py is OBSOLETE and will be REMOVED"
print "ERROR Please use RecExPers/RecoOutputAODList_jobOptions.py"
print "ERROR Or for a copy/merging StreamAOD.TakeItemsFromInput=True"

# list of output objects 

AOD_ItemList = []

# Event Info
AOD_ItemList+=["EventInfo#*"]

# Special MC AOD with HepMC Navigation
AOD_ItemList+=["McEventCollection#GEN_AOD"]

# TrackRecordCollection - muons at the entrance of the spectrometer
AOD_ItemList+=["TrackRecordCollection#MuonEntryLayerFilter"]

# slimmed Inner Detector tracks associated to Muons, Egammas and Taus
if AODFlags.egammaTrackSlimmer or AODFlags.MuonTrackSlimmer or AODFlags.TauTrackSlimmer:
   AOD_ItemList+=["TrackCollection#Tracks"]

# IParticles
AOD_ItemList += [ "egammaContainer#AtlfastElectronCollection" ]
AOD_ItemList += [ "egammaContainer#AtlfastPhotonCollection" ]

AOD_ItemList += [ "egammaContainer#ElectronAODCollection" ]
AOD_ItemList += [ "egammaContainer#PhotonAODCollection" ]

AOD_ItemList += [ "egDetailContainer#egDetailAOD" ]

AOD_ItemList+=["Analysis::MuonContainer#MuidMuonCollection"]
AOD_ItemList+=["Analysis::MuonContainer#StacoMuonCollection"]
AOD_ItemList+=["Analysis::MuonContainer#CaloMuonCollection"]
# Muon Spectrometer Slimmed Track Collection
AOD_ItemList+=["TrackCollection#MuonSlimmedTrackCollection"]

AOD_ItemList+=["Analysis::MuonContainer#AtlfastMuonCollection"]
AOD_ItemList+=["Analysis::MuonContainer#AtlfastNonIsoMuonCollection"]
AOD_ItemList+=["Analysis::MuonContainer#AtlfastMuonFCSCollection"]

AOD_ItemList+=["Analysis::TauJetContainer#Tau1P3PContainer"]
AOD_ItemList+=["Analysis::TauJetContainer#TauRecContainer"]
AOD_ItemList+=["Analysis::TauJetContainer#AtlfastTauJetContainer"]
AOD_ItemList+=["Analysis::TauJetContainer#AtlfastTauJet1p3pContainer"]

AOD_ItemList+=["Analysis::TauDetailsContainer#TauRecDetailsContainer"]
AOD_ItemList+=["Analysis::TauDetailsContainer#Tau1P3PDetailsContainer"]

## Jets
AOD_ItemList+=["JetCollection#AtlfastJetContainer"]

AOD_ItemList+=["TruthParticleContainer#SpclMC"]

AOD_ItemList += [ "JetKeyDescriptor#JetKeyMap" ]
AOD_ItemList += [ "JetCollection#*"]


# INav4MomAssocs: many-to-many associations of INavigable4Momentum objects
AOD_ItemList += [ "INav4MomAssocs#*" ]

# MissingET
AOD_ItemList+=["MissingET#*"]
AOD_ItemList+=["MissingETSig#METSig"]
AOD_ItemList+=["MissingETSigHypoContainer#EtMissHypoCollection"]
AOD_ItemList+=["MissingETSigObjContainer#EtMissObjCollection"]


# TrackParticle
AOD_ItemList+=["Rec::TrackParticleContainer#*"]
AOD_ItemList+=["TrackParticleTruthCollection#*"]

# MuonSpectrometer Shower and SegmentParticle
AOD_ItemList+=["Rec::MuonSpShowerContainer#*"]

#Segments
AOD_ItemList+=["Trk::SegmentCollection#ConvertedMBoySegments"]
AOD_ItemList+=["Trk::SegmentCollection#MooreSegments"]
AOD_ItemList+=["Trk::SegmentCollection#MuGirlSegments"]

# VxContainer
AOD_ItemList+=["VxContainer#VxPrimaryCandidate"]
AOD_ItemList+=["VxContainer#ConversionCandidate"]
AOD_ItemList+=["V0Container#V0Candidates"]

# cells 
AOD_ItemList+=["CaloCellContainer#AODCellContainer"]

# CaloClusters 
CaloClusterItemList=[]
CaloClusterKeys=[]

CaloClusterKeys+=["CaloCalTopoCluster"]
CaloClusterKeys+=["CombinedCluster"]
CaloClusterKeys+=["EMTopoCluster"]

# reshuffl em 
#CaloClusterKeys+=["LArClusterEM"]

CaloClusterKeys+=["egClusterCollection"]
#CaloClusterKeys+=["LArClusterEM35"]
#CaloClusterKeys+=["LArClusterEM37"]
#CaloClusterKeys+=["LArClusterEMgam"]
#CaloClusterKeys+=["LArClusterEMgam35"]
#CaloClusterKeys+=["LArClusterEMgam37"]


CaloClusterKeys+=["LArClusterEMSofte"]

if DetFlags.detdescr.Calo_on():
   CaloClusterKeys+=["MuonClusterCollection"]

for theKey in CaloClusterKeys:
   CaloClusterItemList+=["CaloClusterContainer#"+theKey]

# write the link only for egClusterColl
CaloClusterItemList+=["CaloCellLinkContainer#egClusterCollection_Link"]

if DetFlags.detdescr.Calo_on():
   CaloClusterItemList+=["CaloCellLinkContainer#MuonClusterCollection_Link"]

AOD_ItemList+=CaloClusterItemList

if DetFlags.detdescr.Calo_on():
   AOD_ItemList+=["MuonCaloEnergyContainer#MuonCaloEnergyCollection"]

AOD_ItemList+=["CaloClusterContainer#Tau1P3PPi0ClusterContainer"]

# TileCell
AOD_ItemList+=["TileMuContainer#TileMuObj"]
AOD_ItemList+=["TileCellContainer#MBTSContainer"]

# MC Truth for PileUp
AOD_ItemList+=["PileUpEventInfo#*"]

#obsolete AOD_ItemList+=[ "DetStatusMap#DetectorStatus" ]

if AODFlags.Trigger:
     include ("TriggerRelease/TriggerOutputItemListAOD_jobOptions.py")


# now merge the explicit AOD list to the one coming from ObjKeyStore
# (more and more will be taken from ObjKeyStore)
from AthenaCommon.KeyStore import CfgItemList, CfgKeyStore
theCKS=CfgKeyStore("KeyStore")
CILMergeAOD=CfgItemList("AODMerge",allowWildCard=True)
CILMergeAOD.add(AOD_ItemList)
CILMergeAOD.add(theCKS.streamAOD())

#now prune the itemlist from the object which are soft linked

# this doe not work CILMergeAOD.remove("DataVector<INavigable4Momentum>")
#this does
for k in CILMergeAOD('DataVector<INavigable4Momentum>'):
    CILMergeAOD.removeItem(k)

# The only items declared in StreamESD are those written here.
# Note the use of "=" instead of "+=" is deliberate
#StreamESD.ItemList = fullESDList()

AOD_ItemList = CILMergeAOD()

#print "ESD Output list is :",StreamESD.ItemList
