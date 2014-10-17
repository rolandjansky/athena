#
# block multiple inclusions
#
include.block("FastSiDigitization/PixelDigiTool_jobOptions.py")

include( "PixelConditionsServices/PixelDCSSvc_jobOptions.py" )
#
from GaudiKernel.GaudiHandles import ServiceHandle
from PixelDigitization.JobOptCfg import CustomPixelDigitization
#
# Import Digitization job properties
#
from Digitization.DigitizationFlags import jobproperties

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

# this will create one digitization tool with all default properties
from FastSiDigitization.JobOptCfg import CustomPixelDigitizationTool
#from PixelDigitization.PixelDigitizationConf import PixelDigitizationTool
job.PileUpToolsAlg.PileUpTools += [ CustomPixelDigitizationTool("PixelFastDigitizationTool") ]
pix = job.PileUpToolsAlg.PileUpTools[ "PixelFastDigitizationTool" ]
#xing times in ns
from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Beam.estimatedLuminosity()> 0.5e33: 
    #this is the time of the xing in ns 
    pix.FirstXing = -25
    if jobproperties.Beam.bunchSpacing.get_Value() > 50 :
        # rather than using an equality
        pix.LastXing = 75
    else :
        pix.LastXing = 25
else :
    pix.FirstXing = -50
    pix.LastXing = 100

#pix.OutputLevel = DEBUG

pix.InputObjectName = "PixelHits"

# set the random service, stream name, seeds
rndmStream   = "PixelDigitization"
from Digitization.DigitizationFlags import jobproperties
pix.RndmSvc = jobproperties.Digitization.rndmSvc()
pix.RndmEngine  = rndmStream
jobproperties.Digitization.rndmSeedList.addSeed(rndmStream, 10513239, 492615104 )

