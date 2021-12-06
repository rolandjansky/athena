#!/usr/bin/env python
#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AutoConfigFlags import GetFileMD

from AthenaCommon.Logging import logging
log = logging.getLogger('RunEBWeightsComputation.py')


def ebComputingAlg(flags):
    acc = ComponentAccumulator()

    ebAlg = CompFactory.EnhancedBiasWeightCompAlg()
    # To run with AOD
    ebAlg.TrigDecisionTool = "Trig::TrigDecisionTool/TrigDecisionTool"
    # To run with Run3 EB data
    #ebAlg.FinalDecisionKey = "HLTNav_Summary"
    acc.addEventAlgo(ebAlg)

    return acc


def hltConfigSvcCfg(menuFile):
    acc = ComponentAccumulator()

    hltConfigSvc = CompFactory.getComp("TrigConf::HLTConfigSvc")("HLTConfigSvc")
    hltConfigSvc.InputType = "FILE"
    hltConfigSvc.JsonFileName = menuFile

    acc.addService(hltConfigSvc, False, True)
    return acc


def l1PrescalesCfg(pskFile):
    acc = ComponentAccumulator()

    l1PrescaleCondAlg = CompFactory.getComp("TrigConf::L1PrescaleCondAlg")("L1PrescaleCondAlg")
    l1PrescaleCondAlg.Source = "FILE"
    l1PrescaleCondAlg.Filename = pskFile

    acc.addCondAlgo(l1PrescaleCondAlg)
    return acc


def hltPrescalesCfg(pskFile):
    acc = ComponentAccumulator()

    hltPrescaleCondAlg = CompFactory.getComp("TrigConf::HLTPrescaleCondAlg")("HLTPrescaleCondAlg")
    hltPrescaleCondAlg.Source = "FILE"
    hltPrescaleCondAlg.Filename = pskFile

    acc.addCondAlgo(hltPrescaleCondAlg)
    return acc


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


def getConfigFiles(inputFile):
    hltMenu = getJsonFile("HLTMenu")
    hltPsk = getJsonFile("HLTPrescalesSet")
    l1Psk = getJsonFile("L1PrescalesSet")

    if not hltMenu or not hltPsk or not l1Psk:
        log.info("Files not found - retrieving configuration from db")

        import subprocess
        configKeys = getConfigKeys(inputFile)
        cmd = "TrigConfReadWrite -i {DB} {SMK},{L1PSK},{HLTPSK},{BGSK} -o r3json > Run3ConfigFetchJSONFiles.log".format(**configKeys)
        log.info("Running command '%s'", cmd)
        filesFetchStatus = subprocess.run(cmd, shell=True)
        assert filesFetchStatus.returncode == 0, "TrigConfReadWrite failed to fetch JSON files"

        hltMenu = getJsonFile("HLTMenu")
        hltPsk = getJsonFile("HLTPrescalesSet")
        l1Psk = getJsonFile("L1PrescalesSet")

    log.info("Found files {0}, {1}, {2}".format(hltMenu, hltPsk, l1Psk))
    return (hltMenu, hltPsk, l1Psk)


def getConfigKeys(inputFile):
    run = GetFileMD(inputFile)['runNumbers'][0]
    lb = GetFileMD(inputFile)['lumiBlockNumbers'][0]

    from TrigConfigSvc.TrigConfigSvcCfg import getTrigConfFromCool
    triggerDBKeys = getTrigConfFromCool(run, lb)
    triggerDBKeys['DB'] = 'TRIGGERDB' if run > 230000 else 'TRIGGERDB_RUN1'

    return triggerDBKeys


if __name__=='__main__':
    import sys
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--maxEvents', type=int, help='Maximum number of events to process')
    parser.add_argument('--skipEvents',type=int, help='Number of events to skip')
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

    hltMenuFile, hltPskFile, l1PskFile = getConfigFiles(ConfigFlags.Input.Files)
    cfg.merge(hltConfigSvcCfg(hltMenuFile))
    cfg.merge(l1PrescalesCfg(l1PskFile))
    cfg.merge(hltPrescalesCfg(hltPskFile))

    # TDT will be used until we get Run3 RAW EB data
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from AthenaMonitoring.TriggerInterface import TrigDecisionToolCfg
    cfg.merge(PoolReadCfg(ConfigFlags))
    cfg.merge(TrigDecisionToolCfg(ConfigFlags))

    cfg.merge(ebComputingAlg(ConfigFlags))

    # If you want to turn on more detailed messages ...
    # exampleMonitorAcc.getEventAlgo('ExampleMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=False) # set True for exhaustive info

    sc = cfg.run(args.maxEvents, args.loglevel)
    sys.exit(0 if sc.isSuccess() else 1)
