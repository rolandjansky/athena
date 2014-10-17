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

# Setup the DCS folders and Svc used in the sctSiliconConditionsSvc
if not conddb.folderRequested('/SCT/DCS/CHANSTAT'):
    conddb.addFolder("DCS_OFL","/SCT/DCS/CHANSTAT")
if not conddb.folderRequested('/SCT/DCS/MODTEMP'):
    conddb.addFolder("DCS_OFL","/SCT/DCS/MODTEMP")
if not conddb.folderRequested('/SCT/DCS/HV'):
    conddb.addFolder("DCS_OFL","/SCT/DCS/HV")

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsSvc
InDetSCT_DCSConditionsSvc = SCT_DCSConditionsSvc(name = "InDetSCT_DCSConditionsSvc")
ServiceMgr += InDetSCT_DCSConditionsSvc

# Silicon conditions service (set up by LorentzAngleSvcSetup)
from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SiliconConditionsSvc
ServiceMgr += SCT_SiliconConditionsSvc()
sctSiliconConditionsSvc=ServiceMgr.SCT_SiliconConditionsSvc

# Silicon properties service
from SiPropertiesSvc.SiPropertiesSvcConf import SiPropertiesSvc;
sctSiPropertiesSvc = SiPropertiesSvc(name = "SCT_SiPropertiesSvc",
                                     DetectorName="SCT",
                                     SiConditionsServices = sctSiliconConditionsSvc)
ServiceMgr += sctSiPropertiesSvc

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

