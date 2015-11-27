#
# block multiple inclusions
#
include.block("FastSiDigitization/PixelDigitization_jobOptions.py")

include( "PixelConditionsServices/PixelDCSSvc_jobOptions.py" )
#
from GaudiKernel.GaudiHandles import ServiceHandle
#
# Import Digitization job properties
#
from Digitization.DigitizationFlags import digitizationFlags

# Get ToolSvc and ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr

# make sure ToolSvc is there...
if not "ToolSvc"         in theApp.ExtSvc and \
   not "ToolSvc/ToolSvc" in theApp.ExtSvc:
    theApp.ExtSvc += [ "ToolSvc/ToolSvc"]
    pass

from IOVDbSvc.CondDB import conddb


from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
ToolSvc += PixelRecoDbTool()
ToolSvc.PixelRecoDbTool.InputSource = 1

from PixelConditionsTools.PixelConditionsToolsConf import PixelCalibDbTool
ToolSvc += PixelCalibDbTool()

if not conddb.folderRequested('/PIXEL/PixCalib'):
  conddb.addFolder("PIXEL_OFL","/PIXEL/PixCalib")
if not conddb.folderRequested('/PIXEL/ReadoutSpeed'):
  conddb.addFolder("PIXEL_OFL","/PIXEL/ReadoutSpeed")
from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibSvc
InDetPixelCalibSvc = PixelCalibSvc()
ServiceMgr += InDetPixelCalibSvc



from FastSiDigitization.FastSiDigitizationConf import PixelFastDigitization

# now add config class to algsequence()
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
if digitizationFlags.doXingByXingPileUp():
    from FastSiDigitization.FastSiDigitizationConf import PixelFastDigitizationTool
    job.PileUpToolsAlg.PileUpTools += [ PixelFastDigitizationTool() ]
    pix = job.PileUpToolsAlg.PileUpTools[ "PixelFastDigitizationTool" ]
else:
    job += PixelFastDigitization("PixelFastDigitization")
    pix = job.PixelFastDigitization.DigitizationTool


#
# set the random service and stream name
#
#pix.OutputLevel = VERBOSE
rndmStream   = "PixelDigitization"
#pix.RndmSvc     = digitizationRndmSvc
pix.RndmSvc = digitizationFlags.rndmSvc.get_Value()
pix.RndmEngine  = rndmStream
#
digitizationFlags.rndmSeedList.addSeed(rndmStream, 10513239, 492615104 )
#

### block multiple inclusions
##include.block("FastSiDigitization/PixelDigitization_jobOptions.py")
### Import Digitization job properties
##from Digitization.DigitizationFlags import digitizationFlags
### now add config class to algsequence()
##from AthenaCommon.AlgSequence import AlgSequence
##job = AlgSequence()
##if digitizationFlags.doXingByXingPileUp():
##    job.PileUpToolsAlg.PileUpToosl += [ "PixelFastDigitizationTool" ]
##else:
##    from AthenaCommon.CfgGetter import getAlgorithm
##    job += getAlgorithm("PixelFastDigitization", tryDefaultConfigurable=True)
