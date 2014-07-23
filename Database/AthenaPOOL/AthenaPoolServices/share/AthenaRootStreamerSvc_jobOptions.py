## @file AthenaRootStreamerSvc_jobOptions.py
## @brief 


include.block ( "AthenaPoolServices/AthenaRootStreamerSvc_jobOptions.py" )

## get a handle on the Service Manager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

##
##  Load dll for AthenaRootStreamer Service
##
from AthenaPoolServices.AthenaRootStreamerSvcConf import AthenaRootStreamerSvc
svcMgr += AthenaRootStreamerSvc()
