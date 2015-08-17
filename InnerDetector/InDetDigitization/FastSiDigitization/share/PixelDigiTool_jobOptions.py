# block multiple inclusions
include.block("FastSiDigitization/PixelDigiTool_jobOptions.py")
# Import Digitization job properties
from Digitization.DigitizationFlags import digitizationFlags
# now add config class to algsequence()
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
if digitizationFlags.doXingByXingPileUp():
    job.PileUpToolsAlg.PileUpToosl += [ "PixelFastDigitizationTool" ]
else:
    from AthenaCommon.CfgGetter import getAlgorithm
    job += getAlgorithm("PixelFastDigitization", tryDefaultConfigurable=True)
