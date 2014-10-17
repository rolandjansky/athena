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
from Digitization.DigitizationFlags import jobproperties

# make sure ToolSvc is there...
if not "ToolSvc"         in theApp.ExtSvc and \
   not "ToolSvc/ToolSvc" in theApp.ExtSvc:
    theApp.ExtSvc += [ "ToolSvc/ToolSvc"]
    pass

from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
ToolSvc += PixelRecoDbTool()
ToolSvc.PixelRecoDbTool.InputSource = 1

from PixelConditionsTools.PixelConditionsToolsConf import PixelCalibDbTool
ToolSvc += PixelCalibDbTool()


# Get ToolSvc and ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr

from FastSiDigitization.FastSiDigitizationConf import PixelFastDigitization

# now add config class to algsequence()
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

job += PixelFastDigitization("PixelFastDigitization")
pix = job.PixelFastDigitization

#
# set the random service and stream name
#
rndmStream   = "PixelDigitization"
#pix.RndmSvc     = digitizationRndmSvc
from Digitization.DigitizationFlags import jobproperties
#pix.RndmSvc = jobproperties.Digitization.rndmSvc.get_Value()
#pix.RndmEngine  = rndmStream
#
jobproperties.Digitization.rndmSeedList.addSeed(rndmStream, 10513239, 492615104 )
#

