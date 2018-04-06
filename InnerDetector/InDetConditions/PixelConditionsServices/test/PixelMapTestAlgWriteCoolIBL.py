isIBL = 1
# dbType=1 for PixelMap; 2 for overlay; 3=noise
dbType = 1
import AthenaCommon.AtlasUnixStandardJob

### set up conddb

from IOVDbSvc.CondDB import conddb
#conddb.setGlobalTag('OFLCOND-CSC-00-00-00')
#conddb.setGlobalTag('OFLCOND-MC12-SIM-00')
conddb.setGlobalTag('OFLCOND-RUN12-SDR-22')
if isIBL:
  if dbType == 1:
    conddb.iovdbsvc.dbConnection = "sqlite://;schema=pixmapdb_ibl3ddbm_IBL3D25DBM-04-01.db;dbname=OFLP200"
  elif dbType == 2:
    conddb.iovdbsvc.dbConnection = "sqlite://;schema=MyPixOverlayDB.db;dbname=OFLP200"
  else:
    conddb.iovdbsvc.dbConnection = "sqlite://;schema=pixmapdb_noise.db;dbname=OFLP200"
else:
  if dbType == 1:
    conddb.iovdbsvc.dbConnection = "sqlite://;schema=pixmap.db;dbname=OFLP200"  
  elif dbType == 2:
    conddb.iovdbsvc.dbConnection = "sqlite://;schema=pixmap_overlay.db;dbname=OFLP200"
  else:
    conddb.iovdbsvc.dbConnection = "sqlite://;schema=pixmap_noise.db;dbname=OFLP200"

### set up auditors

from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import AuditorSvc

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc

theAuditorSvc.Auditors  += [ "ChronoAuditor"]
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
theApp.AuditAlgorithms=True


## globalflags

from AthenaCommon.GlobalFlags import globalflags

#globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'geant4'

if isIBL:
  globalflags.DetDescrVersion = 'ATLAS-IBL3D25-04-00-02'
else:  
  globalflags.DetDescrVersion = 'ATLAS-GEO-08-00-00'
  
globalflags.InputFormat = 'pool'
globalflags.print_JobProperties()


## DetFlags

from AthenaCommon.DetFlags import DetFlags

DetFlags.all_setOff()
DetFlags.pixel_setOn()
DetFlags.Print()

#DetDescrVersion = "ATLAS-GEO-08-00-00"

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

# --- setup version
#from InDetIBL_Example.SLHC_JobProperties import SLHC_Flags
## Leave commented out unless overriding with text file.
## Default is to use Geom DB only
#SLHC_Flags.SLHC_Version = "IBL-01"

#print SLHC_Flags

## SLHC setup
#from InDetIBL_Example.SLHC_Setup import SLHC_Setup
#SLHC_Setup = SLHC_Setup()

### define the job

from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

from PixelConditionsServices.PixelConditionsServicesConf import PixelMapTestAlg, SpecialPixelMapSvc

job += PixelMapTestAlg()
job.PixelMapTestAlg.UseSummarySvc = False

from AthenaServices.AthenaServicesConf import AthenaOutputStream

job += AthenaOutputStream( "Stream1" )


### configure the special pixel map service

ServiceMgr += SpecialPixelMapSvc()
SpecialPixelMapSvc = ServiceMgr.SpecialPixelMapSvc
SpecialPixelMapSvc.SpecialPixelMapKeys = [ "SpecialPixelMap" ]
if dbType == 1:
  SpecialPixelMapSvc.DBFolders = [ "/PIXEL/PixMap" ]
  SpecialPixelMapSvc.OutputFolder = "/PIXEL/PixMapShort"
  SpecialPixelMapSvc.OutputLongFolder = "/PIXEL/PixMapLong"
elif dbType == 2:
  SpecialPixelMapSvc.DBFolders = [ "/PIXEL/PixMapOverlay" ]
  SpecialPixelMapSvc.OutputFolder = "/PIXEL/PixMapOverlay"
  SpecialPixelMapSvc.ModuleLevelOverlay = True
else:
  SpecialPixelMapSvc.DBFolders = [ "/PIXEL/PixMap" ]
  SpecialPixelMapSvc.OutputFolder = "/PIXEL/NoiseMapShort"
  SpecialPixelMapSvc.OutputLongFolder = "/PIXEL/NoiseMapLong"

SpecialPixelMapSvc.UseDualFolderStructure = True
# If true, only module masks which fit in a String4k are written to
# the OutputFolder. Module masks which are longer are written to 
# OuputLongFolder as Blob16M

SpecialPixelMapSvc.DifferentialUpdates = False
# If true, the new map is compared to the first map in SpecialPixelMapSvc.SpecialPixelMapKeys 
# and only channels for which the contents have changed are updated.

SpecialPixelMapSvc.DataSource = "Textfiles"

# note: Filelist should be inside the directory of FileListDir
if isIBL:
  if dbType == 1:
    SpecialPixelMapSvc.FileList = "filelistibl" 
    SpecialPixelMapSvc.FileListDir = "SpecialMapIBL3D_OFLCOND_RUN1_SDR_06_orig"
  elif dbType == 2: 
    SpecialPixelMapSvc.FileList = "filelistibl"
    SpecialPixelMapSvc.FileListDir ="SpecialMapIBL3D_OFLCOND_RUN12_SDR_22_overlay_orig"
  else:
    SpecialPixelMapSvc.FileList = "filelistibl"
    SpecialPixelMapSvc.FileListDir ="SpecialMapIBL3D_OFLCOND_RUN12_SDR_22_noise_orig"
