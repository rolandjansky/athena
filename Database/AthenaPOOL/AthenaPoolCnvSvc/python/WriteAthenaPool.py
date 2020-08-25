# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

## @file WriteAthenaPool_jobOptions.py
## @brief AthenaPool job options file for writing event objects.
## @author Peter van Gemmeren <gemmeren@bnl.gov>
###############################################################
#
# This is an jobOptions for writing event objects in AthenaPool.
# It loads the conversion service shared lib, and the EventInfo converter.
# Each subsystem should provide additional jobOpt fragments to load converter and append
# to event object ItemList. For example, LArAthenaPool should include:
#
# ApplicationMgr.Dlls += [ "LArAthenaPool" ];
# Stream1.ItemList += [ "2701#*", "2731#*" ];
#
#==============================================================

## import the default class to create output streams
from OutputStreamAthenaPool.CreateOutputStreams import AthenaPoolOutputStream

## export the default class to create output streams
__all__ = [ 'AthenaPoolOutputStream' ]

def _configureWriteAthenaPool():
    """ Helper method to configure Athena to write out POOL files """

    from AthenaCommon.Logging import logging
    msg = logging.getLogger( 'configureWriteAthenaPool' )
    msg.debug( "Configuring Athena for writing POOL files..." )

    # Load the basic services
    import AthenaPoolCnvSvc.AthenaPool

    from AthenaCommon.AppMgr  import ServiceMgr as svcMgr
    # Switch off splitting by setting default SplitLevel to 0
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DEFAULT_SPLITLEVEL ='0'" ]
    # Set as default the member-wise streaming, ROOT default
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "STREAM_MEMBER_WISE = '1'" ]

    # Increase default BasketSize to 32K, ROOT default (but overwritten by POOL)
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DEFAULT_BUFFERSIZE = '32000'" ]

    # Turn off auto_flush for DataHeader container to avoid basket optimization
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "ContainerName = 'POOLContainer(DataHeader)'; BRANCH_BASKET_SIZE = '256000'" ]
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "ContainerName = 'POOLContainerForm(DataHeaderForm)'; BRANCH_BASKET_SIZE = '1024000'" ]
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "ContainerName = 'TTree=POOLContainerForm(DataHeaderForm)'; CONTAINER_SPLITLEVEL = '99'" ]

    svcMgr.AthenaPoolCnvSvc.TopLevelContainerName = ""
    svcMgr.AthenaPoolCnvSvc.SubLevelBranchName = "<type>/<key>"

    msg.debug( "Configuring Athena for writing POOL files... [DONE]" )
    return

## configuration at module import
_configureWriteAthenaPool()

## clean-up: avoid running multiple times this method
del _configureWriteAthenaPool
