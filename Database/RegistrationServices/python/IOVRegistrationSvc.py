# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file IOVRegistrationSvc.py
## @brief Job options file for iov registration service setup
## @author RD Schaffer <R.D.Schaffer@cern.ch>
## $Id: IOVRegistrationSvc.py,v 1.1 2007-07-13 17:22:53 schaffer Exp $
###############################################################
#
# Job options file for iov registration service setup
#
#==============================================================
#

def _loadBasicIOVRegistrationSvc():
    """Loads the basic services for IOVRegistrationSvc"""

    ## make sure we have been -at least- minimally correctly configured
    import AthenaCommon.AtlasUnixStandardJob

    # add in service
    from AthenaCommon.AppMgr import ServiceMgr
    from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
    ServiceMgr += IOVRegistrationSvc()

    #
    #  Definitions and default values 
    #
    
    # Min and max for run and LB numbers
    IOVRunNumberMin       = 0
    IOVRunNumberMax       = 2147483647 #0x7FFFFFFF
    IOVLBNumberMin        = 0
    IOVLBNumberMax        = 4294967295 #0xFFFFFFFF

## load basic services configuration at module import
_loadBasicIOVRegistrationSvc()

## clean-up: avoid running multiple times this method
del _loadBasicIOVRegistrationSvc

