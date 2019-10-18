# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# @file: TrigServicesConfig.py
# @purpose: customized configurables

from TrigServicesConf import TrigCOOLUpdateHelper as _TrigCOOLUpdateHelper
      
class TrigCOOLUpdateHelper(_TrigCOOLUpdateHelper):
   __slots__ = ()

   def __init__(self, name='TrigCOOLUpdateHelper'):
      super(TrigCOOLUpdateHelper, self).__init__(name)

      from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool
      self.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)
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
   import os
   from AthenaCommon.AppMgr import theApp
   from AthenaCommon.AppMgr import ServiceMgr as svcMgr
   from AthenaCommon.Constants import DEBUG, WARNING

   svcMgr.MessageSvc = theApp.service( "MessageSvc" )     # already instantiated
   MessageSvc = svcMgr.MessageSvc
   MessageSvc.OutputLevel = theApp.OutputLevel

   MessageSvc.Format       = "% F%40W%S%4W%e%s%7W%R%T %0W%M"
   # Add timestamp when running in partition
   if os.environ.get('TDAQ_PARTITION','') != 'athenaHLT':
      MessageSvc.Format = "%t  " + MessageSvc.Format

   MessageSvc.ErsFormat    = "%S: %M"
   MessageSvc.printEventIDLevel = WARNING

   # Message suppression
   MessageSvc.enableSuppression    = False
   MessageSvc.suppressRunningOnly  = True

   # 0 = no suppression, negative = log-suppression, positive = normal suppression
   # Do not rely on the defaultLimit property, always set each limit separately
   MessageSvc.defaultLimit = -100
   MessageSvc.verboseLimit = MessageSvc.defaultLimit
   MessageSvc.debugLimit   = MessageSvc.defaultLimit
   MessageSvc.infoLimit    = MessageSvc.defaultLimit
   MessageSvc.warningLimit = MessageSvc.defaultLimit
   MessageSvc.errorLimit   = 0
   MessageSvc.fatalLimit   = 0

   # Message forwarding to ERS
   MessageSvc.useErsError = ['*']
   MessageSvc.useErsFatal = ['*']

   # set message limit to unlimited when general DEBUG is requested
   if MessageSvc.OutputLevel<=DEBUG :
      MessageSvc.defaultLimit = 0
      MessageSvc.enableSuppression = False

   # show summary statistics of messages in finalize
   MessageSvc.showStats = True
   MessageSvc.statLevel = WARNING

# online ROB data provider service
from TrigServicesConf import HltROBDataProviderSvc as _HltROBDataProviderSvc
class HltROBDataProviderSvc(_HltROBDataProviderSvc):
   __slots__ = ()

   def __init__(self, name='ROBDataProviderSvc'):
      super(HltROBDataProviderSvc, self).__init__(name)
      from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool,defineHistogram
      self.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)
      self.MonTool.Histograms = [ 
         defineHistogram('TIME_ROBReserveData', path='EXPERT', type='TH1F',
                         title='Time to reserve ROBs for later retrieval;time [mu s]',
                         xbins=100, xmin=0, xmax=1000),
         defineHistogram('NUMBER_ROBReserveData', path='EXPERT', type='TH1F',
                         title='Number of reserved ROBs for later retrieval;number',
                         xbins=100, xmin=0, xmax=500),
         defineHistogram('TIME_ROBRequest', path='EXPERT', type='TH1F',
                         title='Time for ROB retrievals;time [mu s]',
                         xbins=100, xmin=0, xmax=10000),
         defineHistogram('NUMBER_ROBRequest', path='EXPERT', type='TH1F',
                         title='Number of retrieved ROBs;number',
                         xbins=100, xmin=0, xmax=1000),
         defineHistogram('TIME_CollectAllROBs', path='EXPERT', type='TH1F',
                         title='Time for retrieving complete event data;time [mu s]',
                         xbins=100, xmin=0, xmax=10000),
         defineHistogram('NUMBER_CollectAllROBs', path='EXPERT', type='TH1F',
                         title='Number of received ROBs for collect call;number',
                         xbins=100, xmin=0, xmax=2500)
         ]
      return
