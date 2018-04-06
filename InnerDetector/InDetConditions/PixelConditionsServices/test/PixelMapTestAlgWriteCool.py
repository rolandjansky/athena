import AthenaCommon.AtlasUnixStandardJob


### set up conddb

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-CSC-00-00-00')

conddb.iovdbsvc.dbConnection = "sqlite://;schema=pixmap.db;dbname=COMP200"


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

globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'geant4'
globalflags.InputFormat = 'pool'
globalflags.print_JobProperties()


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

from PixelConditionsServices.PixelConditionsServicesConf import PixelMapTestAlg, SpecialPixelMapSvc

job += PixelMapTestAlg()
job.PixelMapTestAlg.UseSummarySvc = False

from AthenaServices.AthenaServicesConf import AthenaOutputStream

job += AthenaOutputStream( "Stream1" )


### configure the special pixel map service

ServiceMgr += SpecialPixelMapSvc()
SpecialPixelMapSvc = ServiceMgr.SpecialPixelMapSvc

SpecialPixelMapSvc.DBFolders = [ "/PIXEL/PixMap" ]
# if reading from the database, this must be one of the IOVDbSvc folders
# default: empty 

SpecialPixelMapSvc.SpecialPixelMapKeys = [ "SpecialPixelMap" ]
# default: empty

SpecialPixelMapSvc.OutputFolder = "/PIXEL/PixMapShort"
SpecialPixelMapSvc.OutputLongFolder = "/PIXEL/PixMapLong"

SpecialPixelMapSvc.UseDualFolderStructure = True
# If true, only module masks which fit in a String4k are written to
# the OutputFolder. Module masks which are longer are written to 
# OuputLongFolder as Blob16M

SpecialPixelMapSvc.DifferentialUpdates = False
# If true, the new map is compared to the first map in SpecialPixelMapSvc.SpecialPixelMapKeys 
# and only channels for which the contents have changed are updated.

SpecialPixelMapSvc.DataSource = "Textfiles"
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

OutputConditionsAlg.ObjectList=[ "CondAttrListCollection#/PIXEL/PixMapShort" ]
OutputConditionsAlg.ObjectList+=[ "CondAttrListCollection#/PIXEL/PixMapLong" ]

OutputConditionsAlg.WriteIOV=True

#OutputConditionsAlg.Run1=1
#OutputConditionsAlg.LB1=4
#OutputConditionsAlg.Run2=1
#OutputConditionsAlg.LB2=9

OutputConditionsAlg.IOVTagList=[ "PixMapShort-Test-00" ]
OutputConditionsAlg.IOVTagList+=[ "PixMapLong-Test-00" ]


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

EventSelector.RunNumber         = 1
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
