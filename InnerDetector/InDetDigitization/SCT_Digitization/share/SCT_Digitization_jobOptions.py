## NB This file is only kept for backward compatibility. The main
## SCT_Digitization configuration code can now be found in
## SCT_Digitization/python/SCT_DigitizationConfig.py

# Import Digitization job properties 
from Digitization.DigitizationFlags import digitizationFlags
# now add config class to algsequence()
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon import CfgGetter
job = AlgSequence()
if 'doSplitDigi' in jobproperties.Digitization.experimentalDigi():
   if jobproperties.Digitization.doXingByXingPileUp():
      job.PileUpToolsAlg.PileUpTools += [ CfgGetter.getPrivateTool("SCT_DigitizationToolHS", checkType=True) ]
      job.PileUpToolsAlg.PileUpTools += [ CfgGetter.getPrivateTool("SCT_DigitizationToolPU", checkType=True) ]
   else:
      job += CfgGetter.getAlgorithm("SCT_DigitizationHS")
      job += CfgGetter.getAlgorithm("SCT_DigitizationPU")
else:
   if jobproperties.Digitization.doXingByXingPileUp():
      # this will create one digitization tool with all default properties
      job.PileUpToolsAlg.PileUpTools += [ CfgGetter.getPrivateTool("SCT_DigitizationTool", checkType=True) ]
   else:
      # this will create one digitization with all default properties
      job += CfgGetter.getAlgorithm("SCT_Digitization/SCT_Digitization", tryDefaultConfigurable=True)
