#!/usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# A minimal job that demonstrates what PerfMonMTSvc does
if __name__ == '__main__':

    # Import the common flags/services
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from PerfMonComps.PerfMonCompsConfig import PerfMonMTSvcCfg

    # Set the necessary configuration flags
    # Process 100 events in 8 threads/slots and do full monitoring
    ConfigFlags.Input.Files = []
    ConfigFlags.Exec.MaxEvents = 100
    ConfigFlags.Concurrency.NumThreads = 8
    ConfigFlags.PerfMon.doFullMonMT = True
    ConfigFlags.PerfMon.OutputJSON = 'perfmonmt_test.json'
    ConfigFlags.lock()

    # Set up the configuration and add the relevant services
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PerfMonMTSvcCfg(ConfigFlags))

    # Overwrite cardinality settings
    AlgResourcePool = cfg.getService('AlgResourcePool')
    AlgResourcePool.OverrideUnClonable = True

    # Burn 100 +/- 1 ms per event
    CpuCruncherAlg = CompFactory.getComp('PerfMonTest::CpuCruncherAlg')
    cfg.addEventAlgo(CpuCruncherAlg('CpuCruncherAlg', MeanCpu = 100, RmsCpu = 1), sequenceName = 'AthAlgSeq')
    cfg.getEventAlgo('CpuCruncherAlg').Cardinality=8

    # Leak 10k ints per event, i.e. 40 KB
    LeakyAlg = CompFactory.getComp('PerfMonTest::LeakyAlg')
    cfg.addEventAlgo(LeakyAlg("LeakyAlg", LeakSize = 10000), sequenceName = 'AthAlgSeq')
    cfg.getEventAlgo('LeakyAlg').Cardinality=8

    # Print the configuration and dump the flags
    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    # Run the job
    sc = cfg.run()

    # Exit as appropriate
    import sys
    sys.exit(not sc.isSuccess())
