#Avoid multiple includes
include.block("RecJiveXML/RecJiveXML_DataTypes.py")

# Include the base options if the user didn't already do that
if not "theEventData2XML" in dir():
    include ("JiveXML/JiveXML_jobOptionBase.py")

theEventData2XML.DataTypes += ["JiveXML::JetRecJetRetriever/JetRecJetRetriever"]
theEventData2XML.DataTypes += ["JiveXML::MissingETRetriever/MissingETRetriever"]

## Configuration of the data retrievers can be done as follows:
## useful default settings for 13.0.x AODs
from RecJiveXML.RecJiveXMLConf import JiveXML__JetRecJetRetriever
theJetRecJetRetriever = JiveXML__JetRecJetRetriever (name = "JetRecJetRetriever")
#
theJetRecJetRetriever.FavouriteJetCollection="AntiKt4TopoEMJets"
### read all other jet collections:  
#theJetRecJetRetriever.OtherJetCollections=[]
### to select certain collections, apply a list like this: 
### keep 'old' data10 ESD names also (with 'H1') for backwards compatibility:
theJetRecJetRetriever.OtherJetCollections=["AntiKt6TopoEMJets","AntiKt4TopoJets","AntiKt6TopoJets"]
## NB Some 'AODJet' collections have broken cell/track links, don't access those with
## 'full' JetRecJetRetriever. AODJetRetriever will be ok with those.
# Prefered collection for jet-track association:
theJetRecJetRetriever.TrackCollection="CombinedInDetTracks"
## Prefered collection for jet-track association would be:
# theJetRecJetRetriever.TrackCollection="CombinedInDetTracks"
## but due to jet-track-association issue (wrong links), use instead default:
theJetRecJetRetriever.TrackCollection="Tracks"
## To write-out extended jet quality parameters:
#### This fails after xAOD migration ! Not sure how to recover. jpt 10Jan15
#### Retriever also forcing flag to 'False' now.
##theJetRecJetRetriever.WriteJetQuality = True
#
from RecJiveXML.RecJiveXMLConf import JiveXML__MissingETRetriever
theMissingETRetriever = JiveXML__MissingETRetriever (name = "MissingETRetriever")
#
theMissingETRetriever.FavouriteMissingETCollection="MET_RefFinal"
theMissingETRetriever.OtherMissingETCollections=["MET_TopoObj","MET_RefMuon","MET_Final","MET_Truth"]
#
ToolSvc += theJetRecJetRetriever
ToolSvc += theMissingETRetriever
#
