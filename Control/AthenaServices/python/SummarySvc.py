# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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

  oldMsgSvc = svcMgr.MessageSvc
  del Configurable.allConfigurables[ oldMsgSvc.name() ]
  newMsgSvc = ConfDb.getConfigurable( theApp.MessageSvcType )( oldMsgSvc.name() )
  for name, value in svcMgr.MessageSvc.getValuedProperties().items():
    setattr( newMsgSvc, name, value )
  del svcMgr.MessageSvc
  svcMgr += newMsgSvc
  MessageSvc = svcMgr.MessageSvc

def useAthenaSummarySvc():
  from AthenaCommon.AppMgr import theApp
  from AthenaCommon.AppMgr import ServiceMgr as svcMgr
  theApp.CreateSvc += ["AthenaSummarySvc"]

  from AthenaServices.AthenaServicesConf import AthenaSummarySvc
  from AthenaCommon.OldStyleConfig import  Service
  AthenaSummarySvc = Service("AthenaSummarySvc")

  svcMgr += AthenaSummarySvc

  useMessageLogger()
