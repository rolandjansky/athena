#Avoid multiple includes
include.block("xAODJiveXML/xAODJiveXML_DataTypes.py")

# Include the base options if the user didn't already do that
if not "theEventData2XML" in dir():
    include ("JiveXML/JiveXML_jobOptionBase.py")

### those already produce valid xml (though minimal) output: 
theEventData2XML.DataTypes += ["JiveXML::xAODElectronRetriever/xAODElectronRetriever"]
theEventData2XML.DataTypes += ["JiveXML::xAODMuonRetriever/xAODMuonRetriever"]
theEventData2XML.DataTypes += ["JiveXML::xAODPhotonRetriever/xAODPhotonRetriever"]
### those don't (Atlantis parser crashes):
#theEventData2XML.DataTypes += ["JiveXML::xAODJetRetriever/xAODJetRetriever"]
#theEventData2XML.DataTypes += ["JiveXML::xAODTauRetriever/xAODTauRetriever"]
#theEventData2XML.DataTypes += ["JiveXML::xAODTrackParticleRetriever/xAODTrackParticleRetriever"]

# Configuration of the data retrievers can be done as follows:
from xAODJiveXML.xAODJiveXMLConf import JiveXML__xAODElectronRetriever
thexAODElectronRetriever = JiveXML__xAODElectronRetriever (name = "xAODElectronRetriever")
#thexAODElectronRetriever.StoreGateKey = "ElectronCollection"
thexAODElectronRetriever.OutputLevel = DEBUG
ToolSvc += thexAODElectronRetriever

from xAODJiveXML.xAODJiveXMLConf import JiveXML__xAODMuonRetriever
thexAODMuonRetriever = JiveXML__xAODMuonRetriever (name = "xAODMuonRetriever")
#thexAODMuonRetriever.StoreGateKey = "Muons"
thexAODMuonRetriever.OutputLevel = DEBUG
ToolSvc += thexAODMuonRetriever

from xAODJiveXML.xAODJiveXMLConf import JiveXML__xAODPhotonRetriever
thexAODPhotonRetriever = JiveXML__xAODPhotonRetriever (name = "xAODPhotonRetriever")
#thexAODPhotonRetriever.StoreGateKey = "PhotonCollection"
thexAODPhotonRetriever.OutputLevel = DEBUG
ToolSvc += thexAODPhotonRetriever
