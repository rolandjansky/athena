# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# @file: TrigServicesConfig.py
# @purpose: customized configurables

from TrigServicesConf import TrigCOOLUpdateHelper as _TrigCOOLUpdateHelper
      
class TrigCOOLUpdateHelper(_TrigCOOLUpdateHelper):
   __slots__ = ()

   def __init__(self, name='TrigCOOLUpdateHelper'):
      super(TrigCOOLUpdateHelper, self).__init__(name)

      from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool
      self.MonTool = GenericMonitoringTool('MonTool')
      self.MonTool.defineHistogram('TIME_CoolFolderUpdate', path='EXPERT', type='TH1F',
                                   title='Time for conditions update;time [ms]',
                                   xbins=100, xmin=0, xmax=200)
      return

   def enable(self, folder='/TRIGGER/HLT/COOLUPDATE', tag=None):
      """Enable the COOL folder updates (only use this for data)"""
      
      from AthenaCommon.AppMgr import ServiceMgr as svcMgr
      if not hasattr(svcMgr,'IOVDbSvc'): return
      
      self.coolFolder = folder
      from IOVDbSvc.CondDB import conddb
      if tag is None:
         conddb.addFolder('TRIGGER',self.coolFolder)
      else:
         conddb.addFolderWithTag('TRIGGER',self.coolFolder,tag)
      

def setupMessageSvc():
   from AthenaCommon.AppMgr import theApp
   from AthenaCommon.AppMgr import ServiceMgr as svcMgr
   from AthenaCommon.Constants import VERBOSE, DEBUG, INFO, WARNING

   svcMgr.MessageSvc = theApp.service( "MessageSvc" )     # already instantiated
   MessageSvc = svcMgr.MessageSvc
   MessageSvc.OutputLevel = theApp.OutputLevel

   MessageSvc.Format       = "%t  % F%35W%S%4W%e%s%7W%R%T %0W%M"

   # Message suppression
   MessageSvc.enableSuppression    = False
   MessageSvc.suppressRunningOnly  = True
   MessageSvc.resetStatsAtBeginRun = True

   # 0 = no suppression, negative = log-suppression, positive = normal suppression
   # Do not rely on the defaultLimit property, always set each limit separately
   MessageSvc.defaultLimit = -100
   MessageSvc.verboseLimit = MessageSvc.defaultLimit
   MessageSvc.debugLimit   = MessageSvc.defaultLimit
   MessageSvc.infoLimit    = MessageSvc.defaultLimit
   MessageSvc.warningLimit = MessageSvc.defaultLimit
   MessageSvc.errorLimit   = 0
   MessageSvc.fatalLimit   = 0

   # set message limit to unlimited when general DEBUG is requested
   if MessageSvc.OutputLevel<=DEBUG :
      MessageSvc.defaultLimit = 0
      MessageSvc.enableSuppression = False

   # publish message counts during RUNNING in histogram
   MessageSvc.publishStats = True
   MessageSvc.publishLevel = INFO

   # show summary statistics of messages in finalize
   MessageSvc.showStats = True
   MessageSvc.statLevel = WARNING
   MessageSvc.statLevelRun = VERBOSE

   # publish message counts during RUNNING in histogram
   MessageSvc.publishStats = True
   MessageSvc.publishLevel = INFO
