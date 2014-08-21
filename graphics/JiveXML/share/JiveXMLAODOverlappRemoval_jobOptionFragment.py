#
# job option fragment for running on AODs, which were converted
# using DefaultOverlapRemoval_jobOptions.py from
# PhysicsAnalysis/AnalysisCommon/ParticleBuilderOptions/share
# "User" collection after overlapp removal
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
theElectronRetriever.StoreGateKey = "UserElectronCollection"

from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__MuonRetriever
theMuonRetriever = JiveXML__MuonRetriever (name = "MuonRetriever")
theMuonRetriever.StoreGateKey = "UserStacoMuonCollection"

from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__PhotonRetriever
thePhotonRetriever = JiveXML__PhotonRetriever (name = "PhotonRetriever")
thePhotonRetriever.StoreGateKey = "UserPhotonCollection"

from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__TauJetRetriever
theTauJetRetriever = JiveXML__TauJetRetriever (name = "TauJetRetriever")
theTauJetRetriever.StoreGateKey = "UserTauJetContainer"

from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__ParticleJetRetriever
theParticleJetRetriever = JiveXML__ParticleJetRetriever (name = "ParticleJetRetriever")
theParticleJetRetriever.StoreGateKey = "UserParticleJetCollection"
