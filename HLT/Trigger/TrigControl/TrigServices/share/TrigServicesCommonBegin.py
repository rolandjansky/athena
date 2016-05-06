#**************************************************************
#
# HLT common configuration file: TrigServicesCommonBegin.py
#
#==============================================================

include( "TrigServices/MessageSvc.py" )
include( "TrigServices/TrigServicesEventLoopMgr.py" )

## HLT standard service configuration
from TrigServices.TriggerUnixStandardSetup import setupHLTServicesBegin
setupHLTServicesBegin()
## clean-up: avoid running multiple times this method
del setupHLTServicesBegin

#==============================================================
#
# End of HLT common configuration file: TrigServicesCommonBegin.py
#
#**************************************************************
