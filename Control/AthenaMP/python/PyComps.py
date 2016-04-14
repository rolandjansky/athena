# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#-----Python imports---#
import os, sys, time, shutil

#-----Athena imports---#
import AthenaCommon.SystemOfUnits as Units
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode
from AthenaCommon.Logging import log as msg

from AthenaMP.AthenaMPConf import AthMpEvtLoopMgr
class MpEvtLoopMgr(AthMpEvtLoopMgr):
    def __init__(self, name='AthMpEvtLoopMgr', **kw):

        from AthenaCommon.AppMgr import theApp
        self.nThreads = theApp._opts.threads

        ## init base class
        kw['name'] = name
        super(MpEvtLoopMgr, self).__init__(**kw)

        os.putenv('XRD_ENABLEFORKHANDLERS','1')
        os.putenv('XRD_RUNFORKHANDLER','1')

        from AthenaMPFlags import jobproperties as jp
        self.WorkerTopDir = jp.AthenaMPFlags.WorkerTopDir()
        self.OutputReportFile = jp.AthenaMPFlags.OutputReportFile()
        self.CollectSubprocessLogs = jp.AthenaMPFlags.CollectSubprocessLogs()
        self.Strategy = jp.AthenaMPFlags.Strategy()
        self.PollingInterval = jp.AthenaMPFlags.PollingInterval()
        self.MemSamplingInterval = jp.AthenaMPFlags.MemSamplingInterval()

        from AthenaCommon.DetFlags import DetFlags
        if DetFlags.pileup.any_on():
            self.IsPileup = True
        else:
            self.IsPileup = False
        self.EventsBeforeFork = jp.AthenaMPFlags.EventsBeforeFork()

        if self.Strategy=='EventService':
            self.EventsBeforeFork = 0

        from AthenaCommon.AppMgr import theApp as app
        app.EventLoop = self.getFullJobOptName()

        # Enable FileMgr logging
        from GaudiSvc.GaudiSvcConf import FileMgr
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        svcMgr+=FileMgr(LogFile="FileManagerLog")

        # Save PoolFileCatalog.xml if exists in the run directory
        if os.path.isfile('PoolFileCatalog.xml'):
            shutil.copyfile('PoolFileCatalog.xml','PoolFileCatalog.xml.AthenaMP-saved')

        self.configureStrategy(self.Strategy,self.IsPileup,self.EventsBeforeFork)
        
    def configureStrategy(self,strategy,pileup,events_before_fork):
        from AthenaMPFlags import jobproperties as jp
        from AthenaCommon.ConcurrencyFlags import jobproperties as jp
        event_range_channel = jp.AthenaMPFlags.EventRangeChannel()
        chunk_size = jp.AthenaMPFlags.ChunkSize()
        debug_worker = jp.ConcurrencyFlags.DebugWorkers()

        if strategy=='SharedQueue' or strategy=='RoundRobin':
            from AthenaMPTools.AthenaMPToolsConf import SharedEvtQueueProvider
            self.Tools += [ SharedEvtQueueProvider(IsPileup=pileup,
                                                   EventsBeforeFork=events_before_fork,
                                                   ChunkSize=chunk_size) ]

            if (self.nThreads >= 1):
                from AthenaMPTools.AthenaMPToolsConf import SharedHiveEvtQueueConsumer
                self.Tools += [ SharedHiveEvtQueueConsumer(UseSharedReader=False,
                                                           IsPileup=pileup,
                                                           IsRoundRobin=(strategy=='RoundRobin'),
                                                           EventsBeforeFork=events_before_fork,
                                                           Debug=debug_worker)   ]
            else:
                from AthenaMPTools.AthenaMPToolsConf import SharedEvtQueueConsumer
                self.Tools += [ SharedEvtQueueConsumer(UseSharedReader=False,
                                                       IsPileup=pileup,
                                                       IsRoundRobin=(strategy=='RoundRobin'),
                                                       EventsBeforeFork=events_before_fork,
                                                       ReadEventOrders=jp.AthenaMPFlags.ReadEventOrders(),
                                                       EventOrdersFile=jp.AthenaMPFlags.EventOrdersFile(),
                                                       Debug=debug_worker)   ]

            # Enable seeking
            setupEvtSelForSeekOps()

        elif strategy=='FileScheduling':
            from AthenaMPTools.AthenaMPToolsConf import FileSchedulingTool
            self.Tools += [ FileSchedulingTool(IsPileup=pileup,
                                               Debug=debug_worker) ]

        elif strategy=='SharedReader':
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr

            from AthenaServices.AthenaServicesConf import AthenaSharedMemoryTool
            shm_name = "EvtSelSM_" + str(os.getpid())
            svcMgr.EventSelector.SharedMemoryTool = AthenaSharedMemoryTool("EventStreamingTool")
            if sys.modules.has_key('AthenaPoolCnvSvc.ReadAthenaPool'):
                svcMgr.AthenaPoolCnvSvc.InputStreamingTool = AthenaSharedMemoryTool("InputStreamingTool")
                #svcMgr.AthenaPoolCnvSvc.OutputStreamingTool = AthenaSharedMemoryTool("OutputStreamingTool")

            from AthenaMPTools.AthenaMPToolsConf import SharedReaderTool
            self.Tools += [ SharedReaderTool() ]

            from AthenaMPTools.AthenaMPToolsConf import SharedEvtQueueProvider
            self.Tools += [ SharedEvtQueueProvider(IsPileup=pileup,
                                                   EventsBeforeFork=0) ]

            from AthenaMPTools.AthenaMPToolsConf import SharedEvtQueueConsumer
            self.Tools += [ SharedEvtQueueConsumer(UseSharedReader=True,
                                                   IsPileup=pileup,
                                                   EventsBeforeFork=0) ]

        elif strategy=='EventService':
            channelScatterer2Processor = "AthenaMP_Scatterer2Processor"
            channelProcessor2EvtSel = "AthenaMP_Processor2EvtSel"
            use_token_extractor = jp.AthenaMPFlags.UseTokenExtractor()

            if use_token_extractor:
                from AthenaCommon.AppMgr import ServiceMgr as svcMgr
                from AthenaServices.AthenaServicesConf import AthenaYamplTool
                svcMgr.EventSelector.SharedMemoryTool = AthenaYamplTool("AthenaYamplTool",
                                                                        ChannelName = channelProcessor2EvtSel,
                                                                        Many2One=False)

            from AthenaMPTools.AthenaMPToolsConf import EvtRangeScatterer
            self.Tools += [ EvtRangeScatterer(ProcessorChannel = channelScatterer2Processor,
                                              EventRangeChannel = event_range_channel,
                                              UseTokenExtractor = use_token_extractor,
                                              TokenExtractorChannel = jp.AthenaMPFlags.TokenExtractorChannel(),
                                              DoCaching=jp.AthenaMPFlags.EvtRangeScattererCaching()) ]

            from AthenaMPTools.AthenaMPToolsConf import EvtRangeProcessor
            self.Tools += [ EvtRangeProcessor(IsPileup=pileup,
                                              Channel2Scatterer = channelScatterer2Processor,
                                              Channel2EvtSel = channelProcessor2EvtSel,
                                              UseTokenExtractor = use_token_extractor,
                                              Debug=debug_worker) ]
            # Enable seeking
            setupEvtSelForSeekOps()

        else:
            msg.warning("Unknown strategy. No MP tools will be configured")

