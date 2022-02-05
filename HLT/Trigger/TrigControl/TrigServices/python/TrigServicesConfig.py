# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool


# old-JO style function
def setupMessageSvc():
   from AthenaCommon.AppMgr import theApp
   from AthenaCommon.AppMgr import ServiceMgr as svcMgr
   from AthenaCommon.Constants import DEBUG, WARNING

   svcMgr.MessageSvc = theApp.service( "MessageSvc" )     # already instantiated
   MessageSvc = svcMgr.MessageSvc
   MessageSvc.OutputLevel = theApp.OutputLevel

   MessageSvc.Format       = "%t  % F%40W%C%4W%R%e%s%8W%R%T %0W%M"
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


# Finalize COOL update configuration (called from TriggerUnixStandardSetup.setupCommonServicesEnd)
def enableCOOLFolderUpdates():
   '''Enable COOL folder updates'''

   from AthenaCommon.Logging import logging
   log = logging.getLogger('TrigServicesConfig')

   from AthenaCommon.AppMgr import ServiceMgr as svcMgr
   if not hasattr(svcMgr,'IOVDbSvc'):
      return

   cool_helper = svcMgr.HltEventLoopMgr.CoolUpdateTool

   # Add the COOL folder map to IOVDbSvc. This is done 'manually' so it
   # works both in new and old-style configurations:
   svcMgr.IOVDbSvc.Folders.append('<db>COOLONL_TRIGGER/CONDBR2</db> %s' % cool_helper.CoolFolderMap)

   # Make sure relevant folders are marked as 'extensible':
   for i, dbf in enumerate(svcMgr.IOVDbSvc.Folders):
      for f in cool_helper.Folders:
         if f in dbf and '<extensible/>' not in f:
            svcMgr.IOVDbSvc.Folders[i] += ' <extensible/>'
            log.info('IOVDbSvc folder %s not marked as extensible. Fixing this...', f)


def getTrigCOOLUpdateHelper(name='TrigCOOLUpdateHelper'):
   cool_helper = CompFactory.TrigCOOLUpdateHelper(name)
   cool_helper.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)
   cool_helper.MonTool.defineHistogram('TIME_CoolFolderUpdate', path='EXPERT', type='TH1F',
                                       title='Time for conditions update;time [ms]',
                                       xbins=100, xmin=0, xmax=200)

   # Name of COOL folder map:
   cool_helper.CoolFolderMap = '/TRIGGER/HLT/COOLUPDATE'

   # List of folders that can be updated during the run:
   cool_helper.Folders = ['/Indet/Onl/Beampos',
                          '/TRIGGER/LUMI/HLTPrefLumi',
                          '/TRIGGER/HLT/PrescaleKey']

   return cool_helper


def getHltROBDataProviderSvc(name='ROBDataProviderSvc'):
   '''online ROB data provider service'''
   svc = CompFactory.HltROBDataProviderSvc(name)
   svc.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)
   svc.MonTool.defineHistogram('TIME_ROBReserveData', path='EXPERT', type='TH1F',
                               title='Time to reserve ROBs for later retrieval;time [mu s]',
                               xbins=100, xmin=0, xmax=1000),
   svc.MonTool.defineHistogram('NUMBER_ROBReserveData', path='EXPERT', type='TH1F',
                               title='Number of reserved ROBs for later retrieval;number',
                               xbins=100, xmin=0, xmax=500),
   svc.MonTool.defineHistogram('TIME_ROBRequest', path='EXPERT', type='TH1F',
                               title='Time for ROB retrievals;time [mu s]',
                               xbins=400, xmin=0, xmax=200000),
   svc.MonTool.defineHistogram('NUMBER_ROBRequest', path='EXPERT', type='TH1F',
                               title='Number of retrieved ROBs;number',
                               xbins=100, xmin=0, xmax=1000),
   svc.MonTool.defineHistogram('TIME_CollectAllROBs', path='EXPERT', type='TH1F',
                               title='Time for retrieving complete event data;time [mu s]',
                               xbins=400, xmin=0, xmax=200000),
   svc.MonTool.defineHistogram('NUMBER_CollectAllROBs', path='EXPERT', type='TH1F',
                               title='Number of received ROBs for collect call;number',
                               xbins=100, xmin=0, xmax=2500)
   return svc


