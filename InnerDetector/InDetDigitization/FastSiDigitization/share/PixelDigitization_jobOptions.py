#
# block multiple inclusions
#
include.block("FastSiDigitization/PixelDigitization_jobOptions.py")

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

#################################
# Config pixel conditions setup #
#################################
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
if not hasattr(condSeq, 'PixelConfigCondAlg'):
  from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelConfigCondAlg
  condSeq += PixelConfigCondAlg(name="PixelConfigCondAlg",ReadDeadMapKey="")


useNewChargeFormat  = False
useNewDeadmapFormat = False

if useNewDeadmapFormat:
  if not hasattr(condSeq, "PixelDeadMapCondAlg"):
    from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDeadMapCondAlg
    condSeq += PixelDeadMapCondAlg(name="PixelDeadMapCondAlg",ReadKey="")

#####################
# Calibration setup #
#####################
from IOVDbSvc.CondDB import conddb
if not useNewChargeFormat:
  if not conddb.folderRequested("/PIXEL/PixCalib"):
    conddb.addFolder("PIXEL_OFL", "/PIXEL/PixCalib", className="CondAttrListCollection")
  if not hasattr(condSeq, 'PixelChargeCalibCondAlg'):
    from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelChargeCalibCondAlg
    condSeq += PixelChargeCalibCondAlg(name="PixelChargeCalibCondAlg", ReadKey="/PIXEL/PixCalib")
else:
  if not conddb.folderRequested("/PIXEL/ChargeCalibration"):
    conddb.addFolder("PIXEL_OFL", "/PIXEL/ChargeCalibration", className="CondAttrListCollection")
  if not hasattr(condSeq, 'PixelChargeLUTCalibCondAlg'):
    from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelChargeLUTCalibCondAlg
    condSeq += PixelChargeLUTCalibCondAlg(name="PixelChargeLUTCalibCondAlg", ReadKey="/PIXEL/ChargeCalibration")

from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
ToolSvc += PixelRecoDbTool()
ToolSvc.PixelRecoDbTool.InputSource = 1

if not conddb.folderRequested('/PIXEL/ReadoutSpeed'):
  conddb.addFolder("PIXEL_OFL","/PIXEL/ReadoutSpeed")


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
