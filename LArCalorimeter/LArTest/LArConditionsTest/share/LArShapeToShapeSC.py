include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('geant4')
globalflags.InputFormat.set_Value_and_Lock('pool')

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-GEO-20-00-00"

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
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )

if 'runNumber' not in dir():
    runNumber=2147483647

if not 'beginRun' in dir():
    beginRun=0


from McEventSelector.McEventSelectorConf import McEventSelector
svcMgr += McEventSelector("EventSelector")
svcMgr.EventSelector.RunNumber = runNumber
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 0
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 1


conddb.addFolder("LAR_OFL","/LAR/ElecCalibMC/Shape")
conddb.addFolder("LAR_OFL","/LAR/IdentifierOfl/OnOffIdMap_SC<tag>LARIdentifierOflOnOffIdMap_SC-000</tag>")

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()


from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

from LArConditionsTest.LArConditionsTestConf import LArShapeToSCShape
topSequence+=LArShapeToSCShape(OutputLevel=DEBUG)


#from LArTools.LArToolsConf import LArSuperCellCablingTool
#ToolSvc+=LArSuperCellCablingTool(LArOnOffIdFolder="/LAR/Wrong",OutputLevel=DEBUG)

theApp.EvtMax = 1

svcMgr.IOVDbSvc.GlobalTag   = "OFLCOND-MC12-SIM-08"

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
OutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","dummy.pool.root",
                                        ["CondAttrListCollection#/LAR/ElecCalibSC/Shape",
                                         ],
                                        ["LARElecCalibSCShape-000",
                                         ],
                                        True)

OutputConditionsAlg.Run1     = beginRun


from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = True
svcMgr.IOVRegistrationSvc.SVFolder = False
#svcMgr.IOVRegistrationSvc.userTags = False

svcMgr.IOVRegistrationSvc.OverrideNames += ["Shape","ShapeDer","TimeOffset" ]
types=[]
for i in range(len(svcMgr.IOVRegistrationSvc.OverrideNames)):
    types.append("Blob16M");
svcMgr.IOVRegistrationSvc.OverrideTypes += types;
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=LArSCShapes.db;dbname=OFLP200"

svcMgr.DetectorStore.Dump=True
