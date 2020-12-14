#!/usr/bin/env python
#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Logging import logging
log = logging.getLogger('RunTrigCostAnalysis.py')


# Configure Cost Analysis algorithm
def trigCostAnalysisCfg(flags, baseWeight=1.0, useEBWeights=False):
  from TrigCostAnalysis.ROSToROB import ROSToROBMap
  from Gaudi.Configuration import DEBUG

  acc = ComponentAccumulator()

  # Retrieve run number
  from AthenaConfiguration.AutoConfigFlags import GetFileMD
  runNumbers = GetFileMD(ConfigFlags.Input.Files)['runNumbers']

  if len(runNumbers) > 1:
    log.error('Multiple run numbers from metadata! Only one expected per cost processing')
    return acc

  enhancedBiasWeighter = CompFactory.EnhancedBiasWeighter()
  enhancedBiasWeighter.RunNumber = runNumbers[0]
  enhancedBiasWeighter.UseBunchCrossingTool = False

  trigCostAnalysis = CompFactory.TrigCostAnalysis()
  trigCostAnalysis.OutputLevel = DEBUG
  trigCostAnalysis.RootStreamName = "COSTSTREAM"
  trigCostAnalysis.BaseEventWeight = baseWeight
  trigCostAnalysis.EnhancedBiasTool = enhancedBiasWeighter
  trigCostAnalysis.UseEBWeights = useEBWeights
  trigCostAnalysis.MaxFullEventDumps = 100
  trigCostAnalysis.FullEventDumpProbability = 1 # X. Where probability is 1 in X
  trigCostAnalysis.ROSToROBMap = ROSToROBMap().get_mapping()

  acc.addEventAlgo(trigCostAnalysis)

  return acc


# Configure deserialisation
def decodingCfg(flags):
  from AthenaCommon.CFElements import seqAND
  from TrigEDMConfig import DataScoutingInfo

  acc = ComponentAccumulator()

  acc.addSequence(seqAND("Decoding"))
  acc.addEventAlgo(CompFactory.HLTResultMTByteStreamDecoderAlg(), "Decoding")

  costDataDeserialiser = CompFactory.TriggerEDMDeserialiserAlg("CostDataTrigDeserialiser")
  costDataDeserialiser.ModuleID = DataScoutingInfo.DataScoutingIdentifiers["CostMonDS"]
  acc.addEventAlgo(costDataDeserialiser, "Decoding")

  return acc


# Configure HLTConfigSvc with JSON Menu file
def hltConfigSvcCfg(flags):
  acc = ComponentAccumulator()

  hltConfigSvc = CompFactory.getComp("TrigConf::HLTConfigSvc")("HLTConfigSvc")
  hltConfigSvc.ConfigSource = "None"
  hltConfigSvc.XMLMenuFile = "None"
  hltConfigSvc.InputType = "file"
  hltConfigSvc.JsonFileName = getHltMenu()
  acc.addService(hltConfigSvc, False, True)

  return acc


# Get HLT Menu from json file or from DB
def getHltMenu():
  # Try to find local menu file
  menuFileName = 'HLTMenu_.*json'

  import re, os
  r = re.compile(menuFileName)
  menuFiles = list(filter(r.match, os.listdir('.')))

  if len(menuFiles) > 1:
    log.info("Found more than one menu file! Saving first match %s", menuFiles[0])

  # If local file not found - read HLTMenu from database
  if len(menuFiles) == 0:
    log.error("Menu file not found!")
    # TODO retrieve menu from db

  else:
    return menuFiles[0]


if __name__=='__main__':
  import sys
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  parser = ConfigFlags.getArgumentParser()
  parser.add_argument('--outputHist', type=str, default='TrigCostRoot_Results.root', help='Histogram output ROOT file')
  parser.add_argument('--baseWeight', type=float, default=1.0, help='Base events weight')
  parser.add_argument('--useEBWeights', type=bool, default=False, help='Apply Enhanced Bias weights')
  parser.add_argument('flags', nargs='*', help='Config flag overrides')
  args = parser.parse_args()

  # Setup the Run III behavior
  from AthenaCommon.Configurable import Configurable
  Configurable.configurableRun3Behavior = 1

  # Set the Athena configuration flags
  ConfigFlags.fillFromArgs(parser=parser)
  ConfigFlags.lock()

  # Initialize configuration object, add accumulator, merge, and run.
  from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
  cfg = MainServicesCfg(ConfigFlags)

  from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
  cfg.merge(ByteStreamReadCfg(ConfigFlags))

  histSvc = CompFactory.THistSvc()
  histSvc.Output += ["COSTSTREAM DATAFILE='" + args.outputHist + "' OPT='RECREATE'"]
  cfg.addService(histSvc)

  cfg.merge(decodingCfg(ConfigFlags))
  cfg.merge(hltConfigSvcCfg(ConfigFlags))
  cfg.merge(trigCostAnalysisCfg(ConfigFlags, args.baseWeight, args.useEBWeights))

  # If you want to turn on more detailed messages ...
  # exampleMonitorAcc.getEventAlgo('ExampleMonAlg').OutputLevel = 2 # DEBUG
  cfg.printConfig(withDetails=False) # set True for exhaustive info

  sc = cfg.run(args.evtMax, args.loglevel)
  sys.exit(0 if sc.isSuccess() else 1)
