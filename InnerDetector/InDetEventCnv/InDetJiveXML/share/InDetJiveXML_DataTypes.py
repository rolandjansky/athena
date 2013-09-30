#Avoid multiple includes
include.block("InDetJiveXML/InDetJiveXML_DataTypes.py")

# Include the base options if the user didn't already do that
if not "theEventData2XML" in dir():
    include ("JiveXML/JiveXML_jobOptionBase.py")

theEventData2XML.DataTypes += ["JiveXML::SiSpacePointRetriever/SiSpacePointRetriever"]
theEventData2XML.DataTypes += ["JiveXML::SiClusterRetriever/SiClusterRetriever"]
theEventData2XML.DataTypes += ["JiveXML::PixelClusterRetriever/PixelClusterRetriever"]
theEventData2XML.DataTypes += ["JiveXML::TRTRetriever/TRTRetriever"]

from AthenaCommon.DetFlags import DetFlags
 
if DetFlags.makeRIO.SCT_on():
   theEventData2XML.DataTypes += ["JiveXML::SCTRDORetriever/SCTRDORetriever"]

if DetFlags.makeRIO.pixel_on():
   theEventData2XML.DataTypes += ["JiveXML::PixelRDORetriever/PixelRDORetriever"]

### new datatype, not yet understood by AtlantisJava:
#theEventData2XML.DataTypes += ["JiveXML::BeamSpotRetriever/BeamSpotRetriever"]

#
#from InDetJiveXML.InDetJiveXMLConf import JiveXML__PixelClusterRetriever
#thePixelClusterRetriever = JiveXML__PixelClusterRetriever (name = "PixelClusterRetriever")
#thePixelClusterRetriever.OutputLevel = DEBUG
#ToolSvc += thePixelClusterRetriever
