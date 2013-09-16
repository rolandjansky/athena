include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )
include("LArCalibProcessing/LArCalib_MinimalSetup.py")

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



from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()


from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

from LArConditionsTest.LArConditionsTestConf import LArIdMapConvert
topSequence+=LArIdMapConvert()

from LArConditionsTest.LArConditionsTestConf import LArFebRodMapConvert
topSequence+=LArFebRodMapConvert()


theApp.EvtMax = 1

svcMgr.IOVDbSvc.GlobalTag   = "COMCOND-BLKPA-006-05"


from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
OutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","dummy.pool.root",
                                        ["AthenaAttributeList#/LAR/Identifier/OnOffIdMap",
                                         "AthenaAttributeList#/LAR/Identifier/CalibIdMap",
                                         "AthenaAttributeList#/LAR/Identifier/FebRodMap"
                                         ],
                                        ["LARIdentifierOnOffIdMap-000",
                                         "LARIdentifierCalibIdMap-000",
                                         "LARIdentifierFebRodMap-000",
                                         ],
                                        True)

OutputConditionsAlg.Run1     = beginRun


from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = True
svcMgr.IOVRegistrationSvc.SVFolder = False
svcMgr.IOVRegistrationSvc.userTags = False

svcMgr.IOVRegistrationSvc.OverrideNames += ["OnlineHashToOfflineId","OnlineHashToCalibIds", ]
types=[]
for i in range(len(svcMgr.IOVRegistrationSvc.OverrideNames)):
    types.append("Blob16M");
svcMgr.IOVRegistrationSvc.OverrideTypes += types;
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=LArIdentifierMaps.db;dbname=COMP200"

