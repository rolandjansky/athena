#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AutoConfigFlags import GetFileMD
from AthenaCommon.Logging import logging
log = logging.getLogger('RunTrigCostAnalysis.py')


# Configure Cost Analysis algorithm
def trigCostAnalysisCfg(flags, args, isMC=False):
  from TrigCostAnalysis.ROSToROB import ROSToROBMap

  acc = ComponentAccumulator()

  # Retrieve run number
  runNumbers = GetFileMD(ConfigFlags.Input.Files)['runNumbers']

  if len(runNumbers) > 1:
    log.error('Multiple run numbers from metadata! Only one expected per cost processing')
    return acc

  enhancedBiasWeighter = CompFactory.EnhancedBiasWeighter()
  enhancedBiasWeighter.RunNumber = runNumbers[0]
  enhancedBiasWeighter.UseBunchCrossingData = False
  enhancedBiasWeighter.IsMC = isMC
  if isMC:
    MCpayload = readMCpayload(args)
    enhancedBiasWeighter.MCCrossSection = MCpayload.get('MCCrossSection')
    enhancedBiasWeighter.MCFilterEfficiency = MCpayload.get('MCFilterEfficiency')
    enhancedBiasWeighter.MCKFactor = MCpayload.get('MCKFactor')
    enhancedBiasWeighter.MCIgnoreGeneratorWeights = MCpayload.get('MCIgnoreGeneratorWeights')

  trigCostAnalysis = CompFactory.TrigCostAnalysis()
  trigCostAnalysis.OutputLevel = args.loglevel
  trigCostAnalysis.BaseEventWeight = args.baseWeight
  trigCostAnalysis.EnhancedBiasTool = enhancedBiasWeighter
  trigCostAnalysis.AlgToChainTool = CompFactory.getComp("TrigCompositeUtils::AlgToChainTool")()
  trigCostAnalysis.UseEBWeights = args.useEBWeights
  trigCostAnalysis.MaxFullEventDumps = 100
  trigCostAnalysis.FullEventDumpProbability = 1 # X. Where probability is 1 in X
  trigCostAnalysis.UseSingleTimeRange = isMC or args.useEBWeights
  trigCostAnalysis.ROSToROBMap = ROSToROBMap().get_mapping()
  trigCostAnalysis.DoMonitorChainAlgorithm = args.monitorChainAlgorithm

  trigCostAnalysis.AdditionalHashList = readHashes(args.joFile, args.smk, args.dbAlias)

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

# Read algorithm and class names from HLTJobOptions file
def readHashes(joFileName="", smk=0, dbAlias=""):
  joData = {}

  try:
    if joFileName:
      log.debug("Reading HLTJobOptions from file {0}".format(joFileName))
      with open(joFileName, "r") as joFile:
        import json
        joData = json.load(joFile)
    elif smk and dbAlias:
      log.debug("Reading HLTJobOptions from database {0} {1}".format(smk, dbAlias))
      from TrigConfIO.HLTTriggerConfigAccess import HLTJobOptionsAccess
      joData = HLTJobOptionsAccess(dbalias = dbAlias, smkey = smk)
    else:
      log.debug("Additional collection's names from HLTJobOptions file are not available")
      return list()
  except Exception as err:
    log.warning("Retrieving additional names fron HLTJO failed: {0}".format(err))
    return list()

  namesList = set()
  log.info("Reading additional names...")

  for entry in joData["properties"]:
    namesList.add(entry.split('.')[0])

    # Read algorithm names with classes
    entryObj = joData["properties"][entry]
    if "Members" in entryObj:
      membersList = entryObj["Members"].strip('][').replace("'", "").split(', ')
      namesList.update(membersList)


  log.info("Retrieved {0} additional names".format(len(namesList)))
  return list(namesList)


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
def hltConfigSvcCfg(flags, smk, dbAlias):
  acc = ComponentAccumulator()

  hltConfigSvc = CompFactory.getComp("TrigConf::HLTConfigSvc")("HLTConfigSvc")

  menuFile = getHltMenu()
  # If local file not found - read HLTMenu from database
  if menuFile:
    log.debug("Reading HLTMenu from file {0}".format(menuFile))

    hltConfigSvc.InputType = "FILE"
    hltConfigSvc.JsonFileName = menuFile
  elif smk and dbAlias:
    log.debug("Reading HLTMenu from database {0} {1}".format(smk, dbAlias))

    hltConfigSvc.InputType = "DB"
    hltConfigSvc.JsonFileName = ""
    hltConfigSvc.TriggerDB = dbAlias
    hltConfigSvc.SMK = smk
  else:
    log.error("Cannot read the HLTMenu! Provide file or relevant keys.")

  acc.addService(hltConfigSvc, False, True)

  return acc


