#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

if __name__=='__main__':
  import sys
  from argparse import ArgumentParser
  parser = ArgumentParser()
  parser.add_argument('--chain', default='HLT_mu50_L1MU20', type=str, help='Chain Group for which to plot the navigation graphs')
  parser.add_argument('--collection', type=str, help='Optional, restrict to nodes in a given collection, e.g. if the file contains multiple slim levels ')
  parser.add_argument('--doTDTDump', action='store_true', help='Optional extra dump of TrigDecisionTool information for supplied chain')
  parser.add_argument('--doNavDump', action='store_true', help='Optional extra dump of trigger navigation data to terminal (caution: large)')
  parser.add_argument('--maxEvents', type=int, default=20, help='Maximum number of events to process')
  parser.add_argument('--skipEvents', type=int, help='Number of events to skip')
  parser.add_argument('--loglevel', type=int, default=3, help='Verbosity level')
  parser.add_argument('flags', nargs='*', help='Config flag overrides')
  args = parser.parse_args()

  from AthenaCommon.Logging import logging
  log = logging.getLogger("generateTrigNavGraph.py")
  log.info("Generating NavGraphs for :" + args.chain)

  # Setup the Run III behaviour
  from AthenaCommon.Configurable import Configurable
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  Configurable.configurableRun3Behavior = 1

  # Set the Athena configuration flags
  ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art//TrigAnalysisTest/AthenaTrigAOD_TrigEDMandTDTCheck_LS2_v1_chain/AOD.pool.root"]

  ConfigFlags.Exec.MaxEvents = args.maxEvents
  if args.skipEvents is not None:
    ConfigFlags.Exec.SkipEvents = args.skipEvents

  if args.flags:
    ConfigFlags.fillFromArgs(args.flags)

  ConfigFlags.lock()

  if ConfigFlags.Trigger.EDMVersion != 3:
    log.error("Can only run over a Run 3 AOD or ESD file")
    sys.exit(1)

  # Initialize configuration object, add accumulator, merge, and run.
  from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
  from AthenaConfiguration.ComponentFactory import CompFactory

  from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
  cfg = MainServicesCfg(ConfigFlags)
  cfg.merge(PoolReadCfg(ConfigFlags))

  from TrigDecisionTool.TrigDecisionToolConfig import getTrigDecisionTool
  tdt_ca = getTrigDecisionTool(ConfigFlags)
  tdt = tdt_ca.getPrimary()
  cfg.merge(tdt_ca)

  checker = CompFactory.TrigEDMChecker()
  checker.doDumpAll                       = False
  checker.TriggerDecisionTool             = tdt
  checker.DumpNavigationForChain          = args.chain
  checker.dumpTrigCompositeContainers     = [args.collection] if args.collection is not None else []
  checker.doDumpTrigCompsiteNavigation    = True
  # Other potentially useful commands
  checker.doDumpAllTrigComposite          = args.doNavDump
  if args.doNavDump:
    from Gaudi.Configuration import DEBUG
    checker.OutputLevel = DEBUG
  checker.doTDTCheck                      = args.doTDTDump
  cfg.addEventAlgo(checker)

  sc = cfg.run()

  log.info("Converting .dot files to .dot.pdf")
  import os
  from subprocess import check_call
  for f in os.listdir('.'):
    if "NavGraph_" in f and not ".dot.pdf" in f:
      check_call(['dot','-Tpdf',f,'-o',f+'.pdf'])

  sys.exit(0 if sc.isSuccess() else 1)