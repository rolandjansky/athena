#!/usr/bin/env python
"""Run tests on ISF_MainConfigNew

This test inherits from Simulation/G4Atlas/G4AtlasAlg/test/G4AtlasAlgConfigNew_Test.py

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
if __name__ == '__main__':

    import time
    a = time.time()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg

    # Set up logging and config behaviour
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    log.setLevel(DEBUG)
    Configurable.configurableRun3Behavior = 1


    #import and set config flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Exec.MaxEvents = 1
    ConfigFlags.Exec.SkipEvents = 0
    from AthenaConfiguration.Enums import ProductionStep
    ConfigFlags.Common.ProductionStep = ProductionStep.Simulation
    ConfigFlags.Input.RunNumber = [284500] #Isn't updating - todo: investigate
    ConfigFlags.Input.OverrideRunNumber = True
    ConfigFlags.Input.LumiBlockNumber = [1] # dummy value

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    inputDir = defaultTestFiles.d
    ConfigFlags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1'] #defaultTestFiles.EVNT
    ConfigFlags.Output.HITSFileName = "myHITSnew.pool.root"
    
    #Sim ConfigFlags
    #ConfigFlags.Sim.WorldRRange = 15000
    #ConfigFlags.Sim.WorldZRange = 27000 #change defaults?
    ConfigFlags.Sim.CalibrationRun = "Off" #"DeadLAr" 
    ConfigFlags.Sim.RecordStepInfo = False
    ConfigFlags.Sim.CavernBG = "Signal"
    ConfigFlags.Sim.BeamPipeSimMode = 'FastSim'
    ConfigFlags.Sim.ReleaseGeoModel = False
    ConfigFlags.Sim.ISFRun = True
    ConfigFlags.Sim.ISF.Simulator = "FullG4MT"

    ConfigFlags.Concurrency.NumThreads = 1
    ConfigFlags.Concurrency.NumConcurrentEvents = 1

    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-14"
    ConfigFlags.GeoModel.Align.Dynamic = False

    detectors =['Bpipe', 'BCM', 'DBM',  'Pixel', 'SCT', 'TRT', 'LAr', 'Tile', 'CSC', 'MDT', 'RPC', 'TGC', 'FwdRegion']
    # Setup detector flags
    from SimuJobTransforms.SimulationTestHelpers import setupDetectorSimulateFlagsFromList
    setupDetectorSimulateFlagsFromList(ConfigFlags, detectors)

    #Frozen showers OFF = 0
    ConfigFlags.Sim.LArParameterization = 2

    ConfigFlags.Sim.TruthStrategy = "MC15aPlus"
    # Finalize
    ConfigFlags.lock()

    ## Initialize a new component accumulator
    cfg = MainServicesCfg(ConfigFlags)
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    cfg.merge(PoolReadCfg(ConfigFlags))
    cfg.merge(PoolWriteCfg(ConfigFlags))

    # add BeamEffectsAlg
    from BeamEffects.BeamEffectsAlgConfig import BeamEffectsAlgCfg
    cfg.merge(BeamEffectsAlgCfg(ConfigFlags))

    # add the ISF_MainConfig
    from ISF_Config.ISF_MainConfigNew import Kernel_FullG4MTCfg
    cfg.merge(Kernel_FullG4MTCfg(ConfigFlags))

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    from SimuJobTransforms.SimOutputConfig import getStreamHITS_ItemList
    cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=getStreamHITS_ItemList(ConfigFlags), disableEventTag=True) )

    # FIXME hack to match to buggy behaviour in old style configuration
    OutputStreamHITS = cfg.getEventAlgo("OutputStreamHITS")
    OutputStreamHITS.ItemList.remove("xAOD::EventInfo#EventInfo")
    OutputStreamHITS.ItemList.remove("xAOD::EventAuxInfo#EventInfoAux.")

    # FIXME hack because deduplication is broken
    PoolAttributes = ["TREE_BRANCH_OFFSETTAB_LEN = '100'"]
    PoolAttributes += ["DatabaseName = '" + ConfigFlags.Output.HITSFileName + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '1'"]
    cfg.getService("AthenaPoolCnvSvc").PoolAttributes += PoolAttributes

    # Dump config
    from AthenaConfiguration.ComponentFactory import CompFactory
    cfg.addEventAlgo(CompFactory.JobOptsDumperAlg(FileName="G4AtlasTestConfig.txt"))
    cfg.getService("StoreGateSvc").Dump = True
    cfg.getService("ConditionStore").Dump = True
    cfg.printConfig(withDetails=True, summariseProps = True)

    ConfigFlags.dump()

    # Execute and finish
    sc = cfg.run()

    b = time.time()
    log.info("Run ISF_MainConfigNew_Test in " + str(b-a) + " seconds")

    # Success should be 0
    #os.sys.exit(not sc.isSuccess())

    f = open("test.pkl","wb")
    cfg.store(f)
    f.close()
