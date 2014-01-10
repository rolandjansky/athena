from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from AthenaCommon import CfgGetter
from Digitization.DigitizationFlags import jobproperties
if jobproperties.Digitization.doXingByXingPileUp(): # PileUpTool approach 
    job.PileUpToolsAlg.PileUpTools += [ CfgGetter.getPrivateTool("TgcDigitizationTool", checkType=True) ]
else : # Algorithm approach
    job += CfgGetter.getAlgorithm("TGCDigitizer", tryDefaultConfigurable=True) 
