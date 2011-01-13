# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigT1RPCRecRoiConfig.py 301203 2010-05-26 12:08:22Z krasznaa $
#
# To use the LVL1RPC::RPCRecRoiSvc, one has to import this module simply
# like:
#
#   import TrigT1RPCRecRoiSvc.TrigT1RPCRecRoiConfig
#

from TrigT1RPCRecRoiSvc.TrigT1RPCRecRoiSvcConf import *
from AthenaCommon.AppMgr import ServiceMgr

# Create a logger:
from AthenaCommon.Logging import logging
logger = logging.getLogger( "TrigT1RPCRecRoiConfig.py" )

class RPCRecRoiConfig( LVL1RPC__RPCRecRoiSvc ):

    def __init__( self, name = Configurable.DefaultName ):
        super( RPCRecRoiConfig, self ).__init__( name )

# Configure the cabling. Let the cabling configurable set up the services based
# on the conditions.
import MuonCnvExample.MuonCablingConfig
logger.debug( "Configured the cabling services" )

# Add the RPCRecRoiSvc to the service manager:
RPCRecRoiSvc = RPCRecRoiConfig()
ServiceMgr += RPCRecRoiSvc
logger.info( "LVL1RPC::RPCRecRoiSvc is now configured" )
