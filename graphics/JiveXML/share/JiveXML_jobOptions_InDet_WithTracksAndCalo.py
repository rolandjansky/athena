## temporary RTT jOs for rel19 migration.
## Added more data (calo and tracks),
## until RecExCommon works again. jpt 9Jan14

include ("InDetRecExample/jobOptions.py")

include( "JiveXML/JiveXML_jobOptionBase.py" )
include ("InDetJiveXML/InDetJiveXML_DataTypes.py")

#### from: CaloJiveXML_DataTypes.py:
theEventData2XML.DataTypes += ["JiveXML::CaloFCalRetriever/CaloFCalRetriever"]
theEventData2XML.DataTypes += ["JiveXML::CaloLArRetriever/CaloLArRetriever"]
theEventData2XML.DataTypes += ["JiveXML::CaloHECRetriever/CaloHECRetriever"]
theEventData2XML.DataTypes += ["JiveXML::CaloMBTSRetriever/CaloMBTSRetriever"]
theEventData2XML.DataTypes += ["JiveXML::CaloTileRetriever/CaloTileRetriever"]
theEventData2XML.DataTypes += ["JiveXML::CaloClusterRetriever/CaloClusterRetriever"]

### from: AnalysisJiveXML_DataTypes.py, but no Truth:
#Avoid multiple includes
include.block("AnalysisJiveXML/AnalysisJiveXML_DataTypes.py")

# Include the base options if the user didn't already do that
if not "theEventData2XML" in dir():
    include ("JiveXML/JiveXML_jobOptionBase.py")

theEventData2XML.DataTypes += ["JiveXML::ElectronRetriever/ElectronRetriever"]
theEventData2XML.DataTypes += ["JiveXML::PhotonRetriever/PhotonRetriever"]
theEventData2XML.DataTypes += ["JiveXML::TauJetRetriever/TauJetRetriever"]
theEventData2XML.DataTypes += ["JiveXML::MuonRetriever/MuonRetriever"]
theEventData2XML.DataTypes += ["JiveXML::TrackParticleRetriever/TrackParticleRetriever"]
theEventData2XML.DataTypes += ["JiveXML::BJetRetriever/BJetRetriever"]
theEventData2XML.DataTypes += ["JiveXML::AODJetRetriever/AODJetRetriever"]

# Optional: calorimeter clusters without cells
theEventData2XML.DataTypes += ["JiveXML::AODCaloClusterRetriever/AODCaloClusterRetriever"]

## Clusters from L2 trigger: 
theEventData2XML.DataTypes += ["JiveXML::TrigEMClusterRetriever/TrigEMClusterRetriever"]

# Under Development: TruthParticle displayed as CompositeParticles a la EV;
#theEventData2XML.DataTypes += ["JiveXML::CompositeParticleRetriever/CompositeParticleRetriever"]

# Truth particle retriever
theEventData2XML.DataTypes += ["JiveXML::TruthParticleRetriever/TruthParticleRetriever"]

# Configuration of the data retrievers can be done as follows:
from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__ElectronRetriever
theElectronRetriever = JiveXML__ElectronRetriever (name = "ElectronRetriever")
#theElectronRetriever.StoreGateKey = "ElectronAODCollection"
#theElectronRetriever.OutputLevel = DEBUG
ToolSvc += theElectronRetriever

## example how to switch on writting-out of HLT collections: 
from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__TrackParticleRetriever
theTrackParticleRetriever = JiveXML__TrackParticleRetriever (name = "TrackParticleRetriever")
## Note this flag is False by default
theTrackParticleRetriever.OtherTrackCollections = ["StacoTrackParticles","MooreTrackParticles"]
#theTrackParticleRetriever.DoWriteHLT = True
ToolSvc += theTrackParticleRetriever

## example how to switch on writting-out of HLT collections
## and select favourite and other collections 
from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__AODCaloClusterRetriever
theAODCaloClusterRetriever = JiveXML__AODCaloClusterRetriever (name = "AODCaloClusterRetriever")
## Note: if 'Other Collection' list is given, this flag is ignored
#theAODCaloClusterRetriever.DoWriteHLT = True
## default collection: Most Electrons have ElementLink to this: 
theAODCaloClusterRetriever.FavouriteClusterCollection="egClusterCollection"
## be more restrictive due to multiple collection problems.
## was:
#theAODCaloClusterRetriever.OtherClusterCollections=["LArClusterEMSofte","CombinedCluster","MuonClusterCollection","AllPhotonsClusters","LArClusterEMFrwd","CaloCalTopoCluster"]
## now just:
theAODCaloClusterRetriever.OtherClusterCollections=["CaloCalTopoCluster"]
ToolSvc += theAODCaloClusterRetriever

