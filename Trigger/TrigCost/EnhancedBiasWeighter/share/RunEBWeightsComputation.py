#!/usr/bin/env python
#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AutoConfigFlags import GetFileMD

from AthenaCommon.Logging import logging
log = logging.getLogger('RunEBWeightsComputation.py')


def ebComputingAlg(flags, itemsMap = {}):
    acc = ComponentAccumulator()

    ebAlg = CompFactory.EnhancedBiasWeightCompAlg()
    ebAlg.ChainToItemMap = itemsMap
    # To run with AOD
    #ebAlg.TrigDecisionTool = "Trig::TrigDecisionTool/TrigDecisionTool"
    # To run with Run3 EB data
    from TrigDecisionTool.TrigDecisionToolConfig import getRun3NavigationContainerFromInput
    ebAlg.FinalDecisionKey = getRun3NavigationContainerFromInput(ConfigFlags) #"HLTNav_Summary"
    ebAlg.OutputLevel = ConfigFlags.Exec.OutputLevel
    acc.addEventAlgo(ebAlg)

    return acc


def hltConfigSvcCfg(smk = -1, db = "", menuFile = ""):
    acc = ComponentAccumulator()

    hltConfigSvc = CompFactory.getComp("TrigConf::HLTConfigSvc")("HLTConfigSvc")
    if menuFile:
        hltConfigSvc.InputType = "FILE"
        hltConfigSvc.HLTJsonFileName = menuFile
    elif smk > 0 and db:
        hltConfigSvc.InputType = "DB"
        hltConfigSvc.TriggerDB = db
        hltConfigSvc.SMK = smk

    acc.addService(hltConfigSvc, False, True)
    return acc


def l1PrescalesCfg(psk = -1, db = "", pskFile = ""):
    acc = ComponentAccumulator()

    l1PrescaleCondAlg = CompFactory.getComp("TrigConf::L1PrescaleCondAlg")("L1PrescaleCondAlg")
    if pskFile:
        l1PrescaleCondAlg.Source = "FILE"
        l1PrescaleCondAlg.Filename = pskFile
    elif psk > 0 and db:
        l1PrescaleCondAlg.Source = "DB"
        l1PrescaleCondAlg.TriggerDB = db
        l1PrescaleCondAlg.L1Psk = psk

    acc.addCondAlgo(l1PrescaleCondAlg)
    return acc


def hltPrescalesCfg(psk = -1, db = "", pskFile = ""):
    acc = ComponentAccumulator()

    hltPrescaleCondAlg = CompFactory.getComp("TrigConf::HLTPrescaleCondAlg")("HLTPrescaleCondAlg")
    if pskFile:
        hltPrescaleCondAlg.Source = "FILE"
        hltPrescaleCondAlg.Filename = pskFile
    elif psk > 0 and db:
        hltPrescaleCondAlg.Source = "DB"
        hltPrescaleCondAlg.TriggerDB = db
        hltPrescaleCondAlg.HLTPsk = psk

    acc.addCondAlgo(hltPrescaleCondAlg)
    return acc


# Look for ljson file in current dir
def getJsonFile(name):
    # Try to find local menu file
    fileName = name + '.*json'
    log.debug("Looking for local file  %s", fileName)

    import re, os
    r = re.compile(fileName)
    foundFiles = list(filter(r.match, os.listdir('.')))

    if len(foundFiles) > 1:
        log.info("Found more than one file! Saving first match %s", foundFiles[0])

    return foundFiles[0] if len(foundFiles) > 0 else ""


# Read the keys from the COOL database
# TODO - what if the prescales change?
def getConfigKeys(inputFile):
    run = GetFileMD(inputFile)['runNumbers'][0]
    lb = GetFileMD(inputFile)['lumiBlockNumbers'][0]

    from TrigConfigSvc.TrigConfigSvcCfg import getTrigConfFromCool
    return getTrigConfFromCool(run, lb)


# Read the seeds of low and medium chain, not available in the menu (they are hlt seeded)
def readHLTSeeds(smk=-1, db=""):
    from TrigConfIO.HLTTriggerConfigAccess import HLTJobOptionsAccess
    joData = HLTJobOptionsAccess(dbalias = db, smkey = smk)

    import ast

    chainToItem = {}
    chainToItem["HLT_eb_low_L1RD2_FILLED"] = ast.literal_eval(joData.properties("EnhancedBiasHypo.HLT_eb_low_L1RD2_FILLED")["L1ItemNames"])
    chainToItem["HLT_eb_medium_L1RD2_FILLED"] = ast.literal_eval(joData.properties("EnhancedBiasHypo.HLT_eb_medium_L1RD2_FILLED")["L1ItemNames"])

    return chainToItem
    

if __name__=='__main__':
    import sys
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--maxEvents', type=int, help='Maximum number of events to process')
    parser.add_argument('--skipEvents',type=int, help='Number of events to skip')
    parser.add_argument('--loglevel', type=int, default=3, help='Verbosity level: 1 - VERBOSE, 2 - DEBUG, 3 - INFO')
    parser.add_argument('flags', nargs='*', help='Config flag overrides')  
    args = parser.parse_args()

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.fillFromArgs(args.flags)
    ConfigFlags.Trigger.triggerConfig = 'DB'
    ConfigFlags.Exec.OutputLevel = args.loglevel
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    cfg = MainServicesCfg(ConfigFlags)

    # TODO use main stream deserialisation when running from RAW and TDT when running from AOD
    from TriggerJobOpts.TriggerRecoConfig import TriggerRecoCfg
    cfg.merge( TriggerRecoCfg(ConfigFlags) )

    # TDT will be used until we get Run3 RAW EB data
    # from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    # from AthenaMonitoring.TriggerInterface import TrigDecisionToolCfg
    # cfg.merge(PoolReadCfg(ConfigFlags))
    # cfg.merge(TrigDecisionToolCfg(ConfigFlags))

    hltMenuFile, hltPskFile, l1PskFile = (getJsonFile("HLTMenu"), getJsonFile("HLTPrescalesSet"), getJsonFile("L1PrescalesSet"))
    if not hltMenuFile or not hltPskFile or not l1PskFile:
        log.info("Local files not found - retrieving configuration from db")
        configKeys = getConfigKeys(ConfigFlags.Input.Files)
        cfg.merge(hltConfigSvcCfg(smk = configKeys["SMK"], db = configKeys["DB"]))
        cfg.merge(l1PrescalesCfg(psk = configKeys["L1PSK"], db = configKeys["DB"]))
        cfg.merge(hltPrescalesCfg(psk = configKeys["HLTPSK"], db = configKeys["DB"]))
    else:
        cfg.merge(hltConfigSvcCfg(menuFile = hltMenuFile))
        cfg.merge(l1PrescalesCfg(pskFile = l1PskFile))
        cfg.merge(hltPrescalesCfg(pskFile = hltPskFile))

    itemsMap = readHLTSeeds(smk = configKeys["SMK"], db = configKeys["DB"])

    cfg.merge(ebComputingAlg(ConfigFlags, itemsMap))

    # If you want to turn on more detailed messages ...
    # exampleMonitorAcc.getEventAlgo('ExampleMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=False) # set True for exhaustive info

    sc = cfg.run(args.maxEvents, args.loglevel)
    sys.exit(0 if sc.isSuccess() else 1)
