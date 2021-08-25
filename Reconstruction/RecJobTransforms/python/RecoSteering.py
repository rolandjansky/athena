# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Logging import logging
log = logging.getLogger("RecoSteering.py")

tryConfiguringAll=False
def RecoSteering(flags):
    """
    Generates configuration of the reconstructions

    This driver configures all reconstruction steps unconditionally.
    The selftest available below can be used for simple jobs, yet full functionality is achieved with tansforms that set many flags.
    """
    acc = ComponentAccumulator()

    # setup input    
    if flags.Input.Format == 'BS':
        from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
        acc.merge(ByteStreamReadCfg(flags))
        log.info("---------- Configured BS reading")
    else:
        from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
        acc.merge(PoolReadCfg(flags))
        log.info("---------- Configured POOL reading")

    # calorimeter    
    from CaloRec.CaloRecoConfig import CaloRecoCfg
    acc.merge(CaloRecoCfg(flags,doLCCalib=True))
    log.info("---------- Configured calorimeter reconstruction")

    # ID    
    from InDetConfig.TrackRecoConfig import TrackRecoCfg
    acc.merge(TrackRecoCfg(flags))
    log.info("---------- Configured tracking")

    # muons
    from MuonConfig.MuonReconstructionConfig import MuonReconstructionCfg
    acc.merge(MuonReconstructionCfg(flags))
    log.info("---------- Configured muon tracking")

    if tryConfiguringAll:
        from MuonCombinedConfig.MuonCombinedReconstructionConfig import MuonCombinedReconstructionCfg
        acc.merge(MuonCombinedReconstructionCfg(flags))
        log.info("---------- Configured combined muon reconstruction")

    #Caching of CaloExtension for downstream Combined Performance algorithms.
    #The algorithms that use these cached CaloExtension only run in the reco step that produces ESD.
    if flags.Output.doESD:
      from TrackToCalo.CaloExtensionBuilderAlgCfg import CaloExtensionBuilderAlgCfg
      acc.merge(CaloExtensionBuilderAlgCfg(flags))
      log.info("---------- Configured track calorimeter extension builder")

    if tryConfiguringAll:
        from eflowRec.PFRun3Config import PFCfg
        acc.merge(PFCfg(flags))

    # physics objects
    # egamma
    # jets
    # btagging
    if tryConfiguringAll:
        from AthenaCommon.ConcurrencyFlags import jobproperties # hack to prevent btagging fragments to rename top sequence
        jobproperties.ConcurrencyFlags.NumThreads=flags.Concurrency.NumThreads
        from BTagging.BTagRun3Config import BTagRecoSplitCfg
        acc.merge(BTagRecoSplitCfg(flags))
        log.info("---------- Configured btagging")

    #setup output
    if any((flags.Output.doWriteESD, flags.Output.doWriteAOD, flags.Output.doWriteRDO)):
        from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
        acc.merge(PoolWriteCfg(flags))
        log.info("setup POOL format writing")
        
    if flags.Output.doWriteESD:
        log.info("ESD ItemList: %s", acc.getEventAlgo("OutputStreamESD").ItemList)
        log.info("---------- Configured ESD writing")

    if flags.Output.doWriteAOD:
        log.info("ESD ItemList: %s", acc.getEventAlgo("OutputStreamAOD").ItemList)
        log.info("---------- Configured AOD writing")

    return acc

import sys
def _run(input):
    import datetime
    startStamp = datetime.datetime.now()
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    flags.Exec.MaxEvents = 5
    flags.Concurrency.NumThreads = 1
    flags.Input.Files = []
    #TODO these flags should be defaulted in the divier function above, 
    #TODO    but then we ought to have option to set them from command line should the parser be passed there too?

    flags.Detector.GeometryBCM=True
    flags.Detector.GeometryDBM=True
    flags.Detector.GeometryPixel=True
    flags.Detector.GeometrySCT=True
    flags.Detector.GeometryTRT=True

    flags.Detector.GeometryTile=True
    flags.Detector.GeometryLAr=True

    flags.Calo.TopoCluster.doTopoClusterLocalCalib=False
    flags.Output.ESDFileName="myESD.pool.root"
    flags.Output.AODFileName="myAOD.pool.root"
    parser = flags.getArgumentParser()
    parser.add_argument('--tryConfiguringAll', action="store_true")
    args = flags.fillFromArgs(parser=parser)

    # test inputs
    if flags.Input.Files == []:
        from AthenaConfiguration.TestDefaults import defaultTestFiles
        if input == "RAW":
            flags.Input.Files = defaultTestFiles.RAW
        if input == "ESD":
            flags.Input.Files = defaultTestFiles.ESD
        if input == "RDO":
            flags.Input.Files = defaultTestFiles.RDO


    flags.lock()
    log.info("Configuring according to flag values listed below")
    flags.dump()

    acc = MainServicesCfg(flags)
    if args.tryConfiguringAll: # this option (and related functionality) should be removed once all major fragments can actually be configured
        global tryConfiguringAll
        tryConfiguringAll=True
    acc.merge(RecoSteering(flags), sequenceName="AthAlgSeq")
    confStamp = datetime.datetime.now()
    log.info("configured in %d seconds", (confStamp-startStamp).seconds )
    acc.printConfig(withDetails=True)

    if args.configOnly:
        with open(args.configOnly, "wb") as confFile:
            acc.store(confFile)
            log.info("configOnly option specified. Saved in: %s ... exiting now.", args.configOnly )
            sys.exit(0)
    # running        
    statusCode = acc.run()
    endStamp = datetime.datetime.now()
    log.info("total time spent %d seconds (running %s seconds) ", (endStamp-startStamp).seconds, (endStamp-confStamp).seconds )
    return statusCode


if __name__ == "__main__":
    statusCode = None
    for test in ["RAW", "ESD", "RDO"]:
        if f"--{test}" in sys.argv:
            del sys.argv[sys.argv.index(f"--{test}")]
            statusCode = _run(input=test)
    assert statusCode is not None, "No test was run"
    sys.exit(not statusCode.isSuccess())
