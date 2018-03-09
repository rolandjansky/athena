#No input file -> use MC event selector
if 'DBInstance' not in dir():
    DBInstance="CONDBR2"

if "InputFile" not in dir():
    InputFile="mf_input.txt"

if "TagPostfix" not in dir():
    TagPostfix="-RUN2-UPD3-01"

if "Folder" not in dir():
    Folder = "/LAR/BadChannelsOfl/MissingFEBs"

if "IOVBeginRun" not in dir():
    IOVBeginRun=140000

if "IOVBeginLB" not in dir():
    IOVBeginLB=0

if "IOVEndRun" not in dir():
    IOVEndRun=-1

if "IOVEndLB" not in dir():
    IOVEndLB=-1


if "sqlite" not in dir():
    sqlite="MissingFEBs.db"
    
from string import *
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.GlobalFlags import  globalflags
globalflags.DataSource="data"
globalflags.InputFormat="bytestream"
	
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-R2-2015-04-00-00"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

#Get identifier mapping
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )

include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

#include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )

theApp.EvtMax = 1

svcMgr.EventSelector.RunNumber         = IOVBeginRun
svcMgr.EventSelector.FirstEvent        = 1
#svcMgr.EventSelector.InitialTimeStamp  = 0
#svcMgr.EventSelector.TimeStampInterval = 5
svcMgr.IOVDbSvc.GlobalTag="CONDBR2-ES1PA-2017-04"


## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

theApp.EvtMax=1

#Thats the registration algo
from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelDBAlg
theLArDBAlg=LArBadChannelDBAlg()
theLArDBAlg.WritingMode = 1
theLArDBAlg.FEBFolder=Folder
topSequence += theLArDBAlg

from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
theLArBadChannelTool=LArBadChanTool()
theLArBadChannelTool.ReadFromASCII=True
theLArBadChannelTool.CoolFolder=""

theLArBadChannelTool.CoolMissingFEBsFolder = ""#FEBFolder
theLArBadChannelTool.FEBfile = InputFile
#theLArBadChannelTool.WriteEmptyFolders = False
theLArBadChannelTool.OutputLevel=DEBUG
ToolSvc+=theLArBadChannelTool

OutputList=[ "AthenaAttributeList#"+Folder ]
FEBTag=join(split(Folder, '/'),'') + TagPostfix
OutputTagList=[FEBTag]

WriteIOV=True
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
theOutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","dummy.pool.root",
					   OutputList,OutputTagList,WriteIOV)

theOutputConditionsAlg.Run1 = IOVBeginRun
theOutputConditionsAlg.LB1 = IOVBeginLB

if (IOVEndRun > 0 ) and (IOVEndLB >= 0):
   theOutputConditionsAlg.Run2 = IOVEndRun
   theOutputConditionsAlg.LB2 = IOVEndLB

svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema="+sqlite+";dbname="+DBInstance
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = False

svcMgr.DetectorStore.Dump=True


svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = WARNING)


svcMgr.MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"
