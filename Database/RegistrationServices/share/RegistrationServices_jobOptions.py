## @file RegistrationServices_jobOptions.py
## @brief Description of properties of RegistrationServices


##
##  Load dll for RegistrationServices
##
include.block ( "RegistrationServices/RegistrationServices_jobOptions.py" )

from AthenaCommon.AppMgr import theApp
theApp.Dlls += [ "RegistrationServices" ]
