#**************************************************************
#
# HLT common configuration file: TrigServicesCommonEnd.py
#
#==============================================================

## get a handle on the ServiceMgr
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## HLT standard service configuration 
from TrigServices.TriggerUnixStandardSetup import setupHLTServicesEnd
setupHLTServicesEnd()
## clean-up: avoid running multiple times this method
del setupHLTServicesEnd
#==============================================================
#
# End of HLT common configuration file: TrigServicesCommonEnd.py
#
#**************************************************************
