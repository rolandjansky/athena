# Job options for running on AODs produced by AtlFast
#
include( "JiveXML/JiveXML_jobOptionBase.py" )

theEventData2XML.DataTypes += ["JiveXML::ElectronRetriever/ElectronRetriever"]
theEventData2XML.DataTypes += ["JiveXML::PhotonRetriever/PhotonRetriever"]
theEventData2XML.DataTypes += ["JiveXML::TauJetRetriever/TauJetRetriever"]
theEventData2XML.DataTypes += ["JiveXML::MuonRetriever/MuonRetriever"]
theEventData2XML.DataTypes += ["JiveXML::TrackParticleRetriever/TrackParticleRetriever"]
theEventData2XML.DataTypes += ["JiveXML::BJetRetriever/BJetRetriever"]
theEventData2XML.DataTypes += ["JiveXML::ParticleJetRetriever/ParticleJetRetriever"]
theEventData2XML.DataTypes += ["JiveXML::MissingETRetriever/MissingETRetriever"]
theEventData2XML.DataTypes += ["JiveXML::TruthTrackRetriever/TruthTrackRetriever"]

from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__ElectronRetriever
theElectronRetriever = JiveXML__ElectronRetriever (name = "ElectronRetriever")
theElectronRetriever.StoreGateKey = "AtlfastElectronCollection"
ToolSvc += theElectronRetriever

from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__MuonRetriever
theMuonRetriever = JiveXML__MuonRetriever (name = "MuonRetriever")
theMuonRetriever.StoreGateKey = "AtlfastMuonCollection"
ToolSvc += theMuonRetriever

from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__PhotonRetriever
thePhotonRetriever = JiveXML__PhotonRetriever (name = "PhotonRetriever")
thePhotonRetriever.StoreGateKey = "AtlfastPhotonCollection"
ToolSvc += thePhotonRetriever

from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__TauJetRetriever
theTauJetRetriever = JiveXML__TauJetRetriever (name = "TauJetRetriever")
theTauJetRetriever.StoreGateKey = "AtlfastTauJetContainer"
ToolSvc += theTauJetRetriever

from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__BJetRetriever
theBJetRetriever = JiveXML__BJetRetriever (name = "BJetRetriever")
theBJetRetriever.StoreGateKey = "AtlfastBJetContainer"
ToolSvc += theBJetRetriever

from RecJiveXML.RecJiveXMLConf import JiveXML__MissingETRetriever
theMissingETRetriever = JiveXML__MissingETRetriever (name = "MissingETRetriever")
theMissingETRetriever.FavouriteMissingETCollection = "AtlfastMissingEt"
ToolSvc += theMissingETRetriever

from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__TrackParticleRetriever
theTrackParticleRetriever = JiveXML__TrackParticleRetriever (name = "TrackParticleRetriever")
theTrackParticleRetriever.StoreGateKey = "AtlfastTrackParticles"
ToolSvc += theTrackParticleRetriever
