###
# Atlantis specific jOs for P1
# --> Do not rely on RecExCommission setup (doJiveXML),
#     but setup things ourselves
###

### Enable Atlantis-flag (fake as this happens after we include RecExCommon)
from RecExConfig.RecFlags import rec
rec.doJiveXML.set_Value_and_Lock(True)

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
AlgoJiveXML.StreamTools += [
  JiveXML__StreamToFileTool(FileNamePrefix=OutputDirectory+"/JiveXML",
                            OutputLevel=AtlantisMsgLvl)
]
#Disable default tool
AlgoJiveXML.WriteToFile = False

### Online mode allows you to send events to
### AtlantiJava clients over a network connection
### - this is the same as AlgoJiveXML.OnlineMode = True
###   but allows to the set server port number
if not 'Port' in dir():
  Port=48965
from JiveXML.JiveXMLConf import JiveXML__XMLRPCStreamTool
AlgoJiveXML.StreamTools += [
  JiveXML__XMLRPCStreamTool(ServerPort=Port,OutputLevel=AtlantisMsgLvl)
]
#Disable default tool
AlgoJiveXML.OnlineMode = False

### Also stream events to the new external ONCRPC server
### If you test this from the command line, after setting up 
### AltasHLT, the server  can be started with 'jivexmlserver.exe'

#configure external ONCRCP server service to stream to the machine running our
#server instance and adopt the message level
if not 'ServerHost' in dir():
  ServerHost="pc-tdq-mon-31"
from JiveXML.JiveXMLConf import JiveXML__ExternalONCRPCServerSvc
ServiceMgr += JiveXML__ExternalONCRPCServerSvc("ExternalONCRPCServerSvc",
                                               Hostname=ServerHost,
                                               OutputLevel=AtlantisMsgLvl)
#And add a stream tool that stream using the current stream tag
from JiveXML.JiveXMLConf import JiveXML__StreamToServerTool
AlgoJiveXML.StreamTools += [
  JiveXML__StreamToServerTool(ServerService=ServiceMgr.ExternalONCRPCServerSvc,
                              StreamName=StreamName)
]

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
    #Switch on pulse shapes for Tiles
    ToolSvc.CaloTileRetriever.DoTileDigit = True
    ToolSvc.CaloTileRetriever.DoTileCellDetails = True
    #Switch on pulse shapes for MBTS
    ToolSvc.CaloMBTSRetriever.DoMBTSDigits = True
    #Also switch on LAR digits, but only for Calo streams
    if (Stream in ['L1Calo','L1CaloEM','MinBias']) and not Splash :
      include("EventDisplaysOnline/LArDigits_jobOptions.py")

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

### Finally, print setup if in debug mode
if AtlantisMsgLvl <= DEBUG:
  print "\n\n\t Atlantis setup\n",AlgoJiveXML,"\n\n"
