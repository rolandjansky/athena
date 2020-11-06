import AthenaCommon.AtlasUnixStandardJob

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'geant4'
globalflags.InputFormat = 'pool'
globalflags.print_JobProperties()
globalflags.DetDescrVersion = "ATLAS-IBL-02-00-00"


from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.pixel_setOn()
DetFlags.Print()

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-MC12-SDR-05')

conddb.iovdbsvc.dbConnection = "sqlite://;schema=PixMapOverlay_IBL.db;dbname=OFLP200"

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

from PixelCalibAlgs.PixelCalibAlgsConf import PixMapOverlayWriter

job += PixMapOverlayWriter()
job.PixMapOverlayWriter.ModuleStatus = 1

from AthenaServices.AthenaServicesConf import AthenaOutputStream

job += AthenaOutputStream( "Stream1" )

from PixelConditionsServices.PixelConditionsServicesConf import SpecialPixelMapSvc

ServiceMgr += SpecialPixelMapSvc()
SpecialPixelMapSvc = ServiceMgr.SpecialPixelMapSvc

SpecialPixelMapSvc.RegisterCallback = False    
SpecialPixelMapSvc.OutputFolder = "/PIXEL/PixMapOverlay"

### configure OutputConditionsAlg

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg

OutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg","PixMapOverlay.root")

OutputConditionsAlg.ObjectList=[ "CondAttrListCollection#" + SpecialPixelMapSvc.OutputFolder ]

OutputConditionsAlg.WriteIOV=True

#OutputConditionsAlg.Run1=0
#OutputConditionsAlg.LB1=0
#OutputConditionsAlg.Run2=99
#OutputConditionsAlg.LB2=9

OutputConditionsAlg.IOVTagList=["PixMapOverlay-IBL-000-00"]


### configure IOVRegistrationSvc

from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc

ServiceMgr += IOVRegistrationSvc()
regSvc = ServiceMgr.IOVRegistrationSvc

regSvc.OverrideNames = [ "ModuleSpecialPixelMap_Clob" ]
regSvc.OverrideTypes = [ "String4k" ]


### configure the event selector

from GaudiCoreSvc.GaudiCoreSvcConf import EventSelector

ServiceMgr += EventSelector()

EventSelector.RunNumber         = 0
EventSelector.EventsPerRun      = 5
EventSelector.FirstEvent        = 1
EventSelector.InitialTimeStamp  = 0
EventSelector.TimeStampInterval = 5
theApp.EvtMax                   = 1


### configure the message service
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )

MessageSvc.OutputLevel      = 3
MessageSvc.debugLimit       = 100000
MessageSvc.infoLimit        = 100000
MessageSvc.errorLimit       = 1000

job.PixMapOverlayWriter.ModuleIDFragments = [ ]

# This is where the module ID's go to mask entire modules
job.PixMapOverlayWriter.Modules = [
    84869120,
    85917696,
    87228416,
    18022400,
    21823488,
    42631168,
    42819584,
    42934272,
    43065344,
    43737088,
    43843584,
    51445760 ]

# This is where the dictionary of {Module:[FE's to mask]} goes
job.PixMapOverlayWriter.Frontends = { "50679808": [8,9,10,11],"52187136": [11] }
