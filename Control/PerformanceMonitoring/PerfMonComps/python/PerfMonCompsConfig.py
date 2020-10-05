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

    # Get CA and add the service 
    acc = ComponentAccumulator()
    acc.addService(PerfMonMTSvc(**kwargs), create=True)

    # Enable the auditors that are necessarry for the service
    acc.setAppProperty("AuditAlgorithms", True)
    acc.setAppProperty("AuditTools", True)
    acc.setAppProperty("AuditServices", True)

    # Add the algorithm that is necessary for the service
    PerfMonMTAlg = CompFactory.PerfMonMTAlg
    acc.addEventAlgo(PerfMonMTAlg(), sequenceName='AthAlgSeq')
    
    # Return the CA
    return acc
