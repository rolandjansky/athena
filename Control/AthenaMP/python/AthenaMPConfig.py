# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AllConfigFlags import ConfigFlags, GetFileMD
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format, ProductionStep
from AthenaConfiguration.MainServicesConfig import MainServicesCfg

from AthenaCommon.Logging import log as msg

import os, shutil


def athenaMPRunArgsToFlags(runArgs, flags):
    """Fill MP configuration flags from run arguments."""
    if hasattr(runArgs, "athenaMPWorkerTopDir"):
        flags.MP.WorkerTopDir = runArgs.athenaMPWorkerTopDir

    if hasattr(runArgs, "athenaMPOutputReportFile"):
        flags.MP.OutputReportFile = runArgs.athenaMPOutputReportFile

    if hasattr(runArgs, "athenaMPCollectSubprocessLogs"):
        flags.MP.CollectSubprocessLogs = runArgs.athenaMPCollectSubprocessLogs

    if hasattr(runArgs, "athenaMPStrategy"):
        flags.MP.Strategy = runArgs.athenaMPStrategy

    if hasattr(runArgs, "athenaMPReadEventOrders"):
        flags.MP.ReadEventOrders = runArgs.athenaMPReadEventOrders

    if hasattr(runArgs, "athenaMPEventOrdersFile"):
        flags.MP.EventOrdersFile = runArgs.athenaMPEventOrdersFile

    if hasattr(runArgs, "athenaMPEventsBeforeFork"):
        flags.MP.EventsBeforeFork = runArgs.athenaMPEventsBeforeFork

    if hasattr(runArgs, "sharedWriter"):
        flags.MP.UseSharedWriter = runArgs.sharedWriter

    if hasattr(runArgs, "parallelCompression"):
        flags.MP.UseParallelCompression = runArgs.parallelCompression


