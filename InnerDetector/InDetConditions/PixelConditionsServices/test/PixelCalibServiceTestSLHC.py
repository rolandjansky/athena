isSLHC = 1
mySQ = 1

if isSLHC: 
  myOutput= "pixelcalibsvcSLHC_dump.txt"
else:
  myOutput= "pixelcalibsvc_dump.txt"
  
#import AthenaCommon.AtlasUnixStandardJob

#MyOutPut = INFO
#from AthenaCommon.AppMgr import theApp
#from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

# --- set defaults
globalflags.DataSource='geant4'

if isSLHC:
  include("InDetSLHC_Example/preInclude.NoTRT.py")
else:
  DetFlags.detdescr.ID_setOn()

# Select the geometry version.
from AthenaCommon.GlobalFlags import globalflags
if isSLHC: 
  globalflags.DetDescrVersion = 'ATLAS-SLHC-02-00-00'
else:
  globalflags.DetDescrVersion = 'ATLAS-GEO-08-00-00'

#globalflags.DetDescrVersion = 'ATLAS-SLHC-02-00-00'
#globalflags.DetGeo.set_Value_and_Lock('atlas')
    
# import the # the conditions setup
#from IOVDbSvc.CondDB import conddb
#conddb.setGlobalTag('OFLCOND-SIM-00-00-00')
#conddb.setGlobalTag('OFLCOND-SDR-BS14T-SLHC-04')

# Initialize geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

# --- setup version
from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
## Leave commented out unless overriding with text file.
## Default is to use Geom DB only
#SLHC_Flags.SLHC_Version = "SLHC-01"

print SLHC_Flags

## SLHC setup
from InDetSLHC_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()

from AthenaCommon.AlgSequence import AlgSequence

topSequence = AlgSequence()

#include("PixelConditionsServices/PixelCalibSvc_jobOptions.py")

from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibSvc
ServiceMgr +=PixelCalibSvc()
PixelCalibSvc = ServiceMgr.PixelCalibSvc

#if not (globalflags.DataSource() == 'geant4'):
#  PixelCablingSvc.MappingFile = "PixelCabling/Pixels_Atlas_IdMapping_May08.dat"
#  conddb.addFolder("PIXEL","/PIXEL/ReadoutSpeed")
#else:
#  conddb.addFolderSplitMC("PIXEL","/PIXEL/ReadoutSpeed","/PIXEL/ReadoutSpeed")
#  ServiceMgr += PixelCablingSvc
                
from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibServiceTest

topSequence +=PixelCalibServiceTest(OutputTextFile =myOutput, MakeDummy = TRUE)

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
if isSLHC:
  IOVDbSvc.GlobalTag = "OFLCOND-ATLAS-HL-LHC-00"
else:
  IOVDbSvc.GlobalTag = "OFLCOND-MC12-SIM-00"

#"DEFAULTCOND"

if mySQ:
  from AthenaCommon.AppMgr import ServiceMgr as svcMgr
  # Setup Db stuff
  import AthenaPoolCnvSvc.AthenaPool
  import IOVDbSvc.IOVDb
  if isSLHC:
    connStr = "<dbConnection>sqlite://X;schema=pixcalibdb_slhc.db;dbname=OFLP200</dbConnection>"
    tag = "<tag>PixCalib-SLHC-00</tag>"
  else:
    connStr = "<dbConnection>sqlite://X;schema=pixcalibdb.db;dbname=OFLP200</dbConnection>"
    tag = "<tag>PixCalib-Test-00</tag>"
    
  folder = "/PIXEL/PixCalib"
  svcMgr.IOVDbSvc.Folders += [ folder + tag + connStr ]
else:
  from IOVDbSvc.CondDB import conddb
  conddb.addFolder("PIXEL_OFL","/PIXEL/PixCalib")
     

theApp.EvtMax = 1

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc.OutputLevel = DEBUG
#INFO
#DEBUG
# --- change output format
MessageSvc.Format       = "% F%30W%S%7W%R%T %0W%M"
MessageSvc.defaultLimit = 9999999
