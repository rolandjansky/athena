isIBL = 1
#import AthenaCommon.AtlasUnixStandardJob

#MyOutPut = INFO
#from AthenaCommon.AppMgr import theApp
#from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

# --- set defaults
globalflags.DataSource='geant4'

DetFlags.detdescr.ID_setOn()

# Select the geometry version.
from AthenaCommon.GlobalFlags import globalflags
if isIBL:
  globalflags.DetDescrVersion = 'ATLAS-IBL3D25-03-00-00'
else:
  globalflags.DetDescrVersion = 'ATLAS-GEO-08-00-00'

#globalflags.DetDescrVersion = 'ATLAS-IBL-02-00-00'
#globalflags.DetGeo.set_Value_and_Lock('atlas')
    
# import the # the conditions setup
#from IOVDbSvc.CondDB import conddb
#conddb.setGlobalTag('OFLCOND-SIM-00-00-00')
#conddb.setGlobalTag('OFLCOND-SDR-BS14T-IBL-04')

# Initialize geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

# --- setup version
#from InDetIBL_Example.SLHC_JobProperties import SLHC_Flags
## Leave commented out unless overriding with text file.
## Default is to use Geom DB only
#SLHC_Flags.SLHC_Version = "IBL-01"

#print SLHC_Flags

## SLHC setup
#from InDetIBL_Example.SLHC_Setup import SLHC_Setup
#SLHC_Setup = SLHC_Setup()


from AthenaCommon.AlgSequence import AlgSequence

topSequence = AlgSequence()

from AthenaCommon.AppMgr import ToolSvc

from PixelConditionsTools.PixelConditionsToolsConf import PixelCalibDbTool
ToolSvc +=PixelCalibDbTool()
PixelCalibDbTool.CalibFolder ="/PIXEL/PixCalib"
PixelCalibDbTool.WriteDB = True

from PixelConditionsTools.PixelConditionsToolsConf import PixelCalibDbTestWriteRead

PixelCalibDbTestWriteRead = PixelCalibDbTestWriteRead()
topSequence += PixelCalibDbTestWriteRead
PixelCalibDbTestWriteRead.Write = True
PixelCalibDbTestWriteRead.Read = False
if isIBL:
  PixelCalibDbTestWriteRead.InputTextFile ="pixelcalibsvcIBL3D_dump.txt"
else:
  PixelCalibDbTestWriteRead.InputTextFile ="pixelcalibsvc_dump.txt"
    
PixelCalibDbTestWriteRead.OutputTextFile ="pixelcalib_dump.txt"

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg(outputFile="dummy.root")
myOCA.ObjectList=[ "CondAttrListCollection#/PIXEL/PixCalib" ]
myOCA.WriteIOV=True
myOCA.Run1=222222
myOCA.IOVTagList=["PixCalib-IBL3D25DBM-04-02"]

from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
ServiceMgr += IOVRegistrationSvc()
regSvc = ServiceMgr.IOVRegistrationSvc
regSvc.OverrideNames = [ "data" ]
regSvc.OverrideTypes = [ "String4k" ]

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
IOVDbSvc.GlobalTag = "OFLCOND-RUN1-SDR-06"
#"DEFAULTCOND"

if isIBL:
  IOVDbSvc.dbConnection = "sqlite://X;schema=pixcalibdb_ibl.db;dbname=OFLP200"
else:
  IOVDbSvc.dbConnection = "sqlite://X;schema=pixcalibdb.db;dbname=OFLP200"  

theApp.EvtMax = 1

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc.OutputLevel = INFO
#DEBUG
# --- change output format
MessageSvc.Format       = "% F%30W%S%7W%R%T %0W%M"
MessageSvc.defaultLimit = 9999999