def AthenaMPCfg(configFlags):

    os.putenv('XRD_ENABLEFORKHANDLERS','1')
    os.putenv('XRD_RUNFORKHANDLER','1')

    result=ComponentAccumulator()

    # Configure MP Event Loop Manager
    AthMpEvtLoopMgr=CompFactory.AthMpEvtLoopMgr
    mpevtloop = AthMpEvtLoopMgr()

    mpevtloop.NWorkers=configFlags.Concurrency.NumProcs
    mpevtloop.Strategy=configFlags.MP.Strategy
    mpevtloop.WorkerTopDir = configFlags.MP.WorkerTopDir
    mpevtloop.OutputReportFile = configFlags.MP.OutputReportFile
    mpevtloop.CollectSubprocessLogs = configFlags.MP.CollectSubprocessLogs
    mpevtloop.PollingInterval = configFlags.MP.PollingInterval
    mpevtloop.MemSamplingInterval = configFlags.MP.MemSamplingInterval
    mpevtloop.IsPileup = True if configFlags.Common.ProductionStep in [ProductionStep.PileUpPresampling, ProductionStep.Overlay] else False
    mpevtloop.EventsBeforeFork = 0 if configFlags.MP.Strategy == 'EventService' else configFlags.MP.EventsBeforeFork

    # Configure Gaudi File Manager
    filemgr = CompFactory.FileMgr(LogFile="FileManagerLog")
    result.addService(filemgr)

    # Save PoolFileCatalog.xml if exists in the run directory
    # The saved file will be copied over to workers' run directories just after forking
    if os.path.isfile('PoolFileCatalog.xml'):
        shutil.copyfile('PoolFileCatalog.xml','PoolFileCatalog.xml.AthenaMP-saved')

    # Compute event chunk size
    chunk_size = getChunkSize(configFlags)

    # Configure Strategy
    debug_worker = configFlags.Concurrency.DebugWorkers
    event_range_channel = configFlags.MP.EventRangeChannel
    use_shared_reader = configFlags.MP.UseSharedReader
    use_shared_writer = configFlags.MP.UseSharedWriter
    use_parallel_compression = configFlags.MP.UseParallelCompression

    if configFlags.MP.Strategy=='SharedQueue' or configFlags.MP.Strategy=='RoundRobin':
        if use_shared_reader:
            AthenaSharedMemoryTool = CompFactory.AthenaSharedMemoryTool

            if configFlags.Input.Format is Format.BS:
                evSel=CompFactory.EventSelectorByteStream("EventSelector")

                from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
                bscfg = ByteStreamReadCfg(configFlags)
                result.merge(bscfg)
            else:
                evSel=CompFactory.EventSelectorAthenaPool("EventSelector")

                # AthenaPoolCnvSvc
                apcs=CompFactory.AthenaPoolCnvSvc()
                apcs.InputStreamingTool = AthenaSharedMemoryTool("InputStreamingTool",
                                                                 SharedMemoryName="InputStream"+str(os.getpid()),
                                                                 UseMultipleSegments=True)
                result.addService(apcs)

                from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
                poolcfg = PoolReadCfg(configFlags)

                result.merge(poolcfg)

            evSel.SharedMemoryTool = AthenaSharedMemoryTool("EventStreamingTool",
                                                            SharedMemoryName="EventStream"+str(os.getpid()))
            result.addService(evSel)

        if use_shared_writer:
            if any((configFlags.Output.doWriteESD,
                    configFlags.Output.doWriteAOD,
                    configFlags.Output.doWriteRDO)) or configFlags.Output.HITSFileName!='':
                AthenaSharedMemoryTool = CompFactory.AthenaSharedMemoryTool

                apcs=CompFactory.AthenaPoolCnvSvc()
                apcs.OutputStreamingTool += [ AthenaSharedMemoryTool("OutputStreamingTool_0",
                                                                     SharedMemoryName="OutputStream"+str(os.getpid())) ]
                apcs.ParallelCompression = use_parallel_compression
                result.addService(apcs)

                from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
                poolcfg = PoolWriteCfg(configFlags)

                result.merge(poolcfg)

        queue_provider = CompFactory.SharedEvtQueueProvider(UseSharedReader=use_shared_reader,
                                                            IsPileup=mpevtloop.IsPileup,
                                                            EventsBeforeFork=mpevtloop.EventsBeforeFork,
                                                            ChunkSize=chunk_size)
        if configFlags.Concurrency.NumThreads > 0:
            if mpevtloop.IsPileup:
                raise Exception('Running pileup digitization in mixed MP+MT currently not supported')
            queue_consumer = CompFactory.SharedEvtQueueConsumer(UseSharedWriter=use_shared_writer,
                                                                EventsBeforeFork=mpevtloop.EventsBeforeFork,
                                                                Debug=debug_worker)
        else:
            queue_consumer = CompFactory.SharedEvtQueueConsumer(UseSharedReader=use_shared_reader,
                                                                UseSharedWriter=use_shared_writer,
                                                                IsPileup=mpevtloop.IsPileup,
                                                                IsRoundRobin=(configFlags.MP.Strategy=='RoundRobin'),
                                                                EventsBeforeFork=mpevtloop.EventsBeforeFork,
                                                                ReadEventOrders=configFlags.MP.ReadEventOrders,
                                                                EventOrdersFile=configFlags.MP.EventOrdersFile,
                                                                Debug=debug_worker)
        mpevtloop.Tools += [ queue_provider, queue_consumer ]

        if use_shared_writer:
            shared_writer = CompFactory.SharedWriterTool(MotherProcess=(mpevtloop.EventsBeforeFork>0),
                                                         IsPileup=mpevtloop.IsPileup)
            mpevtloop.Tools += [ shared_writer ]

    elif configFlags.MP.Strategy=='FileScheduling':
        mpevtloop.Tools += [ CompFactory.FileSchedulingTool(IsPileup=mpevtloop.IsPileup,
                                                            Debug=debug_worker) ]

    elif configFlags.MP.Strategy=='EventService':
        channelScatterer2Processor = "AthenaMP_Scatterer2Processor"
        channelProcessor2EvtSel = "AthenaMP_Processor2EvtSel"

        mpevtloop.Tools += [ CompFactory.EvtRangeScatterer(ProcessorChannel = channelScatterer2Processor,
                                                           EventRangeChannel = event_range_channel,
                                                           DoCaching=configFlags.MP.EvtRangeScattererCaching) ]
        mpevtloop.Tools += [ CompFactory.vtRangeProcessor(IsPileup=mpevtloop.IsPileup,
                                                          Channel2Scatterer = channelScatterer2Processor,
                                                          Channel2EvtSel = channelProcessor2EvtSel,
                                                          Debug=debug_worker) ]

    else:
        msg.warning("Unknown strategy %s. No MP tools will be configured", configFlags.MP.Strategy)

    result.addService(mpevtloop, primary=True)

    return result

