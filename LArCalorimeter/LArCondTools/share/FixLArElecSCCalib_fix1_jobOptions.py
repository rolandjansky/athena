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
   InputFolder="/LAR/ElecCalibMCSC/Pedestal"

if 'InputKey' not in dir():
   InputKey="Pedestal_orig"

if 'InTag' not in dir():
    InTag="LARElecCalibMCSCPedestal-000"

if 'InputFolderRamp' not in dir():
   InputFolderRamp="/LAR/ElecCalibMCSC/Ramp"

if 'InputKeyRamp' not in dir():
   InputKeyRamp="Ramp_orig"

if 'InTagRamp' not in dir():
    InTagRamp="LARElecCalibMCSCRamp-mV-LSB"

if 'OutputDB' not in dir():
   OutputDB="sqlite://;schema=newConstants.db;dbname=OFLP200"

if 'OutputFolder' not in dir():
   OutputFolder="/LAR/ElecCalibMCSC/Pedestal"

if 'OutputKey' not in dir():
   OutputKey="Pedestal"

if 'OutTag' not in dir():
   OutTag="LARElecCalibMCSCPedestal-000_scaled"

if 'OutputFolderRamp' not in dir():
   OutputFolderRamp="/LAR/ElecCalibMCSC/Ramp"

if 'OutputKeyRamp' not in dir():
   OutputKeyRamp="Ramp"

if 'OutTagRamp' not in dir():
   OutTagRamp="LARElecCalibMCSCRamp-mV-LSB_scaled"

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

conddb.addFolder("","<db>"+InputDB+"</db>"+InputFolder+"<tag>"+InTag+"</tag><key>Pedestal_in</key>")
conddb.addFolder("","<db>"+InputDB+"</db>"+InputFolderRamp+"<tag>"+InTagRamp+"</tag><key>Ramp_in</key>")

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()


from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

from LArRecUtils.LArRecUtilsConf import LArFlatConditionSvc
if not hasattr(svcMgr,"LArFlatConditionSvc"):
       svcMgr+=LArFlatConditionSvc()
       svcMgr.ProxyProviderSvc.ProviderNames += [ "LArFlatConditionSvc" ]
       svcMgr.LArFlatConditionSvc.DoSuperCells=True
       svcMgr.LArFlatConditionSvc.PedestalSCInput="Pedestal_in"
       svcMgr.LArFlatConditionSvc.PedestalSCOutput=InputKey
       svcMgr.LArFlatConditionSvc.RampSCInput="Ramp_in"
       svcMgr.LArFlatConditionSvc.RampSCOutput=InputKeyRamp

from LArConditionsTest.LArConditionsTestConf import FixLArElecSCCalib
fix=FixLArElecSCCalib(OutputLevel=DEBUG)
fix.FixFlag = 1
fix.FixFactor = 0.9
topSequence+=fix

outObjects=[]
outTags=[]

outObjects += ["CondAttrListCollection#/LAR/ElecCalibMCSC/Pedestal"]
outTags+=[OutTag]
outObjects += ["CondAttrListCollection#/LAR/ElecCalibMCSC/Ramp"]
outTags+=[OutTagRamp]

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
svcMgr.IOVRegistrationSvc.OverrideNames += [ "Pedestal","PedestalRMS","OFCa"   ,"OFCb"    ,"TimeOffset","RampVec","Shape","ShapeDer","HVScaleCorr","MphysOverMcal","DAC2uA","uA2MeV"]
types=[]
for i in range(len(svcMgr.IOVRegistrationSvc.OverrideNames)):
    types.append("Blob16M");
svcMgr.IOVRegistrationSvc.OverrideTypes += types;

svcMgr.DetectorStore.Dump=True

if 'poolcat' in dir():
  svcMgr.PoolSvc.WriteCatalog="xmlcatalog_file:"+poolcat

svcMgr.MessageSvc.defaultLimit=9999999999
