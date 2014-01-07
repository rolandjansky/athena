################################################################
#
#    CONFIGURATION
#
################################################################

## name of the tag to write in database
if not 'MYTAG' in dir() or MYTAG=='':
 MYTAG="PixelOfflineReco-NEWTAG"

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

## configure tool to write stuff in database (i.e. read from text file!)
from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTestWriteRead
topSequence += PixelRecoDbTestWriteRead()
topSequence.PixelRecoDbTestWriteRead.Write = TRUE

## configure tool to access database
from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
ToolSvc += PixelRecoDbTool()
ToolSvc.PixelRecoDbTool.InputSource = 1
ToolSvc.PixelRecoDbTool.OutputLevel = DEBUG
ToolSvc.PixelRecoDbTool.PixelChargeInterpolationDataFile = \
	"PixelChargeInterpolationData-" + MYTAG.replace("PixelOfflineReco-","")
ToolSvc.PixelRecoDbTool.PixelClusterOnTrackErrorDataFile = \
	"PixelClusterOnTrackErrorData-" + MYTAG.replace("PixelOfflineReco-","") 
ToolSvc.PixelRecoDbTool.PixelClusterErrorDataFile = \
	"PixelClusterErrorData-" + MYTAG.replace("PixelOfflineReco-","") 

## setting default conditions
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('DEFAULTCOND')

## configure conditions algorithms
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg()
myOCA.ObjectList = [ "DetCondCFloat#/PIXEL/PixReco"]
myOCA.IOVTagList= [MYTAG]

## Run athena!
theApp.EvtMax = 1
