# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file: TrigServicesConfig.py
# @purpose: customized configurables

from TrigServices.TrigServicesConf import TrigCOOLUpdateHelper as _TrigCOOLUpdateHelper
from AthenaCommon.Logging import logging
log = logging.getLogger('TrigCOOLUpdateHelper')
 
class TrigCOOLUpdateHelper(_TrigCOOLUpdateHelper):
   __slots__ = ()

   def __init__(self, name='TrigCOOLUpdateHelper'):
      super(TrigCOOLUpdateHelper, self).__init__(name)

      from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
      self.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)
      self.MonTool.defineHistogram('TIME_CoolFolderUpdate', path='EXPERT', type='TH1F',
                                   title='Time for conditions update;time [ms]',
                                   xbins=100, xmin=0, xmax=200)
      return

   def enable(self, folders = ['/Indet/Onl/Beampos',
                               '/TRIGGER/LUMI/HLTPrefLumi',
                               '/TRIGGER/HLT/PrescaleKey']):
      """Enable COOL folder updates for given folders (only use this for data)"""
      
      from AthenaCommon.AppMgr import ServiceMgr as svcMgr
      if not hasattr(svcMgr,'IOVDbSvc'): return
      
      self.CoolFolderMap = '/TRIGGER/HLT/COOLUPDATE'
      self.Folders = folders

      from IOVDbSvc.CondDB import conddb
      conddb.addFolder('TRIGGER', self.CoolFolderMap)

      # Make sure relevant folders are marked as 'extensible'
      for i, dbf in enumerate(svcMgr.IOVDbSvc.Folders):
         for f in self.Folders:
            if f in dbf and '<extensible/>' not in f:
               svcMgr.IOVDbSvc.Folders[i] += ' <extensible/>'
               log.info('IOVDbSvc folder %s not marked as extensible. Fixing this...', f)


def setupMessageSvc():
   import os
   from AthenaCommon.AppMgr import theApp
   from AthenaCommon.AppMgr import ServiceMgr as svcMgr
   from AthenaCommon.Constants import DEBUG, WARNING

   svcMgr.MessageSvc = theApp.service( "MessageSvc" )     # already instantiated
   MessageSvc = svcMgr.MessageSvc
   MessageSvc.OutputLevel = theApp.OutputLevel

   MessageSvc.Format       = "% F%40W%S%4W%R%e%s%8W%R%T %0W%M"
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
from TrigServices.TrigServicesConf import HltROBDataProviderSvc as _HltROBDataProviderSvc
class HltROBDataProviderSvc(_HltROBDataProviderSvc):
   __slots__ = ()

   def __init__(self, name='ROBDataProviderSvc'):
      super(HltROBDataProviderSvc, self).__init__(name)
      from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool,defineHistogram
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
                         xbins=400, xmin=0, xmax=200000),
         defineHistogram('NUMBER_ROBRequest', path='EXPERT', type='TH1F',
                         title='Number of retrieved ROBs;number',
                         xbins=100, xmin=0, xmax=1000),
         defineHistogram('TIME_CollectAllROBs', path='EXPERT', type='TH1F',
                         title='Time for retrieving complete event data;time [mu s]',
                         xbins=400, xmin=0, xmax=200000),
         defineHistogram('NUMBER_CollectAllROBs', path='EXPERT', type='TH1F',
                         title='Number of received ROBs for collect call;number',
                         xbins=100, xmin=0, xmax=2500)
         ]
      return

# online event loop manager
from TrigServices.TrigServicesConf import HltEventLoopMgr as _HltEventLoopMgr
class HltEventLoopMgr(_HltEventLoopMgr):
   __slots__ = ()

   def __init__(self, name='HltEventLoopMgr'):
      super(HltEventLoopMgr, self).__init__(name)
      from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
      self.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)
      self.MonTool.defineHistogram('TotalTime', path='EXPERT', type='TH1F',
                                   title='Total event processing time (all events);Time [ms];Events',
                                   xbins=200, xmin=0, xmax=10000)
      self.MonTool.defineHistogram('TotalTimeAccepted', path='EXPERT', type='TH1F',
                                   title='Total event processing time (accepted events);Time [ms];Events',
                                   xbins=200, xmin=0, xmax=10000)
      self.MonTool.defineHistogram('TotalTimeRejected', path='EXPERT', type='TH1F',
                                   title='Total event processing time (rejected events);Time [ms];Events',
                                   xbins=200, xmin=0, xmax=10000)
      self.MonTool.defineHistogram('SlotIdleTime', path='EXPERT', type='TH1F',
                                   title='Time between freeing and assigning a scheduler slot;Time [ms];Events',
                                   xbins=400, xmin=0, xmax=400)

      from TrigSteerMonitor.TrigSteerMonitorConfig import getTrigErrorMonTool
      self.TrigErrorMonTool = getTrigErrorMonTool()

      return