from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__TrigEMClusterRetriever
theTrigEMClusterRetriever = JiveXML__TrigEMClusterRetriever (name = "TrigEMClusterRetriever")
theTrigEMClusterRetriever.StoreGateKey="HLT_TrigT2CaloEgamma"
ToolSvc += theTrigEMClusterRetriever

from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__MuonRetriever
theMuonRetriever = JiveXML__MuonRetriever (name = "MuonRetriever")
#theMuonRetriever.OutputLevel=DEBUG
ToolSvc += theMuonRetriever

## example how to switch on writting-out of HLT collections
from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__TauJetRetriever
theTauJetRetriever = JiveXML__TauJetRetriever (name = "TauJetRetriever")
## Note this flag is False by default
#theTauJetRetriever.DoWriteHLT = True
ToolSvc += theTauJetRetriever

## Configuration of the data retrievers can be done as follows:
from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__AODJetRetriever
theAODJetRetriever = JiveXML__AODJetRetriever (name = "AODJetRetriever")
theAODJetRetriever.FavouriteJetCollection="AntiKt4TopoEMJets"
## If this list is not set, all other collections will be retrieved
theAODJetRetriever.OtherJetCollections=["AntiKt6TopoEMJets","AntiKt4TopoJets","AntiKt6TopoJets"]
##theAODJetRetriever.OtherJetCollections=[]
## To write-out jet quality parameters: 
#theAODJetRetriever.WriteJetQuality = True
ToolSvc += theAODJetRetriever

## Configuration of the data retrievers can be done as follows:
from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__BJetRetriever
theBJetRetriever = JiveXML__BJetRetriever (name = "BJetRetriever")
theBJetRetriever.FavouriteJetCollection="AntiKt4TopoEMJets"
## If this list is not set, all other collections will be retrieved
theBJetRetriever.OtherJetCollections=["AntiKt6TopoEMJets","AntiKt4TopoJets","AntiKt6TopoJets"]
ToolSvc += theBJetRetriever

## example how to switch on writting-out of HLT collections:
from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__TruthParticleRetriever
theTruthParticleRetriever = JiveXML__TruthParticleRetriever (name = "TruthParticleRetriever")
## Note this flag is False by default
theTruthParticleRetriever.StoreGateKey = "TruthEvent"
ToolSvc += theTruthParticleRetriever

include ("RecJiveXML/RecJiveXML_DataTypes.py")
include ("TrkJiveXML/TrkJiveXML_DataTypes.py")
theTrackRetriever.OtherTrackCollections = [""]
theMissingETRetriever.OtherMissingETCollections=[""]

### initial try for jOs to make Upgrade event xml-files smaller (target <15MB):
#    jpt 27Sep11
#
theEventData2XML.DataTypes.remove("JiveXML::SiSpacePointRetriever/SiSpacePointRetriever")
theEventData2XML.DataTypes.remove("JiveXML::SiClusterRetriever/SiClusterRetriever")
theEventData2XML.DataTypes.remove("JiveXML::PixelClusterRetriever/PixelClusterRetriever")
theEventData2XML.DataTypes.remove("JiveXML::TRTRetriever/TRTRetriever")
#
# for real data input:
theEventData2XML.DataTypes.remove("JiveXML::SCTRDORetriever/SCTRDORetriever")
theEventData2XML.DataTypes.remove("JiveXML::PixelRDORetriever/PixelRDORetriever")
#theCaloLArRetriever.LArlCellThreshold = 200.
#theCaloHECRetriever.HEClCellThreshold = 200.

### Default collection 'Tracks' has no spacepoints anymore,
### ie wrong tracks in Polyline mode, so switch to this collection:
theVertexRetriever.TrackCollection = "CombinedInDetTracks"
##
## also run xAOD:
include ("xAODJiveXML/xAODJiveXML_DataTypes.py")
