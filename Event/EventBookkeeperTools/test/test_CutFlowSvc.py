#!/usr/bin/env python
"""Test CutFlowSvc

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
import sys

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

from EventBookkeeperTools.EventBookkeeperToolsConfig import CutFlowSvcCfg, CutFlowOutputList

# Setup configuration
Configurable.configurableRun3Behavior = True
ConfigFlags.Input.Files = defaultTestFiles.AOD_MC
ConfigFlags.Output.AODFileName = "testAOD.pool.root"

# Flags relating to multithreaded execution
threads = 2
ConfigFlags.Concurrency.NumThreads = threads
if threads > 0:
  ConfigFlags.Scheduler.ShowDataDeps = True
  ConfigFlags.Scheduler.ShowDataFlow = True
  ConfigFlags.Scheduler.ShowControlFlow = True
  ConfigFlags.Concurrency.NumConcurrentEvents = threads

ConfigFlags.lock()

# Setup service
acc = MainServicesCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(CutFlowSvcCfg(ConfigFlags))
acc.addEventAlgo(CompFactory.TestFilterReentrantAlg("TestReentrant1", FilterKey="TestReentrant1", Modulo=2))
acc.addEventAlgo(CompFactory.TestFilterReentrantAlg("TestReentrant2", FilterKey="TestReentrant2", Modulo=4))

acc.merge(OutputStreamCfg(ConfigFlags, "AOD", MetadataItemList=CutFlowOutputList(ConfigFlags)))

# Debugging
from AthenaCommon.Constants import VERBOSE
acc.getService('CutFlowSvc').OutputLevel = VERBOSE
acc.getPublicTool('CutBookkeepersTool').OutputLevel = VERBOSE
acc.getEventAlgo('AllExecutedEventsCounterAlg').OutputLevel = VERBOSE

# Execute and finish
sc = acc.run(maxEvents=10*threads)

# Success should be 0
sys.exit(not sc.isSuccess())
