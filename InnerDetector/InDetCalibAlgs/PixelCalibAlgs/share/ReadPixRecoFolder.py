################################################################
#
#    CONFIGURATION
#
################################################################

## name of the tag to read from database
if not 'MYTAG' in dir() or MYTAG=='':
 MYTAG="PixelOfflineReco-Extended-000-01"

## database connection
if not 'DATABASE' in dir() or DATABASE=='':
 DATABASE = "PIXEL_OFL"
## other useful examples:
 #DATABASE = "LOCAL"
#DATABASE = "mycool.db;dbname=COMP200"
#DATABASE = "mycool.db;dbname=OFLP200"

################################################################
#
#    JobOptions
#
################################################################

## use real data database
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DataSource.set_data()

## initialize athena 
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## configure tool to read stuff from database (i.e. write to text file!)
from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTestWriteRead
topSequence += PixelRecoDbTestWriteRead()
topSequence.PixelRecoDbTestWriteRead.Read = TRUE

## configure tool to access database
from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
ToolSvc += PixelRecoDbTool()
ToolSvc.PixelRecoDbTool.InputSource = 2
ToolSvc.PixelRecoDbTool.OutputLevel = DEBUG
ToolSvc.PixelRecoDbTool.PixelChargeInterpolationDataFile = \
	"PixelChargeInterpolationData-" + MYTAG.replace("PixelOfflineReco-","")
ToolSvc.PixelRecoDbTool.PixelClusterOnTrackErrorDataFile = \
	"PixelClusterOnTrackErrorData-" + MYTAG.replace("PixelOfflineReco-","") 
ToolSvc.PixelRecoDbTool.PixelClusterErrorDataFile = \
	"PixelClusterErrorData-" + MYTAG.replace("PixelOfflineReco-","") 

## configure tag and folder in database
from IOVDbSvc.CondDB import conddb
conddb.addFolder(DATABASE,"/PIXEL/PixReco <tag>"+MYTAG+"</tag>")

## Run athena!
theApp.EvtMax = 1