def setupEvtSelForSeekOps():
   """ try to install seek-stuff on the EventSelector side """
   #import sys
   #from AthenaCommon.Logging import log as msg
   msg.debug("setupEvtSelForSeekOps:")
   if sys.modules.has_key('AthenaRootComps.ReadAthenaRoot'):
       # athenarootcomps has seeking enabled by default
       msg.info('=> Seeking enabled.')
       return
   
   if not sys.modules.has_key('AthenaPoolCnvSvc.ReadAthenaPool'):
      ## user did not import that module so we give up
      msg.info( "Cannot enable 'seeking' b/c module " + \
                 "[AthenaPoolCnvSvc.ReadAthenaPool] hasn't been imported..." )
      msg.info( "Modify your jobOptions to import that module "+ \
                 "(or just ignore this message)" )
      return

   from AthenaCommon.AppMgr import theApp, AthAppMgr
   if theApp.state() != AthAppMgr.State.OFFLINE:
      msg.info( "C++ ApplicationMgr already instantiated, probably seeking "+\
                "will be ill-configured..." )
      msg.info( "EventSelector writers should implement updateHandlers" )
   
   from AthenaCommon.AppMgr import ServiceMgr as svcMgr
   from AthenaCommon.Configurable import Configurable
   collectionType = svcMgr.EventSelector.properties()["CollectionType"]

   if collectionType in ( "ImplicitROOT", Configurable.propertyNoValue, ):
      svcMgr.EventSelector.CollectionType = "SeekableROOT"
      msg.info   ( "=> Seeking enabled." )

   elif collectionType in ( "SeekableROOT", ):
      msg.verbose( "=> Seeking already enabled." )

   else:
      msg.warning( "Input seeking is not compatible with collection type of %s",
                   svcMgr.EventSelector.properties()["CollectionType"] )
      msg.warning( "=> Seeking disabled." )
   return

