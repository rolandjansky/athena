# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# "Standalone" jobOptions for running
# AthCUDAExamples::LinearTransformTaskExampleAlg.
#

# The number of CUDA kernels to execute in parallel in the job.
CUDAKernelsInFlight = 5

# Set up the service(s) needed by the job.
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr += CfgMgr.AthCUDA__StreamPoolSvc( "CUDAStreamPoolSvc",
                                             NStreams = CUDAKernelsInFlight )
ServiceMgr += CfgMgr.AthCUDA__KernelRunnerSvc( "CUDAKernelRunnerSvc",
                 StreamPoolSvc = ServiceMgr.CUDAStreamPoolSvc,
                 NParallelKernels = CUDAKernelsInFlight )

# Set up / access the algorithm sequence.
from AthenaCommon.AlgSequence import AlgSequence
algSequence = AlgSequence()

# Add the algorithm to the sequence.
from AthExCUDA.AthExCUDAConf import \
   AthCUDAExamples__LinearTransformTaskExampleAlg
algSequence += AthCUDAExamples__LinearTransformTaskExampleAlg(
   KernelRunnerSvc = ServiceMgr.CUDAKernelRunnerSvc,
   Blocking = True )

# Run for some larger number of events.
theApp.EvtMax = 10000

# Set up the avalanche scheduler correctly, in case we are running with multiple
# threads.
from AthenaCommon.ConcurrencyFlags import jobproperties
if jobproperties.ConcurrencyFlags.NumThreads() != 0:
   from AthenaCommon.AlgScheduler import AlgScheduler
   AlgScheduler.SchedulerSvc.PreemptiveBlockingTasks = True
   AlgScheduler.SchedulerSvc.MaxBlockingAlgosInFlight = CUDAKernelsInFlight
   pass

# Set the event printout interval.
if not hasattr( ServiceMgr, theApp.EventLoop ):
   ServiceMgr += getattr( CfgMgr, theApp.EventLoop )()
   pass
evtLoop = getattr( ServiceMgr, theApp.EventLoop )
evtLoop.EventPrintoutInterval = 1000
