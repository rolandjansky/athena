isIBL = 1
mySQ = 1
#dbType = 1 for PixMap, 2 for Overlay, and 3 for Noise 
dbType =2
import AthenaCommon.AtlasUnixGeneratorJob

## GlobalFlags

from AthenaCommon.GlobalFlags import globalflags

#globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'geant4'
if isIBL:
  globalflags.DetDescrVersion = 'ATLAS-IBL3D25-04-00-02'
else:  
  globalflags.DetDescrVersion = 'ATLAS-GEO-08-00-00'

globalflags.InputFormat = 'pool'
globalflags.print_JobProperties()

### set up conddb

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.pixel_setOn()
DetFlags.Print()

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

from IOVDbSvc.CondDB import conddb
#conddb.setGlobalTag('OFLCOND-CSC-00-00-00')
#conddb.setGlobalTag('OFLCOND-MC12-SIM-00')
conddb.setGlobalTag('OFLCOND-RUN12-SDR-22')
 
#conddb.iovdbsvc.dbConnection = "sqlite://;schema=pixmapibl.db;dbname=OFLP200"

#conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapShort")
#conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapLong")
#conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapOverlay")
#conddb.addOverride('/PIXEL/PixMapShort','PixMapShort-Test-00')
#conddb.addOverride('/PIXEL/PixMapLong','PixMapLong-Test-00')

if mySQ:
  if dbType == 1:
    conddb.iovdbsvc.Folders = [ "<dbConnection>sqlite://;schema=pixmapdb_ibl3ddbm_IBL3D25DBM-04-01.db;dbname=OFLP200</dbConnection> /PIXEL/PixMapShort <tag>PixMapShort-IBL3D25DBM-04-01</tag>" ]
    conddb.iovdbsvc.Folders += [ "<dbConnection>sqlite://;schema=pixmapdb_ibl3ddbm_IBL3D25DBM-04-01.db;dbname=OFLP200</dbConnection> /PIXEL/PixMapLong <tag>PixMapLong-IBL3D25DBM-04-01</tag>" ]
  elif dbType == 2:
    conddb.iovdbsvc.Folders = [ "<dbConnection>sqlite://;schema=MyPixOverlayDB.db;dbname=OFLP200</dbConnection> /PIXEL/PixMapOverlay <tag>PixMapOverlay-SIMU-000-00</tag>" ]
  else:
    conddb.iovdbsvc.Folders = [ "<dbConnection>sqlite://;schema=pixmapdb_ibl3ddbm_IBL3D25DBM-04-01.db;dbname=OFLP200</dbConnection> /PIXEL/NoiseMapShort <tag>NoiseMapShort-IBL3D25DBM-04-01</tag>" ]
    conddb.iovdbsvc.Folders += [ "<dbConnection>sqlite://;schema=pixmapdb_ibl3ddbm_IBL3D25DBM-04-01.db;dbname=OFLP200</dbConnection> /PIXEL/NoiseMapLong <tag>NoiseMapLong-IBL3D25DBM-04-01</tag>" ]
else:
  if dbType == 1:
    conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapShort","/PIXEL/PixMapShort")
    conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapLong","/PIXEL/PixMapLong")
  elif dbType == 2:
    conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapOverlay","/PIXEL/PixMapOverlay")
  else:
    conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/NoiseMapShort","/PIXEL/NoiseMapShort")
    conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/NoiseMapLong","/PIXEL/NoiseMapLong")

  #conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/NoiseMapShort","/PIXEL/NoiseMapShort")
  #conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/NoiseMapLong","/PIXEL/NoiseMapLong")
  #conddb.addFolder("PIXEL_OFL","/PIXEL/NoiseMapShort")
  #conddb.addFolder("PIXEL_OFL","/PIXEL/NoiseMapShort")
  #conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapShort")
  #conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapLong")
  #conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapOverlay")

### configure the special pixel map service

from PixelConditionsServices.PixelConditionsServicesConf import SpecialPixelMapSvc 
SpecialPixelMapSvc = SpecialPixelMapSvc()
if dbType == 1:
  SpecialPixelMapSvc.DBFolders = [ "/PIXEL/PixMapShort", "/PIXEL/PixMapLong" ]                                                         
  SpecialPixelMapSvc.SpecialPixelMapKeys = [ "SpecialPixelMap", "SpecialPixelMapLong" ]  
elif dbType == 2:
  SpecialPixelMapSvc.DBFolders = [ "/PIXEL/PixMapOverlay" ]
  SpecialPixelMapSvc.SpecialPixelMapKeys = [ "SpecialPixelMap" ]
  SpecialPixelMapSvc.OverlayFolder = "/PIXEL/PixMapOverlay"
  SpecialPixelMapSvc.OverlayKey = "PixMapOverlay"
else:
  SpecialPixelMapSvc.DBFolders = [ "/PIXEL/NoiseMapShort", "/PIXEL/NoiseMapLong" ]
  SpecialPixelMapSvc.SpecialPixelMapKeys = [ "SpecialPixelMap", "NoiseMapLong" ]

#SpecialPixelMapSvc.DBFolders += [ "/PIXEL/NoiseMapShort", "/PIXEL/NoiseMapLong" ]
#SpecialPixelMapSvc.SpecialPixelMapKeys += [ "NoiselMapShort", "NoiseMapLong" ]


#SpecialPixelMapSvc.OverlayFolder = "/PIXEL/PixMapOverlay"
#SpecialPixelMapSvc.OverlayKey = "PixMapOverlay"
#SpecialPixelMapSvc.RegisterCallback = False
#SpecialPixelMapSvc.DataSource = "Textfiles"
SpecialPixelMapSvc.ModuleIDsForPrinting = [ "all" ]

### configure the event selector

#ServiceMgr.EventSelector.RunNumber = 200805
ServiceMgr.EventSelector.RunNumber = 222222

### define the job

from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

from PixelConditionsServices.PixelConditionsServicesConf import PixelMapTestAlg
job += PixelMapTestAlg()
PixelMapTestAlg = PixelMapTestAlg()
PixelMapTestAlg.WriteTextFile = TRUE
PixelMapTestAlg.MakeDummy = False
if dbType == 2:
  PixelMapTestAlg.MakeOverlay = True

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
MessageSvc.infoLimit        = 10000
