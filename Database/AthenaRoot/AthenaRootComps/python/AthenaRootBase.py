# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file AthenaRootComps/python/AthenaRootBase.py
# @purpose make the Athena framework write a set of ROOT files
# @author Sebastien Binet <binet@cern.ch>
# 


def _loadBasicAthenaRoot():
     """Loads the basic services for AthenaRoot"""

     from AthenaCommon import CfgMgr
     ## make sure we have been -at least- minimally correctly configured
     import AthenaCommon.AtlasUnixStandardJob

     from AthenaCommon.Logging import logging
     from AthenaCommon.AppMgr  import ServiceMgr as svcMgr

     msg = logging.getLogger( 'loadBasicAthenaRoot' )
     msg.debug( "Loading basic services for AthenaRoot..." )

     svcMgr += CfgMgr.Athena__RootSvc("AthenaRootSvc")
     svcMgr += CfgMgr.Athena__RootCnvSvc("AthenaRootCnvSvc")

     from AthenaCommon.AppMgr import theApp

     if not hasattr (svcMgr, 'EventPersistencySvc'):
         svcMgr += CfgMgr.EvtPersistencySvc( "EventPersistencySvc" )
         svcMgr.EventPersistencySvc.CnvServices += [ "AthenaRootCnvSvc" ]
     if not hasattr (svcMgr, 'ProxyProviderSvc'):
         svcMgr += CfgMgr.ProxyProviderSvc()
            
     # Make sure AthenaPoolServices is loaded for custom streamer
     try:
          svcMgr += CfgMgr.AthenaRootStreamerSvc()
     except TypeError:
          msg.info("could not load AthenaRootStreamerSvc")
          pass
     
     if not hasattr (svcMgr, 'InputMetaDataStore'):
          svcMgr += CfgMgr.StoreGateSvc ("InputMetaDataStore")
          pass
     
     if not hasattr (svcMgr, 'MetaDataStore'):
          svcMgr += CfgMgr.StoreGateSvc ("MetaDataStore")
          pass
     
     msg.debug( "Loading basic services for AthenaRoot... [DONE]" )
     return

## load basic services configuration at module import
_loadBasicAthenaRoot()

## clean-up: avoid running multiple times this method
del _loadBasicAthenaRoot
 
                