def getHltEventLoopMgr(name='HltEventLoopMgr'):
   '''online event loop manager'''
   svc = CompFactory.HltEventLoopMgr(name)
   svc.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)
   svc.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)

   svc.MonTool.defineHistogram('TotalTime', path='EXPERT', type='TH1F',
                              title='Total event processing time (all events);Time [ms];Events',
                              xbins=200, xmin=0, xmax=10000)
   svc.MonTool.defineHistogram('TotalTime;TotalTime_extRange', path='EXPERT', type='TH1F',
                              title='Total event processing time (all events);Time [ms];Events',
                              xbins=200, xmin=0, xmax=20000, opt='kCanRebin')

   svc.MonTool.defineHistogram('TotalTimeAccepted', path='EXPERT', type='TH1F',
                              title='Total event processing time (accepted events);Time [ms];Events',
                              xbins=200, xmin=0, xmax=10000)
   svc.MonTool.defineHistogram('TotalTimeAccepted;TotalTimeAccepted_extRange', path='EXPERT', type='TH1F',
                              title='Total event processing time (accepted events);Time [ms];Events',
                              xbins=200, xmin=0, xmax=20000, opt='kCanRebin')

   svc.MonTool.defineHistogram('TotalTimeRejected', path='EXPERT', type='TH1F',
                              title='Total event processing time (rejected events);Time [ms];Events',
                              xbins=200, xmin=0, xmax=10000)
   svc.MonTool.defineHistogram('TotalTimeRejected;TotalTimeRejected_extRange', path='EXPERT', type='TH1F',
                              title='Total event processing time (rejected events);Time [ms];Events',
                              xbins=200, xmin=0, xmax=20000, opt='kCanRebin')

   svc.MonTool.defineHistogram('SlotIdleTime', path='EXPERT', type='TH1F',
                              title='Time between freeing and assigning a scheduler slot;Time [ms];Events',
                              xbins=400, xmin=0, xmax=400)
   svc.MonTool.defineHistogram('SlotIdleTime;SlotIdleTime_extRange', path='EXPERT', type='TH1F',
                              title='Time between freeing and assigning a scheduler slot;Time [ms];Events',
                              xbins=400, xmin=0, xmax=800, opt='kCanRebin')

   svc.MonTool.defineHistogram('TIME_clearStore', path='EXPERT', type='TH1F',
                              title='Time of clearStore() calls;Time [ms];Calls',
                              xbins=200, xmin=0, xmax=50)
   svc.MonTool.defineHistogram('TIME_clearStore;TIME_clearStore_extRange', path='EXPERT', type='TH1F',
                              title='Time of clearStore() calls;Time [ms];Calls',
                              xbins=200, xmin=0, xmax=200, opt='kCanRebin')

   svc.MonTool.defineHistogram('PopSchedulerTime', path='EXPERT', type='TH1F',
                              title='Time spent waiting for a finished event from the Scheduler;Time [ms];drainScheduler() calls',
                              xbins=250, xmin=0, xmax=250)
   svc.MonTool.defineHistogram('PopSchedulerNumEvt', path='EXPERT', type='TH1F',
                              title='Number of events popped out of scheduler at the same time;Time [ms];drainScheduler() calls',
                              xbins=50, xmin=0, xmax=50)

   from TrigSteerMonitor.TrigSteerMonitorConfig import getTrigErrorMonTool
   svc.TrigErrorMonTool = getTrigErrorMonTool()

   return svc


def TrigServicesCfg(flags):
   acc = ComponentAccumulator()

   rob_data_provider = getHltROBDataProviderSvc()
   acc.addService(rob_data_provider)

   loop_mgr = getHltEventLoopMgr()
   loop_mgr.CoolUpdateTool = getTrigCOOLUpdateHelper()

   from TrigOutputHandling.TrigOutputHandlingConfig import HLTResultMTMakerCfg
   loop_mgr.ResultMaker = HLTResultMTMakerCfg()

   from TriggerJobOpts.TriggerByteStreamConfig import ByteStreamReadCfg
   acc.merge(ByteStreamReadCfg(flags))
   loop_mgr.EvtSel = acc.getService('EventSelectorByteStream')
   loop_mgr.OutputCnvSvc = acc.getService('ByteStreamCnvSvc')

   acc.addService(loop_mgr, primary=True)
   acc.setAppProperty("EventLoop", loop_mgr.name)

   return acc
