#
# Joboptions for loading the AthenaSealSvc
#

# Block multiple includes
include.block( "AthenaSealSvc/AthenaSealSvc_joboptions.py" )

# general application configuration options

## get a handle to the Service Manager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon import CfgMgr

if not hasattr (svcMgr, 'AthenaSealSvc'):
    svcMgr += CfgMgr.AthenaSealSvc()

## put here additional configuration

##

from AthenaCommon.Logging import logging
msg = logging.getLogger( 'AthenaSealSvc' )
msg.info( "DictNames: %r", svcMgr.AthenaSealSvc.DictNames )

## backward compat
AthenaSealSvc = svcMgr.AthenaSealSvc

