
# input root file
#DeadMapFilename = "DeadMaps_182161_184169.root"

# start IOV
#OutputRun = int("182161")
#OutputLB = 1

#tags
#OutputTag = 'PixMapShort-Data11-000-00'
#OutputLongTag = 'PixMapLong-Data11-000-00'
#OutputTag = 'PixMapShort-DATA-RUN2-000-01'
#OutputLongTag = 'PixMapLong-DATA-RUN2-000-01'
#ReferenceTag = 'PixMapShort-DATA-RUN2-000-00'
#ReferenceLongTag = 'PixMapLong-DATA-RUN2-000-00'

doValidate = False


import AthenaCommon.AtlasUnixGeneratorJob


### configure the special pixel map service

from PixelConditionsServices.PixelConditionsServicesConf import SpecialPixelMapSvc

SpecialPixelMapSvc = SpecialPixelMapSvc()
ServiceMgr += SpecialPixelMapSvc

if not 'DifferentialUpdates' in dir() :
  #DifferentialUpdates = True
  DifferentialUpdates = False

SpecialPixelMapSvc.DifferentialUpdates = DifferentialUpdates

SpecialPixelMapSvc.OutputFolder = "/PIXEL/PixMapShort"
SpecialPixelMapSvc.OutputLongFolder = "/PIXEL/PixMapLong"
SpecialPixelMapSvc.UseDualFolderStructure = True

## globalflags

from AthenaCommon.GlobalFlags import globalflags

globalflags.DatabaseInstance = 'CONDBR2'  ####
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'pool'
globalflags.DetDescrVersion = 'ATLAS-R2-2015-03-01-00'
globalflags.ConditionsTag = 'CONDBR2-ES1PA-2015-08'
globalflags.print_JobProperties()


### set up conddb

from IOVDbSvc.CondDB import conddb

#conddb.setGlobalTag('COMCOND-000-00')
conddb.setGlobalTag('CONDBR2-ES1PA-2015-08')

## sqlite file db connection for writing
#conddb.iovdbsvc.dbConnection = "sqlite://;schema=deadmap_july11.db;dbname=COMP200"
conddb.iovdbsvc.dbConnection = "sqlite://;schema=deadmap.db;dbname=CONDBR2"

if not 'OutputTag' in dir() :
  #OutputTag = 'PixDeadMapShort-000-00'
  #OutputLongTag = 'PixDeadMapLong-000-00'
  #OutputTag = 'PixMapShort-RUN2-DATA-RUN1-UPD1-00'
  #OutputLongTag = 'PixMapLong-RUN2-DATA-RUN2-UPD1-00'
  OutputTag = 'PixMapShort-DATA-RUN2-000-00'
  OutputLongTag = 'PixMapLong-DATA-RUN2-000-00'
if not 'ReferenceTag' in dir() :
  ReferenceTag = OutputTag
  ReferenceLongTag = OutputLongTag


if SpecialPixelMapSvc.DifferentialUpdates == True :
  SpecialPixelMapSvc.DBFolders = [ '/PIXEL/PixMapShortRef_oracle', '/PIXEL/PixMapLongRef_oracle' ]
  SpecialPixelMapSvc.SpecialPixelMapKeys = [ 'SpecialPixelMap', 'PixMapLong' ]
  conddb.iovdbsvc.Folders += [ "<dbConnection> oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_PIXEL;dbname=CONDBR2 </dbConnection> /PIXEL/PixMapShort <tag>" + ReferenceTag + "</tag>" + "<key> /PIXEL/PixMapShortRef_oracle </key>"]
  conddb.iovdbsvc.Folders += [ "<dbConnection> oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_PIXEL;dbname=CONDBR2 </dbConnection> /PIXEL/PixMapLong <tag>" + ReferenceLongTag + "</tag>" + "<key> /PIXEL/PixMapLongRef_oracle </key>" ]

  if os.path.isfile("deadmap.db") :
    SpecialPixelMapSvc.OverlayFolder = '/PIXEL/PixMapShortRef_sqlite'
    SpecialPixelMapSvc.OverlayLongFolder = '/PIXEL/PixMapLongRef_sqlite'
    SpecialPixelMapSvc.OverlayKey = 'PixMapShort_sqlite'
    SpecialPixelMapSvc.OverlayLongKey = 'PixMapLong_sqlite'
    SpecialPixelMapSvc.ModuleLevelOverlay = True
    conddb.iovdbsvc.Folders += [ "<dbConnection> sqlite://;schema=deadmap.db;dbname=CONDBR2 </dbConnection> /PIXEL/PixMapShort <tag>" + ReferenceTag + "</tag>" + "<key> /PIXEL/PixMapShortRef_sqlite </key>"]
    conddb.iovdbsvc.Folders += [ "<dbConnection> sqlite://;schema=deadmap.db;dbname=CONDBR2 </dbConnection> /PIXEL/PixMapLong <tag>" + ReferenceLongTag + "</tag>" + "<key> /PIXEL/PixMapLongRef_sqlite </key>" ]



## configure the PixMapDBWriter algorithm:

from PixelCalibAlgs.PixelCalibAlgsConf import PixMapDBWriter

PixMapDBWriter = PixMapDBWriter()

if not 'PixMapFilename' in dir() :
  PixMapFilename = "DeadMap.root"

PixMapDBWriter.PixMapFileName = PixMapFilename   # name of ROOT input file with maps of noisy pixels
PixMapDBWriter.CalculateOccupancy = False
PixMapDBWriter.PixelPropertyName = "dead"
#PixMapDBWriter.PixelStatus = 3
PixMapDBWriter.PixelStatus = 2049
PixMapDBWriter.ListSpecialPixels = False

doMerge = False

if doMerge == True :
  PixMapDBWriter.SpecialPixelMapKey = SpecialPixelMapSvc.SpecialPixelMapKeys[0] # key of special pixel map to merge with the new noise map


## DetFlags

from AthenaCommon.DetFlags import DetFlags

DetFlags.all_setOff()
DetFlags.pixel_setOn()
DetFlags.Print()

#DetDescrVersion = "ATLAS-GEO-08-00-02"

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


### define the job

from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

job += PixMapDBWriter

theApp.CreateSvc += [ ServiceMgr.SpecialPixelMapSvc.getFullName() ]

### configure OutputConditionsAlg

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg

OutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg","dummy.root")

OutputConditionsAlg.WriteIOV=True

OutputConditionsAlg.Run1 = OutputRun
OutputConditionsAlg.LB1 = OutputLB

OutputConditionsAlg.ObjectList=[ "CondAttrListCollection#" + SpecialPixelMapSvc.OutputFolder ]
OutputConditionsAlg.ObjectList+=[ "CondAttrListCollection#" + SpecialPixelMapSvc.OutputLongFolder ]

OutputConditionsAlg.IOVTagList=[ OutputTag ]
OutputConditionsAlg.IOVTagList+=[ OutputLongTag ]

from AthenaServices.AthenaServicesConf import AthenaOutputStream

job += AthenaOutputStream( "Stream1" )

ServiceMgr += SpecialPixelMapSvc


### configure IOVRegistrationSvc

from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc

ServiceMgr += IOVRegistrationSvc()
regSvc = ServiceMgr.IOVRegistrationSvc

regSvc.OverrideNames = [ "ModuleSpecialPixelMap_Clob" ]
regSvc.OverrideTypes = [ "String4k" ]
regSvc.OverrideNames += [ "SpecialPixelMap" ]
regSvc.OverrideTypes += [ "Blob16M" ]

regSvc.PayloadTable = True


### configure the event selector

ServiceMgr.EventSelector.RunNumber         = OutputRun
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
