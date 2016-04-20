isIBL = 0
mySQ = 0
if isIBL:
  myOutput= "pixelcalibsvcIBL3D_dumpdb.txt"
else:
  myOutput= "pixelcalibsvc_dump.txt"

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
###from InDetIBL_Example.SLHC_JobProperties import SLHC_Flags
## Leave commented out unless overriding with text file.
## Default is to use Geom DB only
#SLHC_Flags.SLHC_Version = "IBL-01"

###print SLHC_Flags

## SLHC setup
####from InDetIBL_Example.SLHC_Setup import SLHC_Setup
####SLHC_Setup = SLHC_Setup()

from IOVDbSvc.CondDB import conddb
#conddb.setGlobalTag('OFLCOND-CSC-00-00-00')
#conddb.setGlobalTag('OFLCOND-MC12-SIM-00')
#conddb.setGlobalTag('OFLCOND-RUN1-SDR-06') 
conddb.setGlobalTag('OFLCOND-RUN12-SDR-22')

#conddb.iovdbsvc.dbConnection = "sqlite://;schema=pixmapibl.db;dbname=OFLP200"

#conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapShort")
#conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapLong")
#conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapOverlay")
#conddb.addOverride('/PIXEL/PixMapShort','PixMapShort-Test-00')
#conddb.addOverride('/PIXEL/PixMapLong','PixMapLong-Test-00')

if mySQ:
  if isIBL:
    conddb.iovdbsvc.Folders = [ "<dbConnection>sqlite://;schema=pixmapdb_ibl3ddbm_IBL3D25DBM-04-01.db;dbname=OFLP200</dbConnection> /PIXEL/PixCalib <tag>PixCalib-IBL3D25DBM-04-01</tag>" ]
  else:
    conddb.iovdbsvc.Folders = [ "<dbConnection>sqlite://;schema=pixmapdb.db;dbname=OFLP200</dbConnection> /PIXEL/PixCalib <tag>PixCalib-IBL3D25DBM-04-01</tag>" ]
else:
  conddb.addFolder("PIXEL_OFL","/PIXEL/PixCalib")

### configure the special pixel map service

from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibSvc
PixelCalibSvc = PixelCalibSvc()


#ServiceMgr.EventSelector.RunNumber = 222222
ServiceMgr.EventSelector.RunNumber = 200805

### define the job

from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibServiceTest

job +=PixelCalibServiceTest(OutputTextFile =myOutput, MakeDummy = TRUE ) 

ServiceMgr += PixelCalibSvc


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
