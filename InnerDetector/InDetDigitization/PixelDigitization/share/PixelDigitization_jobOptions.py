#
# block multiple inclusions
#
include.block("PixelDigitization/PixelDigitization_jobOptions.py")
#
from GaudiKernel.GaudiHandles import ServiceHandle

# make sure ToolSvc is there...
if not "ToolSvc"         in theApp.ExtSvc and \
   not "ToolSvc/ToolSvc" in theApp.ExtSvc:
    theApp.ExtSvc += [ "ToolSvc/ToolSvc"]
    pass

from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from AthenaCommon import CfgGetter
if 'doSplitDigi' in jobproperties.Digitization.experimentalDigi():
    job += CfgGetter.getAlgorithm("PixelDigitizationHS")
    job += CfgGetter.getAlgorithm("PixelDigitizationPU")
    pix=ToolSvc.PixelDigitizationToolHS
    pix2=ToolSvc.PixelDigitizationToolPU
    print "Number Of Collisions per beam crossing", digitizationFlags.numberOfCollisions.get_Value()
    print "************************************************************************************************************"
    print pix
    print pix2
else:
    # this will create one digitization with all default properties
    job += CfgGetter.getAlgorithm("PixelDigitization/PixelDigitization", tryDefaultConfigurable=True)
    pix = ToolSvc.PixelDigitizationTool
    print "Number Of Collisions per beam crossing", digitizationFlags.numberOfCollisions.get_Value()
    print "************************************************************************************************************"
    print pix



