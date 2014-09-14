include.block( "TrigSteering/jobOfragment_CONFIG_New.py" )

include("TrigConfigSvc/jobOptions_commonSetup.py")
include("TrigConfigSvc/jobOptions_setupLVL1Svc.py")

########################
# HLTConfigSvc:
########################
from TrigConfigSvc.TrigConfigSvcConfig import DefaultHLTConfigSvc
HLTConfigSvc = DefaultHLTConfigSvc("HLTConfigSvc")
HLTConfigSvc.ConfigSource        = "XML"
HLTConfigSvc.OutputLevel         = VERBOSE

ServiceMgr += HLTConfigSvc

########################
# LVL1ConfigSvc:
########################

LVL1ConfigSvc.ConfigSource        = "XML"
LVL1ConfigSvc.OutputLevel         = VERBOSE
LVL1ConfigSvc.CreateLegacyObjects = False
