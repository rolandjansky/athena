#!/usr/bin/env python
#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file DQTestingDriver.py
@author C. D. Burton
@author P. Onyisi
@date 2019-06-20
@brief Driver script to run DQ with new-style configuration on an ESD/AOD
'''

if __name__=='__main__':
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--preExec', help='Code to execute before locking configs')
    parser.add_argument('--postExec', help='Code to execute after setup')
    parser.add_argument('--dqOffByDefault', action='store_true',
                        help='Set all DQ steering flags to False, user must then switch them on again explicitly')
    parser.add_argument('--maxEvents', type=int,
                        help='Maximum number of events to process')
    parser.add_argument('--loglevel', type=int, default=3,
                        help='Verbosity level')
    parser.add_argument('flags', nargs='*', help='Config flag overrides')
    args = parser.parse_args()

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import *
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q431/21.0/myESD.pool.root']
    ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput.root'
    if args.dqOffByDefault:
        from AthenaMonitoring.DQConfigFlags import allSteeringFlagsOff
        allSteeringFlagsOff()
    ConfigFlags.fillFromArgs(args.flags)

    if args.preExec:
        # bring things into scope
        from AthenaMonitoring.DQConfigFlags import allSteeringFlagsOff
        log.info('Executing preExec: %s', args.preExec)
        exec(args.preExec)

    log.info('FINAL CONFIG FLAGS SETTINGS FOLLOW')
    ConfigFlags.dump()
        
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg, MainServicesThreadedCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    if ConfigFlags.Concurrency.NumThreads == 0:
        cfg = MainServicesSerialCfg()
    else:
        cfg = MainServicesThreadedCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    if ConfigFlags.Concurrency.NumThreads > 0:
        from AthenaMonitoring.AthenaMonitoringConf import ForceIDConditionsAlg
        beginseq = cfg.getSequence("AthBeginSeq")
        beginseq += ForceIDConditionsAlg("ForceIDConditionsAlg")
    
    # load DQ
    from AthenaMonitoring.AthenaMonitoringCfg import AthenaMonitoringCfg
    dq = AthenaMonitoringCfg(ConfigFlags)
    cfg.merge(dq)

    # any last things to do?
    if args.postExec:
        log.info('Executing postExec: %s', args.postExec)
        exec(args.postExec)

    # If you want to turn on more detailed messages ...
    # exampleMonitorAcc.getEventAlgo('ExampleMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=False) # set True for exhaustive info

    cfg.run(args.maxEvents, args.loglevel)
