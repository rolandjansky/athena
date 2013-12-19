## @file IOVRegistrationSvc_jobOptions.py
## @brief Description of properties of IOVRegistrationSvc


##
##  Documentation of IOVRegistrationSvc properties
##
include.block ( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )

# Load general jobopts
include ( "RegistrationServices/RegistrationServices_jobOptions.py" )

from AthenaCommon.AppMgr import ServiceMgr
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
ServiceMgr += IOVRegistrationSvc()

#
#  Definitions and default values 
#

# Min and max for run and LB numbers
IOVRunNumberMin          = 0
IOVRunNumberMax          = 2147483647 #0x7FFFFFFF
IOVLBNumberMin        = 0
IOVLBNumberMax        = 4294967295 #0xFFFFFFFF

#RegSvc = Service ( "IOVRegistrationSvc" )

# Should the folders requested be recreated/reinitialized
#RegSvc.RecreateFolders  = False

# Start/stop of IOV
#RegSvc.BeginRun         = 0

# Start/stop of IOV
#RegSvc.EndRun           = 2147483647 #0x7FFFFFFF

# Start/stop of IOV
#RegSvc.BeginLB       = 0

# Start/stop of IOV
#RegSvc.EndLB         = 4294967295 #0xFFFFFFFF

# Default tag to use
#RegSvc.IOVDbTag         = ""

# Use timestamp for type of IOV
#RegSvc.IOVDbTimeStamp   = False

