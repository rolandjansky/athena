# now add config class to algsequence()
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# this will create one digitization with all default properties
from AthenaCommon import CfgGetter
job.PileUpToolsAlg.PileUpTools += [ CfgGetter.getPrivateTool("BCM_DigitizationTool", checkType=True) ]

