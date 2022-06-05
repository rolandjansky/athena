# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#########################################################################

# This should scale Ped+Ramp for SC 

if 'runNumber' not in dir():
    runNumber=2147483647

if not 'beginRun' in dir():
    beginRun=0

if 'InputDB' not in dir():
   InputDB="COOLOFL_LAR/OFLP200"

if 'InputFolder' not in dir():
   InputFolder="/LAR/IdentifierOfl/OnOffIdMap_SC"

if 'InTag' not in dir():
   InTag="LARIdentifierOflOnOffIdMap_SC-000"

if 'OutputDB' not in dir():
   OutputDB="sqlite://;schema=SCCalibMap.db;dbname=OFLP200"

if 'OutputFolder' not in dir():
   OutputFolder="/LAR/IdentifierOfl/CalibIdMap_SC"

if 'OutTag' not in dir():
   OutTag="LARIdentifierOflCalibIdMap_SC-000"

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-R2-2015-03-01-00"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()  #Switched off to avoid geometry
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

from McEventSelector.McEventSelectorConf import McEventSelector
svcMgr += McEventSelector("EventSelector")
svcMgr.EventSelector.RunNumber = runNumber
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 0
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 1

conddb.addFolder("","<db>"+InputDB+"</db>"+InputFolder+"<tag>"+InTag+"</tag>")

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()


from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

from LArCabling.LArCablingConf import LArSuperCellCablingTool
ToolSvc+=LArSuperCellCablingTool(LArOnOffIdFolder=InputFolder,OutputLevel=DEBUG)


from LArConditionsTest.LArConditionsTestConf import FixLArElecSCCalib
fix=FixLArElecSCCalib(OutputLevel=DEBUG)
fix.FixFlag = 2
topSequence+=fix

outObjects=[]
outTags=[]

outObjects += ["AthenaAttributeList#"+OutputFolder]
outTags+=[OutTag]

theApp.EvtMax = 1

svcMgr.IOVDbSvc.GlobalTag   = "CONDBR2-BLKPA-2015-18"
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg

theOutputConditionsAlgRec=OutputConditionsAlg("OutputConditionsAlgInline","dummy.root",
                                              outObjects,outTags,True)


svcMgr.IOVDbSvc.dbConnection  = OutputDB
svcMgr.IOVDbSvc.DBInstance=""

from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = True
svcMgr.IOVRegistrationSvc.SVFolder=False
svcMgr.IOVRegistrationSvc.OverrideNames += [ "OnlineHashToCalibIds" ]
types=[]
for i in range(len(svcMgr.IOVRegistrationSvc.OverrideNames)):
    types.append("Blob16M");
svcMgr.IOVRegistrationSvc.OverrideTypes += types;

svcMgr.DetectorStore.Dump=True

if 'poolcat' in dir():
  svcMgr.PoolSvc.WriteCatalog="xmlcatalog_file:"+poolcat

svcMgr.MessageSvc.defaultLimit=9999999999
svcMgr.MessageSvc.OutputLevel=INFO
