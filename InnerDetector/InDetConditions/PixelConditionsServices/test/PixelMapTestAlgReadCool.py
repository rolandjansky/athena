import AthenaCommon.AtlasUnixGeneratorJob


## GlobalFlags

from AthenaCommon.GlobalFlags import globalflags

globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'geant4'
globalflags.DetDescrVersion = 'ATLAS-GEO-08-00-00'
globalflags.print_JobProperties()


### set up conddb

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.pixel_setOn()

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-CSC-00-00-00')

conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapShort")
conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapLong")
conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapOverlay")
#conddb.addOverride('/PIXEL/PixMap','PixMap-000-03')

#conddb.iovdbsvc.Folders += [ "<dbConnection>sqlite://;schema=noisemap.db;dbname=OFLP200</dbConnection> /PIXEL/NoiseMap <tag>NoiseMap-000-00</tag>" ]
#conddb.iovdbsvc.Folders += [ "<dbConnection>sqlite://;schema=module_overlay.db;dbname=COMP200</dbConnection> /PIXEL/PixMapOverlay <tag>PixMapOverlay-Test-00</tag>" ]


### configure the special pixel map service

from PixelConditionsServices.PixelConditionsServicesConf import SpecialPixelMapSvc 
SpecialPixelMapSvc = SpecialPixelMapSvc()

SpecialPixelMapSvc.DBFolders = [ "/PIXEL/PixMapShort", "/PIXEL/PixMapLong" ]
SpecialPixelMapSvc.SpecialPixelMapKeys = [ "SpecialPixelMap", "SpecialPixelMapLong" ]

SpecialPixelMapSvc.OverlayFolder = "/PIXEL/PixMapOverlay"
SpecialPixelMapSvc.OverlayKey = "PixMapOverlay"

#SpecialPixelMapSvc.RegisterCallback = False
#SpecialPixelMapSvc.DataSource = "Textfiles"
SpecialPixelMapSvc.ModuleIDsForPrinting = [ "all" ]


### configure the event selector

ServiceMgr.EventSelector.RunNumber = 5


### define the job

from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

from PixelConditionsServices.PixelConditionsServicesConf import PixelMapTestAlg

job += PixelMapTestAlg()

ServiceMgr += SpecialPixelMapSvc


theApp.EvtMax                   = 1


### set up auditors

from AthenaCommon.AppMgr import ServiceMgr

from GaudiSvc.GaudiSvcConf import AuditorSvc

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc

theAuditorSvc.Auditors  += [ "ChronoAuditor"]
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
theApp.AuditAlgorithms=True


### configure the message service
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )

MessageSvc.OutputLevel      = 3
MessageSvc.debugLimit       = 100000
MessageSvc.infoLimit        = 100000
