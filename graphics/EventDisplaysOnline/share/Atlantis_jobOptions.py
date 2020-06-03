###
# Atlantis specific jOs for P1
# --> Do not rely on RecExCommission setup (doJiveXML),
#     but setup things ourselves
###

### Enable Atlantis-flag (fake as this happens after we include RecExCommon)
from RecExConfig.RecFlags import rec
rec.doJiveXML.set_Value_and_Lock(True)  ## Steered by graphics/JiveXML/share/JiveXML_RecEx_Config.py

### Setup output level
if not 'AtlantisMsgLvl' in dir():
  AtlantisMsgLvl=WARNING

### import and create top sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

### Create a new algorithm 
from JiveXML.JiveXMLConf import JiveXML__AlgoJiveXML
AlgoJiveXML = JiveXML__AlgoJiveXML(name="AlgoJiveXML",OutputLevel=AtlantisMsgLvl)

### Set the atlas release version
import os
AlgoJiveXML.AtlasRelease = os.environ["AtlasVersion"]

### Write the event data to disk and to a server
### - this is the same as AlgoJiveXML.WriteToFile = True
###   but allows to set the StreamName as prefix
from JiveXML.JiveXMLConf import JiveXML__StreamToFileTool
StreamToFileTool = JiveXML__StreamToFileTool(
        FileNamePrefix="%s/.Unknown/JiveXML" % OutputDirectory,
        OutputLevel=AtlantisMsgLvl)
ToolSvc += StreamToFileTool
AlgoJiveXML.StreamTools += [ StreamToFileTool ]
#Disable default tool
AlgoJiveXML.WriteToFile = False

### Online mode allows you to send events to
### AtlantisJava clients over a network connection
### - this is the same as AlgoJiveXML.OnlineMode = True
###   but allows to the set server port number
#if 'Port' in dir():
#    from JiveXML.JiveXMLConf import JiveXML__XMLRPCStreamTool
#    AlgoJiveXML.StreamTools += [
#      JiveXML__XMLRPCStreamTool(ServerPort=Port,OutputLevel=AtlantisMsgLvl)
#    ]
#Disable default tool
AlgoJiveXML.OnlineMode = False

### Also stream events to the new external ONCRPC server
### If you test this from the command line, after setting up 
### AltasHLT, the server  can be started with 'jivexmlserver.exe'

#configure external ONCRCP server service to stream to the machine running our
#server instance and adopt the message level
if not 'ServerHost' in dir():
  ServerHost="pc-tdq-mon-29"
from JiveXML.JiveXMLConf import JiveXML__ExternalONCRPCServerSvc
ServiceMgr += JiveXML__ExternalONCRPCServerSvc("ExternalONCRPCServerSvc",
                                               Hostname=ServerHost,
                                               OutputLevel=AtlantisMsgLvl)
#And add a stream tool that stream using the current stream tag
from JiveXML.JiveXMLConf import JiveXML__StreamToServerTool
StreamToServerTool = JiveXML__StreamToServerTool(
        OutputLevel=AtlantisMsgLvl,
        ServerService=ServiceMgr.ExternalONCRPCServerSvc,
	StreamName="Unknown")
ToolSvc += StreamToServerTool
AlgoJiveXML.StreamTools += [ StreamToServerTool ]

### Enable this to recreate the geometry XML files for Atlantis
AlgoJiveXML.WriteGeometry = False

### add the AlgoJiveXML
topSequence += AlgoJiveXML

### add a dummy flag so that AlgoJiveXML won't be reinstantiated
theEventData2XML=AlgoJiveXML

### Now add the data retrievers for sub-detectors
if DetFlags.detdescr.ID_on() :
    include ( "JiveXML/DataTypes_InDet.py" )

if DetFlags.detdescr.Calo_on():
  include ( "JiveXML/DataTypes_Calos.py" )
  if not rec.doHeavyIon:
    #Switch on pulse shapes for Tiles
    ToolSvc.CaloTileRetriever.DoTileDigit = True
    ToolSvc.CaloTileRetriever.DoTileCellDetails = True
    #Switch on pulse shapes for MBTS
    ToolSvc.CaloMBTSRetriever.DoMBTSDigits = True
    #Also switch on LAR digits, but only for Calo streams

if DetFlags.detdescr.Muon_on() :
    include ( "JiveXML/DataTypes_Muons.py" )

