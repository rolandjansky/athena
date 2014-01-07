import AthenaCommon.AtlasUnixGeneratorJob


### configure the special pixel map service

from PixelConditionsServices.PixelConditionsServicesConf import SpecialPixelMapSvc

SpecialPixelMapSvc = SpecialPixelMapSvc()
ServiceMgr += SpecialPixelMapSvc


## globalflags

from AthenaCommon.GlobalFlags import globalflags

globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'pool'
globalflags.print_JobProperties()


### set up conddb

from IOVDbSvc.CondDB import conddb

conddb.setGlobalTag('COMCOND-000-00')

if not 'ReferenceDB' in dir() :
  ReferenceDB = "default"
if not 'ReferenceTag' in dir() :
  ReferenceTag = "PixMapShort-000-00"
  ReferenceLongTag = "PixMapLong-000-00"
if not 'PixelPropertyName' in dir() :
  PixelPropertyName = "dead"


if ReferenceDB == "sqlite" :
  dbConnection = "sqlite://;schema=noisemap.db;dbname=COMP200"
if ReferenceDB == "oracle" :
  dbConnection = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_PIXEL;dbname=COMP200"

if ReferenceDB == "sqlite" or ReferenceDB == "oracle" :
  SpecialPixelMapSvc.DBFolders = [ '/PIXEL/PixMapShortRef', '/PIXEL/PixMapLongRef' ]
  SpecialPixelMapSvc.SpecialPixelMapKeys = [ 'SpecialPixelMap', 'PixMapLong' ]
  conddb.iovdbsvc.Folders += [ "<dbConnection>" + dbConnection + "</dbConnection> /PIXEL/PixMapShort <tag>" + ReferenceTag + "</tag>" + "<key> /PIXEL/PixMapShortRef </key>"]
  conddb.iovdbsvc.Folders += [ "<dbConnection>" + dbConnection + "</dbConnection> /PIXEL/PixMapLong <tag>" + ReferenceLongTag + "</tag>" + "<key> /PIXEL/PixMapLongRef </key>" ]


if ReferenceDB == "default" :
  SpecialPixelMapSvc.DBFolders = [ '/PIXEL/PixMapShortRef_oracle', '/PIXEL/PixMapLongRef_oracle' ]
  SpecialPixelMapSvc.SpecialPixelMapKeys = [ 'SpecialPixelMap', 'PixMapLong' ]
  conddb.iovdbsvc.Folders += [ "<dbConnection> oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_PIXEL;dbname=COMP200 </dbConnection> /PIXEL/PixMapShort <tag>" + ReferenceTag + "</tag>" + "<key> /PIXEL/PixMapShortRef_oracle </key>"]
  conddb.iovdbsvc.Folders += [ "<dbConnection> oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_PIXEL;dbname=COMP200 </dbConnection> /PIXEL/PixMapLong <tag>" + ReferenceLongTag + "</tag>" + "<key> /PIXEL/PixMapLongRef_oracle </key>" ]

  if os.path.isfile("noisemap.db") :
    SpecialPixelMapSvc.OverlayFolder = '/PIXEL/PixMapShortRef_sqlite'
    SpecialPixelMapSvc.OverlayLongFolder = '/PIXEL/PixMapLongRef_sqlite'
    SpecialPixelMapSvc.OverlayKey = 'PixMapShort_sqlite'
    SpecialPixelMapSvc.OverlayLongKey = 'PixMapLong_sqlite' 
    SpecialPixelMapSvc.ModuleLevelOverlay = True
    conddb.iovdbsvc.Folders += [ "<dbConnection> sqlite://;schema=noisemap.db;dbname=COMP200 </dbConnection> /PIXEL/PixMapShort <tag>" + ReferenceTag + "</tag>" + "<key> /PIXEL/PixMapShortRef_sqlite </key>"]
    conddb.iovdbsvc.Folders += [ "<dbConnection> sqlite://;schema=noisemap.db;dbname=COMP200 </dbConnection> /PIXEL/PixMapLong <tag>" + ReferenceLongTag + "</tag>" + "<key> /PIXEL/PixMapLongRef_sqlite </key>" ]



## configure the PixMapDBWriter algorithm:

from PixelCalibAlgs.PixelCalibAlgsConf import PixMapDBWriter

PixMapDBWriter = PixMapDBWriter()

PixMapDBWriter.PixMapFileName = PixMapFilename   # name of ROOT input file with maps of noisy pixels
PixMapDBWriter.DoValidate = True

PixMapDBWriter.SpecialPixelMapKey = SpecialPixelMapSvc.SpecialPixelMapKeys[0] 

PixMapDBWriter.PixelPropertyName = PixelPropertyName

if 'PixelStatus' in dir() :
  PixMapDBWriter.PixelStatus = PixelStatus
if 'ListSpecialPixels' in dir() :
  PixMapDBWriter.ListSpecialPixels = ListSpecialPixels


## DetFlags

from AthenaCommon.DetFlags import DetFlags

DetFlags.all_setOff()
DetFlags.pixel_setOn()
DetFlags.Print()

DetDescrVersion = "ATLAS-GEO-08-00-00"

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


### define the job

from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

job += PixMapDBWriter

theApp.CreateSvc += [ ServiceMgr.SpecialPixelMapSvc.getFullName() ]

### configure the event selector

if not 'ReferenceRun' in dir() :
  ReferenceRun = 0

ServiceMgr.EventSelector.RunNumber         = ReferenceRun
ServiceMgr.EventSelector.EventsPerRun      = 5
ServiceMgr.EventSelector.FirstEvent        = 1
ServiceMgr.EventSelector.InitialTimeStamp  = 0
ServiceMgr.EventSelector.TimeStampInterval = 5
theApp.EvtMax                   = 1


### configure the message service
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )

MessageSvc.OutputLevel      = 3
MessageSvc.debugLimit       = 100000
MessageSvc.infoLimit        = 100000
MessageSvc.errorLimit       = 1000
