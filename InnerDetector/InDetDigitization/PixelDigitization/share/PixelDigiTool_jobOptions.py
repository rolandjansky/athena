#
# block multiple inclusions
#
include.block("PixelDigitization/PixelDigiTool_jobOptions.py")
#
from GaudiKernel.GaudiHandles import ServiceHandle
#
# Import Digitization job properties
#
from Digitization.DigitizationFlags import digitizationFlags

# make sure ToolSvc is there...
if not "ToolSvc"         in theApp.ExtSvc and \
   not "ToolSvc/ToolSvc" in theApp.ExtSvc:
    theApp.ExtSvc += [ "ToolSvc/ToolSvc"]
    pass


#
# now add config class to algsequence()
#
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from AthenaCommon import CfgGetter
if 'doSplitDigi' in jobproperties.Digitization.experimentalDigi():
    job.PileUpToolsAlg.PileUpTools += [CfgGetter.getPrivateTool("PixelDigitizationToolHS", checkType=True)]
    job.PileUpToolsAlg.PileUpTools += [CfgGetter.getPrivateTool("PixelDigitizationToolPU", checkType=True)]
    pix=job.PileUpToolsAlg.PileUpTools['PixelDigitizationToolHS']
    pix2=job.PileUpToolsAlg.PileUpTools['PixelDigitizationToolPU']
    print pix
    print pix2
else:
    # this will create one digitization tool with all default properties
    job.PileUpToolsAlg.PileUpTools += [CfgGetter.getPrivateTool("PixelDigitizationTool", checkType=True)]
    pix=job.PileUpToolsAlg.PileUpTools['PixelDigitizationTool']
    print job.PileUpToolsAlg.PileUpTools['PixelDigitizationTool']



