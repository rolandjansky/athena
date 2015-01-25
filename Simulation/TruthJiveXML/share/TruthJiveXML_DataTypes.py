#Avoid multiple includes
include.block("TruthJiveXML/TruthJiveXML_DataTypes.py")

if not "AtlasExtrapolator" in dir():
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    AtlasExtrapolator = AtlasExtrapolator('AtlasExtrapolator')
    ToolSvc+=AtlasExtrapolator

# Include the base options if the user didn't already do that
if not "theEventData2XML" in dir():
    include ("JiveXML/JiveXML_jobOptionBase.py")

if not "JiveXML::TruthTrackRetriever/TruthTrackRetriever" in theEventData2XML.DataTypes:
    theEventData2XML.DataTypes += ["JiveXML::TruthTrackRetriever/TruthTrackRetriever"]

theEventData2XML.DataTypes += ["JiveXML::TruthMuonTrackRetriever/TruthMuonTrackRetriever"]

# Configuration of the data retrievers can be done as follows:
from TruthJiveXML.TruthJiveXMLConf import JiveXML__TruthTrackRetriever
theTruthTrackRetriever = JiveXML__TruthTrackRetriever (name = "TruthTrackRetriever")
#theTruthTrackRetriever.StoreGateKey = "TruthEvent"
ToolSvc += theTruthTrackRetriever
