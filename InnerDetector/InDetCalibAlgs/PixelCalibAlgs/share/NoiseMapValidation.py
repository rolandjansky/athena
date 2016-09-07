import AthenaCommon.AtlasUnixGeneratorJob


### configure the special pixel map service

from PixelConditionsServices.PixelConditionsServicesConf import SpecialPixelMapSvc

SpecialPixelMapSvc = SpecialPixelMapSvc()
ServiceMgr += SpecialPixelMapSvc


## globalflags

from AthenaCommon.GlobalFlags import globalflags

globalflags.DatabaseInstance = 'CONDBR2'

globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'pool'
globalflags.DetDescrVersion = 'ATLAS-R2-2015-03-01-00'
#globalflags.ConditionsTag = 'OFLCOND-RUN12-SDR-20' # 27-Nov-2014
#globalflags.ConditionsTag = 'CONDBR2-ES1PA-2014-00' # steffen
globalflags.ConditionsTag = 'CONDBR2-BLKPA-2014-03'
globalflags.print_JobProperties()


### set up conddb

from IOVDbSvc.CondDB import conddb

conddb.setGlobalTag('CONDBR2-BLKPA-2014-03')
###conddb.setGlobalTag('COMCOND-000-00')
#conddb.setGlobalTag('OFLCOND-RUN12-SDR-22')

if not 'ReferenceDB' in dir() :
  ReferenceDB = "default"
if not 'ReferenceTag' in dir() :
  ReferenceTag = "PixNoiseMapShort-DATA-RUN2-000-00"
  ReferenceLongTag = "PixNoiseMapLong-DATA-RUN2-000-00"


if ReferenceDB == "sqlite" :
  dbConnection = "sqlite://;schema=noisemap.db;dbname=CONDBR2"
if ReferenceDB == "oracle" :
  dbConnection = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_PIXEL;dbname=CONDBR2"

if ReferenceDB == "sqlite" or ReferenceDB == "oracle" :
  SpecialPixelMapSvc.DBFolders = [ '/PIXEL/NoiseMapShortRef', '/PIXEL/NoiseMapLongRef' ]
  SpecialPixelMapSvc.SpecialPixelMapKeys = [ 'SpecialPixelMap', 'NoiseMapLong' ]
  conddb.iovdbsvc.Folders += [ "<dbConnection>" + dbConnection + "</dbConnection> /PIXEL/NoiseMapShort <tag>" + ReferenceTag + "</tag>" + "<key> /PIXEL/NoiseMapShortRef </key>"]
  conddb.iovdbsvc.Folders += [ "<dbConnection>" + dbConnection + "</dbConnection> /PIXEL/NoiseMapLong <tag>" + ReferenceLongTag + "</tag>" + "<key> /PIXEL/NoiseMapLongRef </key>" ]


if ReferenceDB == "default" :
  SpecialPixelMapSvc.DBFolders = [ '/PIXEL/NoiseMapShortRef_oracle', '/PIXEL/NoiseMapLongRef_oracle' ]
  SpecialPixelMapSvc.SpecialPixelMapKeys = [ 'SpecialPixelMap', 'NoiseMapLong' ]
  conddb.iovdbsvc.Folders += [ "<dbConnection> oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_PIXEL;dbname=CONDBR2 </dbConnection> /PIXEL/NoiseMapShort <tag>" + ReferenceTag + "</tag>" + "<key> /PIXEL/NoiseMapShortRef_oracle </key>"]
  conddb.iovdbsvc.Folders += [ "<dbConnection> oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_PIXEL;dbname=CONDBR2 </dbConnection> /PIXEL/NoiseMapLong <tag>" + ReferenceLongTag + "</tag>" + "<key> /PIXEL/NoiseMapLongRef_oracle </key>" ]

  if os.path.isfile("noisemap.db") :
    SpecialPixelMapSvc.OverlayFolder = '/PIXEL/NoiseMapShortRef_sqlite'
    SpecialPixelMapSvc.OverlayLongFolder = '/PIXEL/NoiseMapLongRef_sqlite'
    SpecialPixelMapSvc.OverlayKey = 'NoiseMapShort_sqlite'
    SpecialPixelMapSvc.OverlayLongKey = 'NoiseMapLong_sqlite'
    SpecialPixelMapSvc.ModuleLevelOverlay = True
    conddb.iovdbsvc.Folders += [ "<dbConnection> sqlite://;schema=noisemap.db;dbname=CONDBR2 </dbConnection> /PIXEL/NoiseMapShort <tag>" + ReferenceTag + "</tag>" + "<key> /PIXEL/NoiseMapShortRef_sqlite </key>"]
    conddb.iovdbsvc.Folders += [ "<dbConnection> sqlite://;schema=noisemap.db;dbname=CONDBR2 </dbConnection> /PIXEL/NoiseMapLong <tag>" + ReferenceLongTag + "</tag>" + "<key> /PIXEL/NoiseMapLongRef_sqlite </key>" ]



## configure the PixMapDBWriter algorithm:

from PixelCalibAlgs.PixelCalibAlgsConf import PixMapDBWriter

PixMapDBWriter = PixMapDBWriter()

if not 'NoiseMapFilename' in dir() :
  NoiseMapFilename = "NoiseMap.root"

PixMapDBWriter.PixMapFileName = NoiseMapFilename   # name of ROOT input file with maps of noisy pixels
PixMapDBWriter.DoValidate = True

PixMapDBWriter.SpecialPixelMapKey = SpecialPixelMapSvc.SpecialPixelMapKeys[0]


## DetFlags

from AthenaCommon.DetFlags import DetFlags

DetFlags.all_setOff()
DetFlags.pixel_setOn()
DetFlags.Print()

#DetDescrVersion = "ATLAS-GEO-08-00-00" #####
#DetDescrVersion = "ATLAS-R2-2015-02-01-00" # 09-Nov-2014


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
