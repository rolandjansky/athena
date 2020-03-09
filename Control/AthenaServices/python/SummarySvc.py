# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# File: AthenaServices/python/SummarySvc.py
# Author: Charles Leggett
# Created: 11/04/09

# This provides an easy way to use the AthenaSummarySvc, which requires
# replacing the MessageSvc with the LoggedMessageSvc. Since the MessageSvc
# is created pretty early on, this is no easy task.

# usage:
#
#   from AthenaServices.SummarySvc import *
#   useAthenaSummarySvc()
#

def useMessageLogger():
  from AthenaCommon.AppMgr import theApp
  theApp.MessageSvcType = 'LoggedMessageSvc'
  import AthenaCommon.ConfigurableDb as ConfDb

  from AthenaCommon.AppMgr import ServiceMgr as svcMgr

  from AthenaCommon.Configurable import Configurable

  import logging
  log = logging.getLogger( 'LoggedMessageSvc' )
  oldMsgSvc = svcMgr.MessageSvc
  del Configurable.allConfigurables[ oldMsgSvc.name() ]
  newMsgSvc = ConfDb.getConfigurable( theApp.MessageSvcType )( oldMsgSvc.name() )
  for name, value in svcMgr.MessageSvc.getValuedProperties().items():
    try:
      setattr( newMsgSvc, name, value )
    except Exception:
      log.info( 'When configuring LoggedMessageSvc, no Property \"%s\" found',name )
  del svcMgr.MessageSvc
  svcMgr += newMsgSvc

def useAthenaSummarySvc():
  from AthenaCommon.AppMgr import theApp
  from AthenaCommon.AppMgr import ServiceMgr as svcMgr
  theApp.CreateSvc += ["AthenaSummarySvc"]

  from AthenaCommon.OldStyleConfig import  Service
  svcMgr += Service("AthenaSummarySvc")

  useMessageLogger()