def getChunkSize(configFlags) -> int:
    chunk_size = 1
    if configFlags.MP.ChunkSize > 0:
        chunk_size = configFlags.MP.ChunkSize
        msg.info('Chunk size set to %i', chunk_size)
    else:
        md = GetFileMD(configFlags.Input.Files)
        #Don't use auto flush for shared reader
        if configFlags.MP.UseSharedReader:
            msg.info('Shared Reader in use, chunk_size set to default (%i)', chunk_size)
        #Use auto flush only if file is compressed with LZMA, else use default chunk_size
        elif configFlags.MP.ChunkSize == -1:
            if md.get('file_comp_alg',-1) == 2:
                chunk_size = md.get('auto_flush',-1)
                msg.info('Chunk size set to auto flush (%i)', chunk_size)
            else:
                msg.info('LZMA algorithm not in use, chunk_size set to default (%i)', chunk_size)
        #Use auto flush only if file is compressed with LZMA or ZLIB, else use default chunk_size
        elif configFlags.MP.ChunkSize == -2:
            if md.get('file_comp_alg',-1) in [1,2]:
                chunk_size = md.get('auto_flush',-1)
                msg.info('Chunk size set to auto flush (%i)', chunk_size)
            else:
                msg.info('LZMA nor ZLIB in use, chunk_size set to default (%i)', chunk_size)
                #Use auto flush only if file is compressed with LZMA, ZLIB or LZ4, else use default chunk_size
        elif configFlags.MP.ChunkSize == -3:
            if md.get('file_comp_alg',-1) in [1,2,4]:
                chunk_size = md.get('auto_flush',-1)
                msg.info('Chunk size set to auto flush (%i)', chunk_size)
            else:
                msg.info('LZMA, ZLIB nor LZ4 in use, chunk_size set to (%i)', chunk_size)
        #Use auto flush value for chunk_size, regarldess of compression algorithm
        elif configFlags.MPChunkSize <= -4:
            chunk_size = md.get('auto_flush',-1)
            msg.info('Chunk size set to auto flush (%i)', chunk_size)
        else:
            msg.warning('Invalid ChunkSize, Chunk Size set to default (%i)', chunk_size)

    return chunk_size


if __name__=="__main__":

    # -----------------  Hello World Example ------------------
    # ConfigFlags.Exec.MaxEvents=10
    # ConfigFlags.Concurrency.NumProcs=2

    # cfg=MainServicesCfg(ConfigFlags)

    # from AthExHelloWorld.HelloWorldConfig import HelloWorldCfg
    # cfg.merge(HelloWorldCfg())

    # cfg.run()
    # -----------------  Hello World Example ------------------

    # -----------------  Example with input file --------------
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.ESD
    ConfigFlags.Exec.MaxEvents=10
    ConfigFlags.Concurrency.NumProcs=2

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    cfg=MainServicesCfg(ConfigFlags)
    from AthenaPoolCnvSvc.PoolReadConfig import EventSelectorAthenaPoolCfg
    cfg.merge(EventSelectorAthenaPoolCfg(ConfigFlags))
    cfg.run()
    # -----------------  Example with input file --------------

    msg.info('All OK!')