#Switch on trigger if trig data is there 
if rec.doTrigger():
  include ( "JiveXML/DataTypes_Trig.py" )
  #Special options for ACR
  from TrigJiveXML.TrigJiveXMLConf import JiveXML__EmTauROIRetriever
  ToolSvc += JiveXML__EmTauROIRetriever (name = "EmTauROIRetriever",readCPM=True)
  from TrigJiveXML.TrigJiveXMLConf import JiveXML__JetROIRetriever
  ToolSvc += JiveXML__JetROIRetriever (name = "JetROIRetriever", readJEM = True)

### Always use this one
include ( "JiveXML/DataTypes_Reco.py" )

theJetRecJetRetriever.OtherJetCollections = []
#theMissingETRetriever.OtherMissingETCollections = []

## ================== Edit: 25-02-2015 by sjiggins - Do not limit additional Track Collection ========================                  
include ( "JiveXML/DataTypes_Trig.py" ) # 25-02-2015 Change made by sjiggins to allow doTriggers to float
#include ("xAODJiveXML/xAODJiveXML_DataTypes.py") # 04-04/15 by sjiggins
#### jpthomas 4Jun15: Added correct muon track collection, but still also retrieve MS-only too:
ToolSvc.TrackRetriever.OtherTrackCollections = ["CombinedMuonTracks","MuonSpectrometerTracks"] 
## ===================================================================================================================

## ========================== Edit: Beam Splash reduction flags ====================================                      
#topSequence.theEventData2XML.DataTypes.remove("JiveXML::SiSpacePointRetriever/SiSpacePointRetriever");       
##topSequence.theEventData2XML.DataTypes.remove("JiveXML::SiClusterRetrieverSiClusterRetriever");
##topSequence.theEventData2XML.DataTypes.remove("JiveXML::PixelClusterRetriever/PixelClusterRetriever"); #Solved by TrigJiveXML checkout
#topSequence.theEventData2XML.DataTypes.remove("JiveXML::TRTRetriever/TRTRetriever");                              
##topSequence.theEventData2XML.DataTypes.remove("JiveXML::TrigSiSpacePointRetrieverTrigSiSpacePointRetriever");  
#topSequence.theEventData2XML.DataTypes.remove("JiveXML::SCTRDORetriever/SCTRDORetriever");                       
#topSequence.theEventData2XML.DataTypes.remove("JiveXML::MdtPrepDataRetriever/MdtPrepDataRetriever");              

#topSequence.CaloLArRetriever.LArlCellThreshold = 500.;
#topSequence.CaloHECRetriever.HEClCellThreshold = 500.;                                                                    

#ToolSvc.CaloLArRetriever.LArlCellThreshold = 500.;
#ToolSvc.CaloHECRetriever.HEClCellThreshold = 500.;


#ToolSvc.TrackRetriever.OtherTrackCollections = [];                                   
#ToolSvc.xAODTrackParticleRetriever.OtherTrackCollections = []                                                              
## ======================================================================================= 

if rec.doHeavyIon:
  #topSequence.AlgoJiveXML.DataTypes.remove("JiveXML::V0CandidateRetriever/V0CandidateRetriever") 
  #topSequence.AlgoJiveXML.DataTypes.remove("JiveXML::TruthTrackRetriever/TruthTrackRetriever") 
  #topSequence.AlgoJiveXML.DataTypes.remove("JiveXML::TruthMuonTrackRetriever/TruthMuonTrackRetriever") 
  topSequence.AlgoJiveXML.DataTypes.remove("JiveXML::SiSpacePointRetriever/SiSpacePointRetriever") 
  topSequence.AlgoJiveXML.DataTypes.remove("JiveXML::SiClusterRetriever/SiClusterRetriever") 
  topSequence.AlgoJiveXML.DataTypes.remove("JiveXML::PixelClusterRetriever/PixelClusterRetriever") 
  topSequence.AlgoJiveXML.DataTypes.remove("JiveXML::TRTRetriever/TRTRetriever") 
  topSequence.AlgoJiveXML.DataTypes.remove("JiveXML::CaloClusterRetriever/CaloClusterRetriever") 
  topSequence.AlgoJiveXML.DataTypes.remove("JiveXML::PixelRDORetriever/PixelRDORetriever")
  topSequence.AlgoJiveXML.DataTypes.remove("JiveXML::SCTRDORetriever/SCTRDORetriever")

  ToolSvc.TrackRetriever.PriorityTrackCollection = "Tracks" 
  ToolSvc.TrackRetriever.OtherTrackCollections = ["ConvertedStacoTracks","ConvertedMuIdCBTracks"] 
  
  ToolSvc.TrackRetriever.DoHitsDetails = False 

### Finally, print setup if in debug mode
if AtlantisMsgLvl <= DEBUG:
  print "\n\n\t Atlantis setup\n",AlgoJiveXML,"\n\n"
