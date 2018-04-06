isIBL = 1
mySQ = 1
#import AthenaCommon.AtlasUnixStandardJob

#MyOutPut = INFO
#from AthenaCommon.AppMgr import theApp
#from AthenaCommon.AppMgr import ServiceMgr

if not 'MYRUN' in dir():
   MYRUN=0

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

# --- set defaults
globalflags.DataSource='geant4'

DetFlags.detdescr.ID_setOn()

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-MC12-SIM-00')
if mySQ:
  conddb.iovdbsvc.Folders = [ "<dbConnection>sqlite://;schema=myofflinedb_ibl.db;dbname=OFLP200</dbConnection> /PIXEL/PixReco <tag> PixelOfflineReco-Test-00</tag>" ]
else:
  conddb.addFolder("PIXEL_OFL","/PIXEL/PixReco")

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
from InDetIBL_Example.SLHC_JobProperties import SLHC_Flags
## Leave commented out unless overriding with text file.
## Default is to use Geom DB only
#SLHC_Flags.SLHC_Version = "IBL-01"

print SLHC_Flags

## SLHC setup
from InDetIBL_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()

from AthenaCommon.AlgSequence import AlgSequence

topSequence = AlgSequence()

#include("PixelConditionsServices/PixelRecoDb_jobOptions.py")

from PixelConditionsServices.PixelConditionsServicesConf import PixelOfflineCalibSvc
ServiceMgr +=PixelOfflineCalibSvc()
 
from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
ToolSvc += PixelRecoDbTool()
ToolSvc.PixelRecoDbTool.OutputLevel = VERBOSE
ToolSvc.PixelRecoDbTool.InputSource = 2
                
from PixelConditionsServices.PixelConditionsServicesConf import PixelOfflineCalibServiceTest
topSequence +=PixelOfflineCalibServiceTest(OutputTextFilePixelClusterError ="PixelClusterErrorDump_IBL.txt",
                                           OutputTextFilePixelClusterOnTrackError="PixelClusterOnTrackErrorDump_IBL.txt",
                                           OutputTextFilePixelChargeInterpolation="PixelChargeInterpolationDump_IBL.txt")

#from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
#IOVDbSvc.GlobalTag = "OFLCOND-SIM-00-00-00"
#"DEFAULTCOND"

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
