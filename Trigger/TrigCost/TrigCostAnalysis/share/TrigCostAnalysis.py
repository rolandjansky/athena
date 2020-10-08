#!/usr/bin/env python
#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

if __name__=='__main__':
  import sys
  from argparse import ArgumentParser
  parser = ArgumentParser()
  #
  parser.add_argument('--outputHist', default='TrigCostRoot_Results.root', type=str, help='Histogram output ROOT file')
  #
  parser.add_argument('--maxEvents', type=int, default=-1, help='Maximum number of events to process')
  parser.add_argument('--loglevel', type=int, default=3, help='Verbosity level')
  parser.add_argument('flags', nargs='*', help='Config flag overrides')
  args = parser.parse_args()

  # Setup the Run III behavior
  from AthenaCommon.Configurable import Configurable
  Configurable.configurableRun3Behavior = 1

  # Set the Athena configuration flags
  from AthenaConfiguration.AllConfigFlags import ConfigFlags

  # Set the Athena configuration flags
  ConfigFlags.Input.Files = ["root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/data16_13TeV/8d/de/AOD.10654269._000566.pool.root.1"]

  ConfigFlags.lock()

  # Initialize configuration object, add accumulator, merge, and run.
  from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
  from AthenaConfiguration.ComponentFactory import CompFactory
  from AthenaCommon import CfgMgr
  from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
  from TrigEDMConfig import DataScoutingInfo
  from AthenaCommon.CFElements import seqAND
  from Gaudi.Configuration import DEBUG

  cfg = MainServicesCfg(ConfigFlags)
  cfg.merge(ByteStreamReadCfg(ConfigFlags))

  histSvc = CompFactory.THistSvc()
  histSvc.Output += ["COSTSTREAM DATAFILE='" + args.outputHist + "' OPT='RECREATE'"]
  cfg.addService(histSvc)

  cfg.addSequence(seqAND("Decoding"))
  cfg.addEventAlgo(CfgMgr.HLTResultMTByteStreamDecoderAlg(), "Decoding")
  costDataDeserialiser = CfgMgr.TriggerEDMDeserialiserAlg("CostDataTrigDeserialiser")
  costDataDeserialiser.ModuleID = DataScoutingInfo.DataScoutingIdentifiers["CostMonDS"]
  cfg.addEventAlgo(costDataDeserialiser, "Decoding")

  hltConfigSvc = CfgMgr.TrigConf__HLTConfigSvc( "HLTConfigSvc" )
  hltConfigSvc.ConfigSource = "None"
  hltConfigSvc.XMLMenuFile = "None"
  hltConfigSvc.InputType = "file"
  hltConfigSvc.JsonFileName = "HLTMenu_9.json"
  cfg.addService(hltConfigSvc)

  enhancedBiasWeighter = CfgMgr.EnhancedBiasWeighter()
  enhancedBiasWeighter.RunNumber = 379158
  enhancedBiasWeighter.UseBunchCrossingTool = False

  trigCostAnalysis = CfgMgr.TrigCostAnalysis()
  trigCostAnalysis.OutputLevel = DEBUG
  trigCostAnalysis.RootStreamName = "COSTSTREAM"
  trigCostAnalysis.BaseEventWeight = 10.0
  trigCostAnalysis.EnhancedBiasTool = enhancedBiasWeighter
  trigCostAnalysis.UseEBWeights = False
  trigCostAnalysis.MaxFullEventDumps = 100
  trigCostAnalysis.FullEventDumpProbability = 1 # X. Where probability is 1 in X
  cfg.addEventAlgo(trigCostAnalysis)

  # If you want to turn on more detailed messages ...
  # exampleMonitorAcc.getEventAlgo('ExampleMonAlg').OutputLevel = 2 # DEBUG
  cfg.printConfig(withDetails=False) # set True for exhaustive info

  sc = cfg.run(args.maxEvents, args.loglevel)
  sys.exit(0 if sc.isSuccess() else 1)
