# block multiple inclusions
include.block("FastSiDigitization/SCT_Digitization_jobOptions.py")
# Import Digitization job properties
from Digitization.DigitizationFlags import digitizationFlags
# now add config class to algsequence()
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
if digitizationFlags.doXingByXingPileUp():
    job.PileUpToolsAlg.PileUpTools += [ "SCT_FastDigitizationTool" ]
else:
    from AthenaCommon.CfgGetter import getAlgorithm
    job += getAlgorithm("SCT_FastDigitization", tryDefaultConfigurable=True)