def readConfigFromCool(smk, dbAlias):
  # Try to read keys from COOL (for P1 data)
  from TrigConfStorage.TriggerCoolUtil import TriggerCoolUtil
  dbconn = TriggerCoolUtil.GetConnection("CONDBR2")
  runNumber = GetFileMD(ConfigFlags.Input.Files)['runNumbers'][0]
  configKeys = TriggerCoolUtil.getHLTConfigKeys(dbconn, [[runNumber, runNumber]])

  log.debug("Getting keys from COOL for run {0}".format(runNumber))
  if configKeys and runNumber in configKeys.keys():
    if not smk:
      smk = configKeys[runNumber]['SMK']

    if not dbAlias:
      # For example TRIGGERDBDEV1;22.0.20;Athena -> TRIGGERDBDEV1
      dbAlias = configKeys[runNumber]['DB'].split(";")[0]

    log.debug("Config keys are SMK: {0} DB alias: {1}".format(smk, dbAlias))

  else:
    log.debug("Configuration keys for run {0} not found!".format(runNumber))
    dbAlias = None if not dbAlias else dbAlias
    smk = None if not smk else smk

  return (smk, dbAlias)


# Get HLT Menu from json file
def getHltMenu():
  # Try to find local menu file
  menuFileName = 'HLTMenu_.*json'

  import re, os
  r = re.compile(menuFileName)
  menuFiles = list(filter(r.match, os.listdir('.')))

  if len(menuFiles) > 1:
    log.info("Found more than one menu file! Saving first match %s", menuFiles[0])

  return menuFiles[0] if len(menuFiles) > 0 else ""


if __name__=='__main__':
  import sys
  from argparse import ArgumentParser
  parser = ArgumentParser()
  parser.add_argument('--outputHist', type=str, default='TrigCostRoot_Results.root', help='Histogram output ROOT file')
  parser.add_argument('--monitorChainAlgorithm', action='store_true', help='Turn on Chain Algorithm monitoring')
  parser.add_argument('--baseWeight', type=float, default=1.0, help='Base events weight')
  parser.add_argument('--useEBWeights', type=bool, default=False, help='Apply Enhanced Bias weights')
  parser.add_argument('--joFile', type=str, help='Optional HLTJobOptions file to add more hashes')

  parser.add_argument('--smk', type=int, help='SuperMasterKey to retrieve menu file')
  parser.add_argument('--dbAlias', type=str, help='Database alias to retrieve menu file')

  parser.add_argument('--MCCrossSection', default=0.0, type=float, help='For MC input: Cross section of process in nb')
  parser.add_argument('--MCFilterEfficiency', default=1.0, type=float, help='For MC input: Filter efficiency of any MC filter (0.0 - 1.0)')
  parser.add_argument('--MCKFactor', default=1.0, type=float, help='For MC input: Additional multiplicitive fudge-factor to the supplied cross section.')
  parser.add_argument('--MCIgnoreGeneratorWeights', action='store_true', help='For MC input: Flag to disregard any generator weights.')

  parser.add_argument('--maxEvents', type=int, help='Maximum number of events to process')
  parser.add_argument('--skipEvents',type=int, help='Number of events to skip')
  parser.add_argument('--loglevel', type=int, default=3, help='Verbosity level: 1 - VERBOSE, 2 - DEBUG, 3 - INFO')
  parser.add_argument('flags', nargs='*', help='Config flag overrides')  
  args = parser.parse_args()

  log.level = args.loglevel

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

  # Retrieve config from cool database
  if not args.smk or not args.dbAlias:
    (args.smk, args.dbAlias) = readConfigFromCool(args.smk, args.dbAlias)

  cfg.merge(hltConfigSvcCfg(ConfigFlags, args.smk, args.dbAlias))
  cfg.merge(trigCostAnalysisCfg(ConfigFlags, args, ConfigFlags.Input.isMC))

  # If you want to turn on more detailed messages ...
  # exampleMonitorAcc.getEventAlgo('ExampleMonAlg').OutputLevel = 2 # DEBUG
  cfg.printConfig(withDetails=False) # set True for exhaustive info
  sc = cfg.run(args.maxEvents, args.loglevel)
  sys.exit(0 if sc.isSuccess() else 1)
