## Job options for masking a set of pixel modules, frontends and column pairs during the
## reconstruction or digitization
##
## The list of modules is written to an sqlite file. The file can be read in a
## reconstruction or digitization job by adding the lines
#
# from IOVDbSvc.CondDB import conddb
# conddb.blockFolder('/PIXEL/PixMapOverlay')
# conddb.addFolderWithTag("module_overlay.db","/PIXEL/PixMapOverlay","PixMapOverlay-Test-00",True) 
#
## to the job options. In the reconstruction, the mask is applied during the clustering. To mask
## modules during the reconstruction from ESD the clustering needs to be re-run.
##


import AthenaCommon.AtlasUnixStandardJob


### set up conddb

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-CSC-00-00-00')

conddb.iovdbsvc.dbConnection = "sqlite://;schema=pixmap_overlay.db;dbname=COMP200"


## globalflags

from AthenaCommon.GlobalFlags import globalflags

globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'bytestream'
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

from PixelCalibAlgs.PixelCalibAlgsConf import PixMapOverlayWriter

job += PixMapOverlayWriter()
#job.PixMapOverlayWriter.Modules = [ 34414592, 45490176, 52903936, 84852736 ]
job.PixMapOverlayWriter.ModuleStatus = 3

### Beginnig characters of module ID
### (mask layers, bistaves, staves, pp0s or single modules, by online ID) 
#job.PixMapOverlayWriter.ModuleIDFragments = [ "D1C" ]


### Frontends
#job.PixMapOverlayWriter.Frontends = { "L1_B13_S2_A7_M0": [8,9,10,11],
#                                      "L1_B02_S2_A7_M2A": [11] }

### Column Pairs
### Note for each module, a single vector of integers is given
### with alternating frontend/column pair indices
#job.PixMapOverlayWriter.ColumnPairs = { "L2_B24_S1_C7_M1C": [7,5,
#                                                             7,6,
#                                                             7,7,
#                                                             7,8],
#                                        "L2_B08_S1_C7_M3C": [1,2] }


### Cooling loops, rods, pp0s (mask by connectivity)
#job.PixMapOverlayWriter.CoolingLoops = [ 4, 19, 20, 21 ]
#job.PixMapOverlayWriter.RODs = [ "ROD_B2_S16", "ROD_B2_S17", "ROD_B2_S18", "ROD_B2_S19", "ROD_B2_S20" ]
#job.PixMapOverlayWriter.PP0s = [ "L1_B01_S1_A6", "L1_B01_S1_A6", "L1_B01_S2_A7" ]


### Generic objects (any object defined in connectivity database
#job.PixMapOverlayWriter.GenericObjects = [ "ROD_CRATE_B1" ]

#job.PixMapOverlayWriter.ConnectionString = "oracle://ATLAS_COOLPROD/ATLAS_COOLONL_PIXEL"
#job.PixMapOverlayWriter.ConnectivityTag = "PIT-ALL-V38"
#job.PixMapOverlayWriter.AliasTag = "PIT-ALL-V38"

from AthenaServices.AthenaServicesConf import AthenaOutputStream

job += AthenaOutputStream( "Stream1" )


### configure the special pixel map service

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

#OutputConditionsAlg.Run1=1
#OutputConditionsAlg.LB1=4
#OutputConditionsAlg.Run2=1
#OutputConditionsAlg.LB2=9

OutputConditionsAlg.IOVTagList=["PixMapOverlay-Test-00"]


### configure IOVRegistrationSvc

from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc

ServiceMgr += IOVRegistrationSvc()
regSvc = ServiceMgr.IOVRegistrationSvc

regSvc.OverrideNames = [ "ModuleSpecialPixelMap_Clob" ]
regSvc.OverrideTypes = [ "String4k" ]


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
