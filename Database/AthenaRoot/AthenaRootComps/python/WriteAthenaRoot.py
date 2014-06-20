# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file AthenaRootComps/python/WriteAthenaRoot.py
# @purpose make the Athena framework write a set of ROOT files
# @author Sebastien Binet <binet@cern.ch>
# 

## import the default class to create output streams
from AthenaRootComps.Configurables import createNtupleOutputStream

## export the default class to create output streams
__all__ = [ 'createNtupleOutputStream' ]

def _configureWriteAthenaRoot():
    """ Helper method to configure Athena to write out ROOT files """

    from AthenaCommon.Logging import logging
    msg = logging.getLogger( 'configureWriteAthenaRoot' )
    msg.debug( "Configuring Athena for writing ROOT files..." )

    # Load the basic services
    import AthenaRootComps.AthenaRootBase

    '''
    from AthenaCommon.AppMgr  import ServiceMgr as svcMgr
    # Switch off splitting by setting default SplitLevel to 0
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DEFAULT_SPLITLEVEL ='0'" ]
    # Set as default the member-wise streaming, ROOT default
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "STREAM_MEMBER_WISE = '1'" ]

    # Increase default BasketSize to 32K, ROOT default (but overwritten by POOL)
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DEFAULT_BUFFERSIZE = '32000'" ]

    # Turn off auto_flush for DataHeader container to avoid basket optimization
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "ContainerName = 'POOLContainer_DataHeader_p5'; TREE_AUTO_FLUSH = '0'" ]
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "ContainerName = 'POOLContainer_DataHeaderForm_p5'; TREE_AUTO_FLUSH = '0'" ]
    '''
    
    msg.debug( "Configuring Athena for writing ROOT files... [DONE]" )
    return

## configuration at module import
_configureWriteAthenaRoot()

## clean-up: avoid running multiple times this method
del _configureWriteAthenaRoot
