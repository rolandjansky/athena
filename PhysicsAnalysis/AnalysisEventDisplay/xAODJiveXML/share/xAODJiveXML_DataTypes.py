#Avoid multiple includes
include.block("xAODJiveXML/xAODJiveXML_DataTypes.py")

# Include the base options if the user didn't already do that
if not "theEventData2XML" in dir():
    include ("JiveXML/JiveXML_jobOptionBase.py")

theEventData2XML.DataTypes += ["JiveXML::xAODCaloClusterRetriever/xAODCaloClusterRetriever"]
theEventData2XML.DataTypes += ["JiveXML::xAODElectronRetriever/xAODElectronRetriever"]
theEventData2XML.DataTypes += ["JiveXML::xAODMissingETRetriever/xAODMissingETRetriever"]
theEventData2XML.DataTypes += ["JiveXML::xAODMuonRetriever/xAODMuonRetriever"]
theEventData2XML.DataTypes += ["JiveXML::xAODPhotonRetriever/xAODPhotonRetriever"]
theEventData2XML.DataTypes += ["JiveXML::xAODJetRetriever/xAODJetRetriever"]
theEventData2XML.DataTypes += ["JiveXML::xAODTauRetriever/xAODTauRetriever"]
theEventData2XML.DataTypes += ["JiveXML::xAODTrackParticleRetriever/xAODTrackParticleRetriever"]
theEventData2XML.DataTypes += ["JiveXML::xAODVertexRetriever/xAODVertexRetriever"]

###### new for 2014: First retriever for xAOD;
# Configuration of the data retrievers can be done as follows:
from xAODJiveXML.xAODJiveXMLConf import JiveXML__xAODElectronRetriever
thexAODElectronRetriever = JiveXML__xAODElectronRetriever (name = "xAODElectronRetriever")
#thexAODElectronRetriever.StoreGateKey = "ElectronCollection"
##thexAODElectronRetriever.OutputLevel = DEBUG
ToolSvc += thexAODElectronRetriever

from xAODJiveXML.xAODJiveXMLConf import JiveXML__xAODMissingETRetriever
thexAODMissingETRetriever = JiveXML__xAODMissingETRetriever (name = "xAODMissingETRetriever")
#thexAODMissingETRetriever.StoreGateKey = "MET_RefFinal"
thexAODMissingETRetriever.FavouriteMETCollection="MET_Reference_AntiKt4EMTopo"
## If this list is not set, all other collections will be retrieved
thexAODMissingETRetriever.OtherMETCollections=["MET_Calo","MET_LocHadTopo","MET_Core_AntiKt4LCTopo"]
#### to request all collections:
##thexAODMissingETRetriever.OtherMETCollections=[]
##thexAODMissingETRetriever.OutputLevel = DEBUG
ToolSvc += thexAODMissingETRetriever

from xAODJiveXML.xAODJiveXMLConf import JiveXML__xAODMuonRetriever
thexAODMuonRetriever = JiveXML__xAODMuonRetriever (name = "xAODMuonRetriever")
#thexAODMuonRetriever.StoreGateKey = "Muons"
##thexAODMuonRetriever.OutputLevel = DEBUG
ToolSvc += thexAODMuonRetriever

from xAODJiveXML.xAODJiveXMLConf import JiveXML__xAODPhotonRetriever
thexAODPhotonRetriever = JiveXML__xAODPhotonRetriever (name = "xAODPhotonRetriever")
#thexAODPhotonRetriever.StoreGateKey = "PhotonCollection"
##thexAODPhotonRetriever.OutputLevel = DEBUG
ToolSvc += thexAODPhotonRetriever

from xAODJiveXML.xAODJiveXMLConf import JiveXML__xAODJetRetriever
thexAODJetRetriever = JiveXML__xAODJetRetriever (name = "xAODJetRetriever")
thexAODJetRetriever.FavouriteJetCollection="AntiKt4EMPFlowJets"
## If this list is not set, all other collections will be retrieved
#thexAODJetRetriever.OtherJetCollections=["AntiKt4EMTopoJets", "AntiKt4LCTopoJets", "AntiKt4TruthJets", "AntiKt10LCTopoJets"]
#thexAODJetRetriever.OutputLevel = VERBOSE
ToolSvc += thexAODJetRetriever

from xAODJiveXML.xAODJiveXMLConf import JiveXML__xAODTauRetriever
thexAODTauRetriever = JiveXML__xAODTauRetriever (name = "xAODTauRetriever")
#thexAODTauRetriever.StoreGateKey = "TauRecContainer"
##thexAODTauRetriever.OutputLevel = DEBUG
ToolSvc += thexAODTauRetriever

from xAODJiveXML.xAODJiveXMLConf import JiveXML__xAODTrackParticleRetriever
thexAODTrackParticleRetriever = JiveXML__xAODTrackParticleRetriever (name = "xAODTrackParticleRetriever")
thexAODTrackParticleRetriever.StoreGateKey = "InDetTrackParticles"
thexAODTrackParticleRetriever.OtherTrackCollections = ["CombinedMuonTrackParticles","GSFTrackParticles"]
## To retriever any track collection (incl muons):
#thexAODTrackParticleRetriever.OtherTrackCollections = []
##thexAODTrackParticleRetriever.OutputLevel = DEBUG
ToolSvc += thexAODTrackParticleRetriever

from xAODJiveXML.xAODJiveXMLConf import JiveXML__xAODVertexRetriever
thexAODVertexRetriever = JiveXML__xAODVertexRetriever (name = "xAODVertexRetriever")
thexAODVertexRetriever.PrimaryVertexCollection = "PrimaryVertices"
thexAODVertexRetriever.SecondaryVertexCollection = "BTagging_AntiKt2TrackSecVtx"
##thexAODVertexRetriever.OutputLevel = DEBUG
ToolSvc += thexAODVertexRetriever

from xAODJiveXML.xAODJiveXMLConf import JiveXML__xAODCaloClusterRetriever
thexAODCaloClusterRetriever = JiveXML__xAODCaloClusterRetriever (name = "xAODCaloClusterRetriever")
thexAODCaloClusterRetriever.FavouriteClusterCollection = "egammaClusters"
thexAODCaloClusterRetriever.OtherClusterCollections = ["CaloCalTopoClusters"]
##thexAODCaloClusterRetriever.OutputLevel = DEBUG
ToolSvc += thexAODCaloClusterRetriever

