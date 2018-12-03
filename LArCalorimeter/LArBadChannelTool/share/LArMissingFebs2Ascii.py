if 'DBInstance' not in dir():
   DBInstance="CONDBR2"

if 'OutputFile' not in dir():
    OutputFile="mf_output.txt"
    
if 'RunNumber' not in dir():
   RunNumber=2147483647

if 'LBNumber' not in dir():
   LBNumber = 0


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

#Get identifier mapping
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )

include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

theApp.EvtMax = 1

svcMgr.EventSelector.RunNumber         = RunNumber
svcMgr.EventSelector.FirstLB           = LBNumber

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)


if "sqlite" in dir():
    conddb.addFolder("","/LAR/BadChannelsOfl/MissingFEBs<db>sqlite://;schema="+sqlite+";dbname="+DBInstance+"</db><tag>LARBadChannelsOflMissingFEBs-RUN2-UPD3-01</tag>")
else:
    conddb.addFolder("LAR_OFL","/LAR/BadChannelsOfl/MissingFEBs")#<tag>LARBadChannelsMissingFEBs-empty</tag>")
                 
svcMgr.IOVDbSvc.GlobalTag="CONDBR2-ES1PA-2017-04" 


from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
theLArBadChannelTool=LArBadChanTool()
theLArBadChannelTool.CoolFolder=""
theLArBadChannelTool.CoolMissingFEBsFolder="/LAR/BadChannelsOfl/MissingFEBs"
ToolSvc+=theLArBadChannelTool



from LArBadChannelTool.LArBadChannelToolConf import LArBadFeb2Ascii
theLArBadFeb2Ascii=LArBadFeb2Ascii()
theLArBadFeb2Ascii.FileName=OutputFile
topSequence+=theLArBadFeb2Ascii
