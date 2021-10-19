#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging

## A minimal new-style configuration for PerfMonMTSvc
def PerfMonMTSvcCfg(flags, **kwargs):
    """ Configuring PerfMonMTSvc """
   
    # Get the logger
    log = logging.getLogger("PerfMonMTSvcCfg")
    log.info("Configuring PerfMonMTSvc with flags:")
    log.info("  >> doFastMonMT {}".format(flags.PerfMon.doFastMonMT))
    log.info("  >> doFullMonMT {}".format(flags.PerfMon.doFullMonMT))

    # Check if basic monitoring is asked for
    if not flags.PerfMon.doFastMonMT and not flags.PerfMon.doFullMonMT:
        log.info("Nothing to be done...")
        return ComponentAccumulator()

    # Hook to PerfMonMTSvc
    PerfMonMTSvc = CompFactory.PerfMonMTSvc

    # Set the main properties for the service
    import os,psutil
    kwargs.setdefault("wallTimeOffset",
                      psutil.Process(os.getpid()).create_time() * 1000)
    kwargs.setdefault("numberOfThreads",
                      max(1,flags.Concurrency.NumThreads))
    kwargs.setdefault("numberOfSlots",
                      max(1,flags.Concurrency.NumConcurrentEvents))
    kwargs.setdefault("doComponentLevelMonitoring",
                      flags.PerfMon.doFullMonMT)
    kwargs.setdefault("jsonFileName", flags.PerfMon.OutputJSON)

    # Get CA and add the service 
    acc = ComponentAccumulator(sequence="AthBeginSeq")
    beginSeq = acc.getSequence("AthBeginSeq")
    beginSeq.IgnoreFilterPassed = False
    beginSeq.StopOverride = False
    beginSeq.Sequential = True
    acc.addService(PerfMonMTSvc(**kwargs), create=True)

    # Enable the auditors that are necessarry for the service
    acc.setAppProperty("AuditAlgorithms", True)
    acc.setAppProperty("AuditTools", True)
    acc.setAppProperty("AuditServices", True)

    # Return the CA
    return acc

# A minimal job that demonstrates what PerfMonMTSvc does
if __name__ == '__main__':

    # Import the common flags/services
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg

    # Set the necessary configuration flags
    # Process 100 events in 1 thread/slot and do fast monitoring
    ConfigFlags.Exec.MaxEvents = 100
    ConfigFlags.Concurrency.NumProcs = 1
    ConfigFlags.Concurrency.NumThreads = 1
    ConfigFlags.PerfMon.doFastMonMT = True
    ConfigFlags.PerfMon.OutputJSON = 'perfmonmt_test.json'
    ConfigFlags.lock()

    # Set up the configuration and add the relevant services
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PerfMonMTSvcCfg(ConfigFlags))

    # Burn 100 +/- 1 ms per event
    CpuCruncherAlg = CompFactory.getComp('PerfMonTest::CpuCruncherAlg')
    cfg.addEventAlgo(CpuCruncherAlg('CpuCruncherAlg', MeanCpu = 100, RmsCpu = 1), sequenceName = 'AthAlgSeq')

    # Leak 10k ints per event, i.e. 40 KB
    LeakyAlg = CompFactory.getComp('PerfMonTest::LeakyAlg')
    cfg.addEventAlgo(LeakyAlg("LeakyAlg", LeakSize = 10000), sequenceName = 'AthAlgSeq')

    # Print the configuration and dump the flags
    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    # Run the job
    sc = cfg.run()

    # Exit as appropriate
    import sys
    sys.exit(not sc.isSuccess())
