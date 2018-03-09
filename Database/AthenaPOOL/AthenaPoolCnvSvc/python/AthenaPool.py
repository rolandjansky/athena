# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file AthenaPool_jobOptions.py
## @brief Core job options file for AthenaPOOL to setup the basic sercives.
## @author Peter van Gemmeren <gemmeren@bnl.gov>
## $Id: AthenaPool.py,v 1.10 2008-11-18 22:44:00 binet Exp $
###############################################################
#
# The core jobOptions for AthenaPOOL
# Setup the basic sercives
#
#==============================================================
#
# Required libs:

def _loadBasicAthenaPool():
    """Loads the basic services for AthenaPool"""

    from AthenaCommon import CfgMgr
    ## make sure we have been -at least- minimally correctly configured
    import AthenaCommon.AtlasUnixStandardJob

    from AthenaCommon.Logging import logging
    from AthenaCommon.AppMgr  import ServiceMgr as svcMgr

    msg = logging.getLogger( 'loadBasicAthenaPool' )
    msg.debug( "Loading basic services for AthenaPool..." )

    svcMgr += CfgMgr.PoolSvc()
    svcMgr.PoolSvc.MaxFilesOpen = 0
    #if in AthAnalysisBase, we will set the outputlevel of PoolSvc to ERROR, to silence warnings about missing reflex types
    #detect AthAnalysisBase by looking at the CMTEXTRATAGS env var, if it contains 'ManaCore' then we are in AthAnalysisBase
    import os
    from AthenaCommon.Constants import ERROR
    if "ManaCore" in os.environ.get('CMTEXTRATAGS',""): svcMgr.PoolSvc.OutputLevel=ERROR

    svcMgr += CfgMgr.AthenaPoolCnvSvc()

    """
    from AthenaCommon.AppMgr import theApp
    theApp.Dlls += [
        ## FIXME
        # needed: why isn't this guy being picked up through genmap ??
        "DBDataModelAthenaPoolPoolCnv", 
        ]
    """

    if not hasattr (svcMgr, 'EventPersistencySvc'):
        svcMgr += CfgMgr.EvtPersistencySvc( "EventPersistencySvc" )
    svcMgr.EventPersistencySvc.CnvServices += [ "AthenaPoolCnvSvc" ]
    if not hasattr (svcMgr, 'ProxyProviderSvc'):
        svcMgr += CfgMgr.ProxyProviderSvc()

    """
    #
    # Make sure AthenaSealSvc is loaded for dict check
    svcMgr += CfgMgr.AthenaSealSvc()

    #
    # Make sure AthenaPoolServices is loaded for custom streamer
    svcMgr += CfgMgr.AthenaRootStreamerSvc()

    # Load streamer allowing conversion of old CLHEP classes
    import AtlasSealCLHEP.OldCLHEPStreamers
    """

    # Add in MetaDataSvc
    svcMgr += CfgMgr.MetaDataSvc( "MetaDataSvc" )

    # Make MetaDataSvc an AddressProvider
    svcMgr.ProxyProviderSvc.ProviderNames += [ "MetaDataSvc" ]
    if not hasattr (svcMgr, 'MetaDataStore'):
        svcMgr += CfgMgr.StoreGateSvc( "MetaDataStore" )
 
    msg.debug( "Loading basic services for AthenaPool... [DONE]" )
    return

## load basic services configuration at module import
_loadBasicAthenaPool()

## clean-up: avoid running multiple times this method
del _loadBasicAthenaPool

