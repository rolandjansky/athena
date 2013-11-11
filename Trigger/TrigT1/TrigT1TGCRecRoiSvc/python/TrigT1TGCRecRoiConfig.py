# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigT1TGCRecRoiConfig.py 301207 2010-05-26 12:16:53Z krasznaa $
#
# To use the LVL1TGC::TGCRecRoiSvc, one has to import this module simply
# like:
#
#   import TrigT1TGCRecRoiSvc.TrigT1TGCRecRoiConfig
#

from TrigT1TGCRecRoiSvc.TrigT1TGCRecRoiSvcConf import *
from AthenaCommon.AppMgr import ServiceMgr

# Create a logger:
from AthenaCommon.Logging import logging
logger = logging.getLogger( "TrigT1TGCRecRoiConfig.py" )

class TGCRecRoiConfig( LVL1TGC__TGCRecRoiSvc ):

    def __init__( self, name = Configurable.DefaultName ):
        super( TGCRecRoiConfig, self ).__init__( name )

# Configure the cabling. Let the cabling configurable set up the services based
# on the conditions.
import MuonCnvExample.MuonCablingConfig
logger.debug( "Configured the cabling services" )

# Add the TGCRecRoiSvc to the service manager:
TGCRecRoiSvc = TGCRecRoiConfig()
ServiceMgr += TGCRecRoiSvc
logger.info( "LVL1TGC::TGCRecRoiSvc is now configured" )
