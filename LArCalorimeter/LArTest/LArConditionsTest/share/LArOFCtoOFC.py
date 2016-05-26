# This should convert some samples OFC to another samples one, by adding NullsToAdd nulls 

if 'runNumber' not in dir():
    runNumber=2147483647

if not 'beginRun' in dir():
    beginRun=0

if 'HECShift' not in dir():
   HECShift=True

if 'NullsToAdd' not in dir():
   NullsToAdd=1

if 'doShape' not in dir():
   doShape=False

if 'InputDB' not in dir():
   InputDB="sqlite://;schema=freshConstants_repro_comp200.db;dbname=COMP200"

if 'InputFolder' not in dir():
   InputFolder="/LAR/ElecCalibOfl/OFC/PhysWave/RTM/4samples1phase"

if doShape and 'InputShapeFolder' not in dir():
   InputShapeFolder="/LAR/ElecCalibOfl/Shape/RTM/4samples1phase"

if 'InputKey' not in dir():
   InputKey="LArOFC_4_0_mu_picked"

if doShape and 'InputShapeKey' not in dir():
   InputShapeKey="LArShape_4_0_picked"

if 'InTagSuffix' not in dir():
   InTagSuffix="-mu-20-RUN2-UPD3-00"

if doShape and 'InShapeTagSuffix' not in dir():
   InShapeTagSuffix="-corr-RUN2-UPD3-00"

if 'OutputDB' not in dir():
   OutputDB="sqlite://;schema=freshConstants_repro.db;dbname=CONDBR2"

if 'OutputFolder' not in dir():
   OutputFolder="/LAR/ElecCalibOfl/OFC/PhysWave/RTM/5samples1phase"

if doShape and 'OutputShapeFolder' not in dir():
   OutputShapeFolder="/LAR/ElecCalibOfl/Shape/RTM/5samples1phase"

if 'OutputKey' not in dir():
   OutputKey="LArOFC"

if doShape and 'OutputShapeKey' not in dir():
   OutputShapeKey="LArShape"

if 'OutTagSuffix' not in dir():
   OutTagSuffix="_mu_20-repro-2012"

if doShape and 'OutShapeTagSuffix' not in dir():
   OutShapeTagSuffix="_mu_20-repro-2012"

if 'OutputPoolFile' not in dir():
   OutputPoolFile="LArOFC_5s_from_4s_"+str(beginRun)+".pool.root"

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
globalflags.DatabaseInstance.set_Value_and_Lock('CONDBR2')

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-GEO-21-00-00"

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

from string import *

conddb.addFolder("","<db>"+InputDB+"</db><key>"+InputKey+"</key>"+InputFolder+"<tag>"+join(split(InputFolder, '/'),'')+InTagSuffix+"</tag>")
if doShape:
  conddb.addFolder("","<db>"+InputDB+"</db><key>"+InputShapeKey+"</key>"+InputShapeFolder+"<tag>"+join(split(InputShapeFolder, '/'),'')+InShapeTagSuffix+"</tag>")

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()


from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

from LArConditionsTest.LArConditionsTestConf import LArOFCtoOFC
theLArOFCtoOFC=LArOFCtoOFC(OutputLevel=DEBUG)
theLArOFCtoOFC.inKey = InputKey
theLArOFCtoOFC.outKey = OutputKey
if doShape:
 theLArOFCtoOFC.doShape=doShape
 theLArOFCtoOFC.inShapeKey=InputShapeKey
 theLArOFCtoOFC.outShapeKey=OutputShapeKey

theLArOFCtoOFC.ShiftHEC = HECShift
theLArOFCtoOFC.NullsAdded = NullsToAdd
topSequence+=theLArOFCtoOFC


#from LArTools.LArToolsConf import LArSuperCellCablingTool
#ToolSvc+=LArSuperCellCablingTool(LArOnOffIdFolder="/LAR/Wrong",OutputLevel=DEBUG)

theApp.EvtMax = 1

#svcMgr.IOVDbSvc.GlobalTag   = "COMCOND-BLKPA-RUN1-07"
svcMgr.IOVDbSvc.GlobalTag   = "CONDBR2-BLKPA-2015-18"
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
OutObjects=["LArOFCComplete#"+OutputKey+"#"+OutputFolder]
OutTags=[join(split(OutputFolder, '/'),'') +OutTagSuffix]
if doShape:
   OutObjects+=["LArShapeComplete#"+OutputShapeKey+"#"+OutputShapeFolder]
   OutTags+=[join(split(OutputShapeFolder, '/'),'') +OutShapeTagSuffix]

OutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg",OutputPoolFile, OutObjects,OutTags, True)

OutputConditionsAlg.Run1     = beginRun


from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = True
svcMgr.IOVRegistrationSvc.SVFolder = False
#svcMgr.IOVRegistrationSvc.userTags = False
svcMgr.IOVDbSvc.dbConnection  = OutputDB

svcMgr.DetectorStore.Dump=True
