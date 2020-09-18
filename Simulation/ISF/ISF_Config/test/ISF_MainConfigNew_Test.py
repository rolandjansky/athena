#!/usr/bin/env python
"""Run tests on ISF_MainConfigNew

This test inherits from Simulation/G4Atlas/G4AtlasAlg/test/G4AtlasAlgConfigNew_Test.py

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
# based on https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/Control/AthenaServices/python/Configurables.py#0247
def EvtIdModifierSvc_add_modifier(svc,
        run_nbr=None, evt_nbr=None, time_stamp=None, lbk_nbr=None,
        nevts=1):
    if run_nbr is None:
        modify_run_nbr = 0
        run_nbr = 0
    else:
        modify_run_nbr = 1

    if evt_nbr is None:
        modify_evt_nbr = 0
        evt_nbr = 0
    else:
        modify_evt_nbr = 1

    if time_stamp is None:
        modify_time_stamp = 0
        time_stamp = 0
    else:
        modify_time_stamp = 1

    if lbk_nbr is None:
        modify_lbk_nbr = 0
        lbk_nbr = 0
    else:
        modify_lbk_nbr = 1

    mod_bit = int(0b0000
                | (modify_run_nbr << 0)
                | (modify_evt_nbr << 1)
                | (modify_time_stamp << 2)
                | (modify_lbk_nbr << 3))

    svc.Modifiers += [run_nbr, evt_nbr, time_stamp, lbk_nbr,
                    nevts, mod_bit]

if __name__ == '__main__':

    import time
    a = time.time()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    import os

    # Set up logging and config behaviour
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    log.setLevel(DEBUG)
    Configurable.configurableRun3Behavior = 1


    #import and set config flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.RunNumber = [284500] #Isn't updating - todo: investigate
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    inputDir = defaultTestFiles.d
    ConfigFlags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1'] #defaultTestFiles.EVNT
    ConfigFlags.Output.HITSFileName = "myHITSnew1106_2.pool.root"
    
    #Sim ConfigFlags
    #ConfigFlags.Sim.WorldRRange = 15000
    #ConfigFlags.Sim.WorldZRange = 27000 #change defaults?
    ConfigFlags.Sim.CalibrationRun = "Off"
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

    #set the detector flags:
    #inner detectors
    ConfigFlags.Detector.SimulateBCM = True
    ConfigFlags.Detector.GeometryBCM = True
    ConfigFlags.Detector.SimulateDBM = True
    ConfigFlags.Detector.GeometryDBM = True
    ConfigFlags.Detector.SimulatePixel = True
    ConfigFlags.Detector.GeometryPixel = True
    ConfigFlags.Detector.SimulateSCT = True
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.Detector.SimulateTRT = True
    ConfigFlags.Detector.GeometryTRT = True

    #muon
    ConfigFlags.Detector.SimulateMuon = True #True
    ConfigFlags.Detector.GeometryMuon = True #True <these two break it (others can be true)
    ConfigFlags.Detector.SimulateMDT = True #True
    ConfigFlags.Detector.GeometryMDT = True #True
    ConfigFlags.Detector.SimulateRPC = True #True
    ConfigFlags.Detector.GeometryRPC = True #True
    ConfigFlags.Detector.SimulateTGC = True #True
    ConfigFlags.Detector.GeometryTGC = True #True
    ConfigFlags.Detector.SimulateCSC = True #True
    ConfigFlags.Detector.GeometryCSC = True #True

    #LAr
    ConfigFlags.Detector.SimulateLAr = True
    ConfigFlags.Detector.GeometryLAr = True
    ConfigFlags.Detector.SimulateTile = True
    ConfigFlags.Detector.GeometryTile = True

    ConfigFlags.Detector.SimulateHGTD = False
    #ConfigFlags.Detector.GeometryHGTD = False #isn't a flag -- is it needed?


    ConfigFlags.Detector.SimulateBpipe = True
    ConfigFlags.Detector.GeometryBpipe = True


    #forward region not migrated yet
    ConfigFlags.Detector.SimulateLucid = False
    ConfigFlags.Detector.SimulateZDC = False
    ConfigFlags.Detector.SimulateALFA = False
    ConfigFlags.Detector.SimulateAFP = False
    ConfigFlags.Detector.SimulateFwdRegion = False
    ConfigFlags.Detector.SimulateForward = False

    # Finalize
    ConfigFlags.lock()

    ## Initialize a new component accumulator
    cfg = MainServicesCfg(ConfigFlags)
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    cfg.merge(PoolReadCfg(ConfigFlags))
    cfg.merge(PoolWriteCfg(ConfigFlags))
    # todo its own cfg ...
    myRunNumber = 284500
    myFirstLB = 1
    myInitialTimeStamp = 1446539185
    evtMax = 4
    from AthenaConfiguration.ComponentFactory import CompFactory
    evtIdModifierSvc = CompFactory.EvtIdModifierSvc(EvtStoreName="StoreGateSvc")
    iovDbMetaDataTool = CompFactory.IOVDbMetaDataTool()
    iovDbMetaDataTool.MinMaxRunNumbers = [myRunNumber, 2147483647]
    cfg.addPublicTool(iovDbMetaDataTool)
    EvtIdModifierSvc_add_modifier(evtIdModifierSvc, run_nbr=myRunNumber, lbk_nbr=myFirstLB, time_stamp=myInitialTimeStamp, nevts=evtMax)
    eventSelector = cfg.getService("EventSelector")
    eventSelector.OverrideRunNumber = True
    eventSelector.RunNumber = myRunNumber
    eventSelector.FirstLB = myFirstLB
    eventSelector.InitialTimeStamp = myInitialTimeStamp # Necessary to avoid a crash
    if hasattr(eventSelector, "OverrideRunNumberFromInput"):
        eventSelector.OverrideRunNumberFromInput = True
    cfg.addService(evtIdModifierSvc)
    # ... up to here?

    # add BeamEffectsAlg
    from BeamEffects.BeamEffectsAlgConfig import BeamEffectsAlgCfg
    cfg.merge(BeamEffectsAlgCfg(ConfigFlags))

    # add the ISF_MainConfig
    from ISF_Config.ISF_MainConfigNew import Kernel_FullG4MTCfg
    cfg.merge(Kernel_FullG4MTCfg(ConfigFlags))

    from TileGeoG4SD.TileGeoG4SDToolConfig import TileGeoG4SDCalcCfg
    cfg.merge(TileGeoG4SDCalcCfg(ConfigFlags))

    # FIXME hack to match to buggy behaviour in old style configuration
    OutputStreamHITS = cfg.getEventAlgo("OutputStreamHITS")
    OutputStreamHITS.ItemList.remove("xAOD::EventInfo#EventInfo")
    OutputStreamHITS.ItemList.remove("xAOD::EventAuxInfo#EventInfoAux.")

    # FIXME hack because deduplication is broken
    PoolAttributes = ["TREE_BRANCH_OFFSETTAB_LEN = '100'"]
    PoolAttributes += ["DatabaseName = '" + ConfigFlags.Output.HITSFileName + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '1'"]
    cfg.getService("AthenaPoolCnvSvc").PoolAttributes += PoolAttributes

    # Dump config
    cfg.addEventAlgo(CompFactory.JobOptsDumperAlg(FileName="G4AtlasTestConfig.txt"))
    cfg.getService("StoreGateSvc").Dump = True
    cfg.getService("ConditionStore").Dump = True
    cfg.printConfig(withDetails=True, summariseProps = True)

    ConfigFlags.dump()

    # Execute and finish
    sc = cfg.run(maxEvents=evtMax)

    b = time.time()
    log.info("Run G4AtlasAlg in " + str(b-a) + " seconds")

    # Success should be 0
    #os.sys.exit(not sc.isSuccess())

    f = open("test.pkl","wb")
    cfg.store(f)
    f.close()
