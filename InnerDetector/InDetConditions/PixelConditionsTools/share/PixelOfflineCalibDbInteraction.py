###############################################################
#
# Job options file to read charge interpolation constants from
# text file and output a new pool file and sqlite file
#
#==============================================================

if 'WRITEDB' in dir() and WRITEDB:
  dowrite=TRUE
  doread=FALSE
  constantssource=1
  constantsfile="WrittenConstants.txt"
  errorsfile="WrittenErrors.txt"
else:
  dowrite=FALSE
  doread=TRUE
  constantssource=2
  constantsfile="ReadConstants.txt"
  errorsfile="ReadErrors.txt"

if not 'MYTAG' in dir() or MYTAG=='':
  MYTAG="PixelOfflineReco-03"

if not 'MYDBTYPE' in dir() or MYDBTYPE!="PIXEL_OFL":
  #MYDBTYPE="PIXEL_OFL"
  MYDBTYPE="LOCAL"


if not 'MYRUN' in dir():
  MYRUN=0

#==============================================================

from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.DetFlags import DetFlags


#GlobalFlags.DetGeo.set_ctbh8()
GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DetGeo.set_commis()

GlobalFlags.DataSource.set_geant4()
#GlobalFlags.DataSource.set_data()

#include("IOVDbSvc/CondDBSetup.py")
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('DEFAULTCOND')
if doread:
  conddb.addFolder(MYDBTYPE,"/PIXEL/PixReco <tag>"+MYTAG+"</tag>")
  ServiceMgr.IOVDbSvc.forceRunNumber=MYRUN

# Just the pixel and SCT
DetFlags.detdescr.pixel_setOn()
#DetFlags.detdescr.SCT_setOn()

# Select the geometry version.
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion='ATLAS-CSC-02-00-00'

# Initialize geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion


# This line can be excluded and it will by default connect to SQlite file mycool.db
# IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=mycool.db;X:OFLP200"
#include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )


from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg(outputFile="dummy.root")
myOCA.ObjectList = [ "DetCondCFloat#/PIXEL/PixReco"]
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
ToolSvc.PixelRecoDbTool.PixelChargeInterpolationDataFile = constantsfile
ToolSvc.PixelRecoDbTool.PixelClusterOnTrackErrorDataFile = errorsfile
ToolSvc.PixelRecoDbTool.DumpConstants = 1

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = INFO
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1

#==============================================================
#
# End of job options file
#
###############################################################
