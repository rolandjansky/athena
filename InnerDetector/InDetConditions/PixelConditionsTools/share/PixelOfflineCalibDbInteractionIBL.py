###############################################################
#
# Job options file to read charge interpolation constants from
# text file and output a new pool file and sqlite file
#
#==============================================================
isIBL = 1
WRITEDB = 1
mySQ = 1
if 'WRITEDB' in dir() and WRITEDB:
  dowrite=TRUE
  doread=FALSE
  constantssource=1
  errorsfileold="Offlinedb/PixelClusterErrorData_new.txt"
  constantsfile="Offlinedb/PixelChargeInterpolationData_new.txt"
  errorsfilenew="Offlinedb/PixelClusterOnTrackErrorData_new.txt"
  print " Writing DB "
else:
  dowrite=FALSE
  doread=TRUE
  constantssource=2
  errorsfileold="Offlinedb/PixelClusterErrorData_read.txt"
  constantsfile="Offlinedb/PixelChargeInterpolationData_read.txt"
  errorsfilenew="Offlinedb/PixelClusterOnTrackErrorData_read.txt"
  print " Reading from DB"

if not 'MYTAG' in dir() or MYTAG=='':
  #MYTAG="PixelOfflineReco-03"
  #MYTAG="PixelOfflineReco-7TeV-000-05"
   MYTAG="PixelOfflineReco-Test-00"

if not 'MYDBTYPE' in dir() or MYDBTYPE!="PIXEL_OFL":
  MYDBTYPE="PIXEL_OFL"
  #MYDBTYPE="LOCAL"

if not 'MYRUN' in dir():
  MYRUN=0

#==============================================================

from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.DetFlags import DetFlags

#GlobalFlags.DetGeo.set_ctbh8()
#GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DetGeo.set_commis()

GlobalFlags.DataSource='geant4'
#GlobalFlags.DataSource.set_data()
DetFlags.detdescr.ID_setOn()

#include("IOVDbSvc/CondDBSetup.py")
from IOVDbSvc.CondDB import conddb
#conddb.setGlobalTag('DEFAULTCOND')
conddb.setGlobalTag('OFLCOND-MC12-SIM-00')

if doread:
  if mySQ:
    if isIBL:
      conddb.iovdbsvc.Folders = [ "<dbConnection>sqlite://;schema=myofflinedb_ibl.db;dbname=OFLP200</dbConnection> /PIXEL/PixReco <tag> PixelOfflineReco-Test-00</tag> " ]
    else:
      conddb.iovdbsvc.Folders = [ "<dbConnection>sqlite://;schema=myofflinedb.db;dbname=OFLP200</dbConnection> /PIXEL/PixReco <tag> PixelOfflineReco-Test-00</tag> " ]      
  else:  
    conddb.addFolder(MYDBTYPE,"/PIXEL/PixReco <tag>"+MYTAG+"</tag>")
    
# Just the pixel and SCT
DetFlags.detdescr.pixel_setOn()
#DetFlags.detdescr.SCT_setOn()

# Select the geometry version.
from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetDescrVersion='ATLAS-CSC-02-00-00'
if isIBL:
  globalflags.DetDescrVersion = 'ATLAS-IBL3D25-03-00-00'
else:  
  globalflags.DetDescrVersion = 'ATLAS-GEO-08-00-00'

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

# This line can be excluded and it will by default connect to SQlite file mycool.db
# IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=mycool.db;X:OFLP200"
#include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )

if dowrite:
  from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
  if isIBL:
    IOVDbSvc.dbConnection = "sqlite://X;schema=myofflinedb_ibl.db;dbname=OFLP200"
  else:
    IOVDbSvc.dbConnection = "sqlite://X;schema=myofflinedb.db;dbname=OFLP200"
  from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
  myOCA=OutputConditionsAlg(outputFile="dummy.root")
  myOCA.ObjectList = [ "DetCondCFloat#/PIXEL/PixReco"]
  myOCA.WriteIOV=True
  myOCA.IOVTagList= [MYTAG]

# Load algorithms Any algorithm that uses the tool will do
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTestWriteRead
topSequence += PixelRecoDbTestWriteRead()
topSequence.PixelRecoDbTestWriteRead.Read = doread
topSequence.PixelRecoDbTestWriteRead.Write = dowrite

from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
ToolSvc += PixelRecoDbTool()
ToolSvc.PixelRecoDbTool.OutputLevel = VERBOSE
ToolSvc.PixelRecoDbTool.InputSource = constantssource
ToolSvc.PixelRecoDbTool.PixelClusterErrorDataFile = errorsfileold
ToolSvc.PixelRecoDbTool.PixelChargeInterpolationDataFile = constantsfile
ToolSvc.PixelRecoDbTool.PixelClusterOnTrackErrorDataFile = errorsfilenew
ToolSvc.PixelRecoDbTool.DumpConstants = 1

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
#MessageSvc.OutputLevel      = INFO
MessageSvc.OutputLevel      = INFO

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1

MessageSvc.defaultLimit = 9999999
#==============================================================
#
# End of job options file
#
###############################################################