else:
  if dbType == 1:
    SpecialPixelMapSvc.FileList = "Filelist"
    SpecialPixelMapSvc.FileListDir = "SpecialMap_orig"
  elif dbType == 2:
    SpecialPixelMapSvc.FileList = "filelistibl"
    SpecialPixelMapSvc.FileListDir ="SpecialMapIBL3D_OFLCOND_RUN12_SDR_22_overlay_orig"
  else:
    SpecialPixelMapSvc.FileList = "filelistibl"
    SpecialPixelMapSvc.FileListDir ="SpecialMapIBL3D_OFLCOND_RUN12_SDR_22_noise_orig"

# "Database", "Textfiles" or "None"; default: "Database"
# data source to be used when SpecialPixelMapSvc::create() is called
# when the DataSource is "Textfiles" the DetectorSpecialPixelMap and the corresponding CondAttrListCollection are created
# when the DataSource is "Database" only the DetectorSpecialPixelMap is created
# default: "None" (special pixel maps are not created with "create()" but by callback function)

#SpecialPixelMapSvc.ModuleIDsForPrinting = [ "all" ]
# list of modules IDs to print when SpecialPixelMapSvc::print() is called, can be "all"
# default: empty

SpecialPixelMapSvc.RegisterCallback = False    
# triggers registration of callback in SpecialPixelMapSvc::initialize()
# which automatically updates the list of DetectorSpecialPixelMaps in the detector store
# whenever the corresponding CondAttrListCollections are updated by the IOVDbSvc
# default: true

SpecialPixelMapSvc.PrintVerbosePixelID = True
# print pixel ID as chip/column/row (verbose) or unsigned int (not verbose)
# default: true

SpecialPixelMapSvc.PrintBinaryPixelStatus = True
# print pixel status in binary or decimal format
# default: true

SpecialPixelMapSvc.MarkSpecialRegions = True
# if true, regions (modules/chips/column pairs) of pixels with the same pixel status are marked
# the status for those regions is stored in a special data structure, the individual pixel status integers
# are removed from the map
# default: true

SpecialPixelMapSvc.MergePixelMaps = True
# merge all pixel maps into one, stored at SGKeys(0)
# default: true


### configure OutputConditionsAlg

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg

OutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg","dummy.root")
if dbType == 1:
  OutputConditionsAlg.ObjectList=[ "CondAttrListCollection#/PIXEL/PixMapShort" ]
  OutputConditionsAlg.ObjectList+=[ "CondAttrListCollection#/PIXEL/PixMapLong" ]
elif dbType == 2:
  OutputConditionsAlg.ObjectList=[ "CondAttrListCollection#/PIXEL/PixMapOverlay" ]
else:
  OutputConditionsAlg.ObjectList=[ "CondAttrListCollection#/PIXEL/NoiseMapShort" ]
  OutputConditionsAlg.ObjectList+=[ "CondAttrListCollection#/PIXEL/NoiseMapLong" ]

OutputConditionsAlg.WriteIOV=True
OutputConditionsAlg.Run1=222222

#OutputConditionsAlg.Run1=1
#OutputConditionsAlg.LB1=4
#OutputConditionsAlg.Run2=1
#OutputConditionsAlg.LB2=9

if isIBL:
  if dbType == 1:
    OutputConditionsAlg.IOVTagList=[ "PixMapShort-IBL3D25DBM-04-01" ]
    OutputConditionsAlg.IOVTagList+=[ "PixMapLong-IBL3D25DBM-04-01" ]
  elif dbType == 2:
    OutputConditionsAlg.IOVTagList=[ "PixMapOverlay-SIMU-000-00" ]
  else:
    OutputConditionsAlg.IOVTagList=[ "NoiseMapShort-IBL3D25DBM-04-01" ]
    OutputConditionsAlg.IOVTagList+=[ "NoiseMapLong-IBL3D25DBM-04-01" ]
else:
  if dbType == 1:
    OutputConditionsAlg.IOVTagList=[ "PixMapShort-Test-00" ]
    OutputConditionsAlg.IOVTagList+=[ "PixMapLong-Test-00" ]
  elif dbType == 2:
    OutputConditionsAlg.IOVTagList=[ "PixMapOverlay-SIMU-000-00" ]
  else: 
    OutputConditionsAlg.IOVTagList=[ "NoiseMapShort-Test-00" ]
    OutputConditionsAlg.IOVTagList+=[ "NoiseMapLong-Test-00" ]

### configure IOVRegistrationSvc for writing of CLOBs

from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc

ServiceMgr += IOVRegistrationSvc()
regSvc = ServiceMgr.IOVRegistrationSvc

regSvc.OverrideNames = [ "ModuleSpecialPixelMap_Clob" ]
regSvc.OverrideTypes = [ "String4k" ]
regSvc.OverrideNames += [ "SpecialPixelMap" ]
regSvc.OverrideTypes += [ "Blob16M" ]

regSvc.PayloadTable = True

theApp.CreateSvc += [ ServiceMgr.IOVRegistrationSvc.getFullName() ]

### configure the event selector

from GaudiSvc.GaudiSvcConf import EventSelector

ServiceMgr += EventSelector()

#EventSelector.RunNumber         = 200805
EventSelector.RunNumber         = 222222
EventSelector.EventsPerRun      = 5
EventSelector.FirstEvent        = 1
EventSelector.InitialTimeStamp  = 0
EventSelector.TimeStampInterval = 5
theApp.EvtMax                   = 1

### configure the message service
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )

MessageSvc.OutputLevel      = 2
MessageSvc.debugLimit       = 100000
MessageSvc.infoLimit        = 10000
MessageSvc.errorLimit       = 1000

conddb.blockFolder('/Indet/Align')
