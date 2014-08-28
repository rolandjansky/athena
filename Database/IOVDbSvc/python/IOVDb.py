# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file IOVDb.py
## @brief Core job python configurable to setup IOVDbSvc
## @author RD Schaffer <R.D.Schaffer@cern.ch>
## $Id: IOVDb.py,v 1.4 2009-05-07 14:40:35 schaffer Exp $
###############################################################
#
# The core jobOptions for IOVDbSvc
# Setup the basic sercives
#
#==============================================================
#
# Required libs:

def _loadBasicIOVDb():
    """Loads the basic services for IOVDbSvc"""

    ## make sure we have been -at least- minimally correctly configured
    import AthenaCommon.AtlasUnixStandardJob

    from AthenaCommon.Logging import logging
    from AthenaCommon.AppMgr  import ServiceMgr as svcMgr

    msg = logging.getLogger( 'loadBasicIOVDb' )
    msg.debug( "Loading basic services for IOVDbSvc..." )

    # Load IOVDbSvc
    from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
    svcMgr += IOVDbSvc()

    # Set IOVDbSvc as proxy provider
    from AthenaCommon.ConfigurableDb import getConfigurable
    svcMgr += getConfigurable("ProxyProviderSvc")()
    svcMgr.ProxyProviderSvc.ProviderNames += [ "IOVDbSvc" ]

    # Load IOVSvc
    from IOVSvc.IOVSvcConf import IOVSvc
    svcMgr += IOVSvc()

    # Load EventInfoMgt
    import EventInfoMgt.EventInfoMgtInit

    # add in /TagInfo to be read from File meta data
    svcMgr.IOVDbSvc.Folders+=["/TagInfo<metaOnly/>"]

    msg.debug( "Loading basic services for IOVDb... [DONE]" )
    return

## load basic services configuration at module import
_loadBasicIOVDb()

## clean-up: avoid running multiple times this method
del _loadBasicIOVDb

