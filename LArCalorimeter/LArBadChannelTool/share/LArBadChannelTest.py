BadChanFolder="/LAR/BadChannels/BadChannels"
TagPostfix="-UPD3-00"
FEBFolder = "/LAR/BadChannels/MissingFEBs"
GlobalTag = "COMCOND-ES1C-000-00"

from string import *
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_bytestream()

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-Comm-00-00-00"

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

#Get identifier mapping
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )

include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

theApp.EvtMax = 1

svcMgr.EventSelector.RunNumber         = 10000000
svcMgr.EventSelector.FirstEvent        = 1

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

BadChanTag=join(split(BadChanFolder, '/'),'') + TagPostfix
FEBTag=join(split(FEBFolder, '/'),'') + TagPostfix

DBConnection = "<dbConnection>sqlite://;schema=BadChannels.db;dbname=CONDBR2</dbConnection>"
DBBadChanFolder = BadChanFolder + "<tag>" + BadChanTag + "</tag>" + DBConnection
DBFebFolder     = FEBFolder     + "<tag>" + FEBTag     + "</tag>" + DBConnection

svcMgr.IOVDbSvc.Folders+=[DBBadChanFolder]
svcMgr.IOVDbSvc.Folders+=[DBFebFolder]

#svcMgr.IOVDbSvc.Folders+=["/LAR/BadChannels/BadChannels<tag>LARBadChannelsBadChannels-M6-01</tag><dbConnection>sqlite://;schema=BadChannels.db;dbname=CONDBR2</dbConnection>" ]
#svcMgr.IOVDbSvc.Folders+=["/LAR/BadChannels/MissingFEBs<tag>LARBadChannelsMissingFEBs-M6-01</tag><dbConnection>sqlite://;schema=BadChannels.db;dbname=CONDBR2</dbConnection>" ]
#conddb.addFolder("LAR","/LAR/BadChannels/BadChannels")
svcMgr.IOVDbSvc.GlobalTag=GlobalTag

from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelToolTest
theLArBadChannels=LArBadChannelToolTest()
#theLArBadChannels.FileName="bc_out.txt"
topSequence+=theLArBadChannels

from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
theLArBadChannelTool=LArBadChanTool()
theLArBadChannelTool.OutputLevel=DEBUG
theLArBadChannelTool.DumpCache = True
theLArBadChannelTool.CoolMissingFEBsFolder = "/LAR/BadChannels/MissingFEBs" # read missing FEBs folder
#theLArBadChannelTool.CoolMissingFEBsFolder = "" # deactivate missing FEBs folder
ToolSvc+=theLArBadChannelTool
