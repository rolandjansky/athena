#
# block multiple inclusions
#
include.block("FastSiDigitization/SiSmearing_jobOptions.py")

include( "PixelConditionsServices/PixelDCSSvc_jobOptions.py" )
#
from GaudiKernel.GaudiHandles import ServiceHandle
#
# Import Digitization job properties
#
from Digitization.DigitizationFlags import jobproperties

# Get ToolSvc and ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr

# make sure ToolSvc is there...
if not "ToolSvc"         in theApp.ExtSvc and \
   not "ToolSvc/ToolSvc" in theApp.ExtSvc:
    theApp.ExtSvc += [ "ToolSvc/ToolSvc"]
    pass

# Import Beam job properties
from AthenaCommon.BeamFlags import jobproperties

# # Setup the DCS folders and tool used in the sctSiliconConditionsTool
# from SCT_ConditionsTools.SCT_DCSConditionsToolSetup import SCT_DCSConditionsToolSetup
# sct_DCSConditionsToolSetup = SCT_DCSConditionsToolSetup()
# sct_DCSConditionsToolSetup.setup()

# # Silicon conditions tool
# from SCT_ConditionsTools.SCT_SiliconConditionsToolSetup import SCT_SiliconConditionsToolSetup
# sct_SiliconConditionsToolSetup = SCT_SiliconConditionsToolSetup()
# sct_SiliconConditionsToolSetup.setDcsTool(sct_DCSConditionsSvcSetup.getTool())
# sct_SiliconConditionsToolSetup.setup()

# # Silicon properties tool
# from SiPropertiesTool.SCT_SiPropertiesToolSetup import SCT_SiPropertiesToolSetup
# sct_SiPropertiesToolSetup = SCT_SiPropertiesToolSetup()
# sct_SiPropertiesToolSetup.setSiliconTool(sct_SiliconConditionsToolSetup.getTool())
# sct_SiPropertiesToolSetup.setup()
# sctSiPropertiesTool = sct_SiPropertiesToolSetup.getTool()

from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
ToolSvc += PixelRecoDbTool()
ToolSvc.PixelRecoDbTool.InputSource = 1

from PixelConditionsTools.PixelConditionsToolsConf import PixelCalibDbTool
ToolSvc += PixelCalibDbTool()

from IOVDbSvc.CondDB import conddb
conddb.addFolder("PIXEL_OFL","/PIXEL/PixCalib")

# setup PixelCalibDbTool in ToolSvc
ToolSvc.PixelCalibDbTool.CalibFolder ="/PIXEL/PixCalib"
ToolSvc.PixelCalibDbTool.CalibLocation ="PixCalibKey"
ToolSvc.PixelCalibDbTool.WriteDB =False

from FastSiDigitization.FastSiDigitizationConf import SiSmearedDigitization

# now add config class to algsequence()
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

job += SiSmearedDigitization("PixelSmearedDigitization")
pix = job.PixelSmearedDigitization

job += SiSmearedDigitization("SCTSmearedDigitization")
pix = job.SCTSmearedDigitization

#
# set the random service and stream name
#
rndmStream   = "SiSmearedDigitization"
#pix.RndmSvc     = digitizationRndmSvc
from Digitization.DigitizationFlags import jobproperties
#pix.RndmSvc = jobproperties.Digitization.rndmSvc.get_Value()
#pix.RndmEngine  = rndmStream
#
jobproperties.Digitization.rndmSeedList.addSeed(rndmStream, 10513239, 492615104 )
#

