# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @file IOVRegistrationSvc.py
## @brief Job options file for iov registration service setup
## @author RD Schaffer <R.D.Schaffer@cern.ch>
###############################################################
#
# Job options file for iov registration service setup
#
#==============================================================
#

def _loadBasicIOVRegistrationSvc():
    """Loads the basic services for IOVRegistrationSvc"""

    ## make sure we have been -at least- minimally correctly configured
    import AthenaCommon.AtlasUnixStandardJob   # noqa: F401

    # add in service
    from AthenaCommon.AppMgr import ServiceMgr
    from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
    ServiceMgr += IOVRegistrationSvc()


## load basic services configuration at module import
_loadBasicIOVRegistrationSvc()

## clean-up: avoid running multiple times this method
del _loadBasicIOVRegistrationSvc

