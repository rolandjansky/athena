#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Logging import logging
log = logging.getLogger('RunTrigCostAnalysis.py')


# Configure Cost Analysis algorithm
def trigCostAnalysisCfg(flags, baseWeight=1.0, useEBWeights=False, isMC=False, MCpayload={}):
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
  enhancedBiasWeighter.IsMC = isMC
  if isMC:
    enhancedBiasWeighter.MCCrossSection = MCpayload.get('MCCrossSection')
    enhancedBiasWeighter.MCFilterEfficiency = MCpayload.get('MCFilterEfficiency')
    enhancedBiasWeighter.MCKFactor = MCpayload.get('MCKFactor')
    enhancedBiasWeighter.MCIgnoreGeneratorWeights = MCpayload.get('MCIgnoreGeneratorWeights')

  trigCostAnalysis = CompFactory.TrigCostAnalysis()
  trigCostAnalysis.OutputLevel = DEBUG
  trigCostAnalysis.RootStreamName = "COSTSTREAM"
  trigCostAnalysis.BaseEventWeight = baseWeight
  trigCostAnalysis.EnhancedBiasTool = enhancedBiasWeighter
  trigCostAnalysis.AlgToChainTool = CompFactory.getComp("TrigCompositeUtils::AlgToChainTool")()
  trigCostAnalysis.UseEBWeights = useEBWeights
  trigCostAnalysis.MaxFullEventDumps = 100
  trigCostAnalysis.FullEventDumpProbability = 1 # X. Where probability is 1 in X
  trigCostAnalysis.UseSingleTimeRange = isMC
  trigCostAnalysis.ROSToROBMap = ROSToROBMap().get_mapping()

  acc.addEventAlgo(trigCostAnalysis)

  return acc

# Prepare dictionary with MC parameters read from arguments
def readMCpayload(args):
  payload = {}

  payload['MCCrossSection'] = args.MCCrossSection
  payload['MCFilterEfficiency'] = args.MCFilterEfficiency
  payload['MCKFactor'] = args.MCKFactor
  payload['MCIgnoreGeneratorWeights'] = args.MCIgnoreGeneratorWeights

  return payload


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
  from argparse import ArgumentParser
  parser = ArgumentParser()
  parser.add_argument('--outputHist', type=str, default='TrigCostRoot_Results.root', help='Histogram output ROOT file')
  parser.add_argument('--baseWeight', type=float, default=1.0, help='Base events weight')
  parser.add_argument('--useEBWeights', type=bool, default=False, help='Apply Enhanced Bias weights')

  parser.add_argument('--MCCrossSection', default=0.0, type=float, help='For MC input: Cross section of process in nb')
  parser.add_argument('--MCFilterEfficiency', default=1.0, type=float, help='For MC input: Filter efficiency of any MC filter (0.0 - 1.0)')
  parser.add_argument('--MCKFactor', default=1.0, type=float, help='For MC input: Additional multiplicitive fudge-factor to the supplied cross section.')
  parser.add_argument('--MCIgnoreGeneratorWeights', action='store_true', help='For MC input: Flag to disregard any generator weights.')

  parser.add_argument('--maxEvents', type=int, help='Maximum number of events to process')
  parser.add_argument('--skipEvents",type=int, help="Number of events to skip')
  parser.add_argument('--loglevel', type=int, default=3, help='Verbosity level')
  parser.add_argument('flags', nargs='*', help='Config flag overrides')  
  args = parser.parse_args()

  # Setup the Run III behavior
  from AthenaCommon.Configurable import Configurable
  Configurable.configurableRun3Behavior = 1

  # Set the Athena configuration flags
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  ConfigFlags.fillFromArgs(args.flags)
  ConfigFlags.lock()

  # Initialize configuration object, add accumulator, merge, and run.
  from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
  cfg = MainServicesCfg(ConfigFlags)

  if ConfigFlags.Input.isMC:
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))
  else:
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    cfg.merge(ByteStreamReadCfg(ConfigFlags))
    cfg.merge(decodingCfg(ConfigFlags))

  histSvc = CompFactory.THistSvc()
  histSvc.Output += ["COSTSTREAM DATAFILE='" + args.outputHist + "' OPT='RECREATE'"]
  cfg.addService(histSvc)

  cfg.merge(hltConfigSvcCfg(ConfigFlags))
  cfg.merge(trigCostAnalysisCfg(ConfigFlags, args.baseWeight, args.useEBWeights, ConfigFlags.Input.isMC, readMCpayload(args)))

  # If you want to turn on more detailed messages ...
  # exampleMonitorAcc.getEventAlgo('ExampleMonAlg').OutputLevel = 2 # DEBUG
  cfg.printConfig(withDetails=False) # set True for exhaustive info

  sc = cfg.run(args.maxEvents, args.loglevel)
  sys.exit(0 if sc.isSuccess() else 1)
