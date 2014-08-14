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
theTruthParticleRetriever.StoreGateKey = "SpclMC"
ToolSvc += theTruthParticleRetriever

if not "JiveXML::TruthTrackRetriever/TruthTrackRetriever" in theEventData2XML.DataTypes:
    theEventData2XML.DataTypes += ["JiveXML::TruthTrackRetriever/TruthTrackRetriever"]
    from TruthJiveXML.TruthJiveXMLConf import JiveXML__TruthTrackRetriever
    theTruthTrackRetriever = JiveXML__TruthTrackRetriever (name = "TruthTrackRetriever")
    theTruthTrackRetriever.StoreGateKey = "GEN_AOD"
    ToolSvc += theTruthTrackRetriever

if not "JiveXML::VertexRetriever/VertexRetriever" in theEventData2XML.DataTypes:
    theEventData2XML.DataTypes += ["JiveXML::VertexRetriever/VertexRetriever"]
    from TrkJiveXML.TrkJiveXMLConf import JiveXML__VertexRetriever
    theVertexRetriever = JiveXML__VertexRetriever (name = "VertexRetriever")
    theVertexRetriever.DoWritePrimAndSecVertexOnly=True
    ToolSvc += theVertexRetriever

if not "JiveXML::MissingETRetriever/MissingETRetriever" in theEventData2XML.DataTypes:
    theEventData2XML.DataTypes += ["JiveXML::MissingETRetriever/MissingETRetriever"]
    from RecJiveXML.RecJiveXMLConf import JiveXML__MissingETRetriever
    theMissingETRetriever = JiveXML__MissingETRetriever (name = "MissingETRetriever")
    theMissingETRetriever.FavouriteMissingETCollection="MET_RefFinal"
    theMissingETRetriever.OtherMissingETCollections=["MET_Topo","MET_Base"]
    ToolSvc += theMissingETRetriever
