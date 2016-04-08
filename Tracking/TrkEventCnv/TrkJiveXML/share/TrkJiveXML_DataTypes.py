#Avoid multiple includes
include.block("TrkJiveXML/TrkJiveXML_DataTypes.py")

# Include the base options if the user didn't already do that
if not "theEventData2XML" in dir():
    include ("JiveXML/JiveXML_jobOptionBase.py")

theEventData2XML.DataTypes += ["JiveXML::VertexRetriever/VertexRetriever"]
 
# These options will retrieve any available collection of segments/tracks from storegate
theEventData2XML.DataTypes += ["JiveXML::SegmentRetriever/SegmentRetriever"]
theEventData2XML.DataTypes += ["JiveXML::TrackRetriever/TrackRetriever"]

### Configuration of the data retrievers can be done as follows:
from TrkJiveXML.TrkJiveXMLConf import JiveXML__TrackRetriever
theTrackRetriever = JiveXML__TrackRetriever (name = "TrackRetriever")
### Those still exist ? 
#theTrackRetriever.PriorityTrackCollection = "CombinedInDetTracks"
### Switch to the general tracks:
theTrackRetriever.PriorityTrackCollection = "Tracks"
### Select only assorted muon tracks:
theTrackRetriever.OtherTrackCollections = ["CombinedMuonTracks","MuonSpectrometerTracks","ConvertedStacoTracks","ConvertedMuIdCBTracks","CombinedInDetTracks","GSFTracks"]
### 
### The Event Filter track collections are not written to XML by default.
### To write them out, you must uncomment the following line:
#theTrackRetriever.DoWriteHLT = True
### switch residual data off:
theTrackRetriever.DoWriteResiduals = False
ToolSvc += theTrackRetriever

### Configuration of the data retrievers can be done as follows:
from TrkJiveXML.TrkJiveXMLConf import JiveXML__VertexRetriever
theVertexRetriever = JiveXML__VertexRetriever (name = "VertexRetriever")
## For AOD input:
#theVertexRetriever.IgnoreTrackAssociations = True
## New: for AOD input. But might not work ? Default is "Tracks" 
# theVertexRetriever.TrackCollection = "TrackParticleCandidate"
ToolSvc += theVertexRetriever
