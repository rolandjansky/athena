#!/usr/bin/env python
"""Test CutFlowSvc

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
import sys
from argparse import ArgumentParser

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

from EventBookkeeperTools.EventBookkeeperToolsConfig import CutFlowSvcCfg, CutFlowOutputList

# Argument parsing
parser = ArgumentParser(prog='test_CutFlowSvc')
parser.add_argument('input', type=str, nargs='?',
                    help='Specify the input file')
parser.add_argument('-t', '--threads', default=2, type=int,
                    help='The number of concurrent threads to run. 0 uses serial Athena.')
args = parser.parse_args()

# Setup configuration
Configurable.configurableRun3Behavior = True
if args.input:
    ConfigFlags.Input.Files = [args.input]
else:
    ConfigFlags.Input.Files = defaultTestFiles.AOD_MC
ConfigFlags.Output.AODFileName = "testAOD.pool.root"

# Flags relating to multithreaded execution
threads = args.threads
maxEvents = 10
ConfigFlags.Concurrency.NumThreads = threads
if threads > 0:
  maxEvents = 10 * threads
  ConfigFlags.Scheduler.ShowDataDeps = True
  ConfigFlags.Scheduler.ShowDataFlow = True
  ConfigFlags.Scheduler.ShowControlFlow = True
  ConfigFlags.Concurrency.NumConcurrentEvents = threads

ConfigFlags.lock()

# Setup service
acc = MainServicesCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))

if 'EventInfo' not in ConfigFlags.Input.Collections:
  from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg
  acc.merge(EventInfoCnvAlgCfg(ConfigFlags, disableBeamSpot=True))

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
sc = acc.run(maxEvents=maxEvents)

# Success should be 0
sys.exit(not sc.isSuccess())
