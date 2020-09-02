# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# To use the LVL1RPC::RPCRecRoiSvc, one has to import this module simply
# like:
#
#   import TrigT1RPCRecRoiSvc.TrigT1RPCRecRoiConfig
#

from TrigT1RPCRecRoiSvc.TrigT1RPCRecRoiSvcConf import LVL1RPC__RPCRecRoiSvc
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.Configurable import Configurable

# Create a logger:
from AthenaCommon.Logging import logging
logger = logging.getLogger( "TrigT1RPCRecRoiConfig.py" )

class RPCRecRoiConfig( LVL1RPC__RPCRecRoiSvc ):

    def __init__( self, name = Configurable.DefaultName ):
        super( RPCRecRoiConfig, self ).__init__( name )

# Configure the cabling. Let the cabling configurable set up the services based
# on the conditions.
import MuonCnvExample.MuonCablingConfig  # noqa: F401
logger.debug( "Configured the cabling services" )

# Add the RPCRecRoiSvc to the service manager:
RPCRecRoiSvc = RPCRecRoiConfig()
ServiceMgr += RPCRecRoiSvc
logger.info( "LVL1RPC::RPCRecRoiSvc is now configured" )
