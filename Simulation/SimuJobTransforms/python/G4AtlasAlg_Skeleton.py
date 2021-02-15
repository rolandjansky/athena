# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys
from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude

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

def defaultSimulationFlags(ConfigFlags):
    """Fill default simulation flags"""
    # TODO: how to autoconfigure those
    from AthenaConfiguration.Enums import ProductionStep
    ConfigFlags.Common.ProductionStep = ProductionStep.Simulation
    ConfigFlags.Sim.CalibrationRun = "Off" #"DeadLAr" 
    ConfigFlags.Sim.RecordStepInfo = False
    ConfigFlags.Sim.CavernBG = "Signal"
    ConfigFlags.Sim.BeamPipeSimMode = 'FastSim'
    ConfigFlags.Sim.ReleaseGeoModel = False
    ConfigFlags.Sim.ISFRun = False # Need to change this for G4AA and ISF!
    ConfigFlags.GeoModel.Align.Dynamic = False

    #Frozen showers OFF = 0
    # ConfigFlags.Sim.LArParameterization = 2


    #set the detector flags: - all on currently
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




def fromRunArgs(runArgs):
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    from AthenaCommon.Logging import logging
    log = logging.getLogger('Overlay')
    log.info('****************** STARTING Simulation *****************')

    log.info('**** Transformation run arguments')
    log.info(str(runArgs))

    log.info('**** Setting-up configuration flags')
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    commonRunArgsToFlags(runArgs, ConfigFlags)

    if hasattr(runArgs, 'inputEVNTFile'):
        ConfigFlags.Input.Files = runArgs.inputEVNTFile
    else:
        raise RuntimeError('No input EVNT file defined')

    if hasattr(runArgs, 'outputHITSFile'):
        if runArgs.outputHITSFile == 'None':
            ConfigFlags.Output.HITSFileName = ''
        else:
            ConfigFlags.Output.HITSFileName  = runArgs.outputHITSFile
    else:
        raise RuntimeError('No outputHITSFile defined')

    if hasattr(runArgs, 'DataRunNumber'):
        ConfigFlags.Input.RunNumber = [runArgs.DataRunNumber] # is it updating?

    if hasattr(runArgs, 'outputHITSFile'):
        ConfigFlags.Sim.PhysicsList = runArgs.physicsList

    if hasattr(runArgs, 'conditionsTag'):
        ConfigFlags.IOVDb.GlobalTag = runArgs.conditionsTag

    if hasattr(runArgs, 'simulator'):
        ConfigFlags.Sim.ISF.Simulator = runArgs.simulator

    if hasattr(runArgs, 'truthStrategy'):
        ConfigFlags.Sim.TruthStrategy = runArgs.truthStrategy

    if hasattr(runArgs, 'maxEvents'):
        evtMax = runArgs.maxEvents
    else:
        evtMax = -1
    
    # Setup common simulation flags
    defaultSimulationFlags(ConfigFlags)

    # Pre-include
    processPreInclude(runArgs, ConfigFlags)

    # Pre-exec
    processPreExec(runArgs, ConfigFlags)

    # Lock flags
    ConfigFlags.lock()

    #run ISF
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    cfg.merge(PoolReadCfg(ConfigFlags))
    cfg.merge(PoolWriteCfg(ConfigFlags))
    # todo its own cfg ...
    #todo check evtMax=-1 works with this method
    myRunNumber = 284500
    myFirstLB = 1
    myInitialTimeStamp = 1446539185
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
    cfg.addService(evtIdModifierSvc, create=True)
    # ... up to here?

    # add BeamEffectsAlg
    from BeamEffects.BeamEffectsAlgConfig import BeamEffectsAlgCfg
    cfg.merge(BeamEffectsAlgCfg(ConfigFlags))

    #add the G4AtlasAlg
    from G4AtlasAlg.G4AtlasAlgConfigNew import G4AtlasAlgCfg
    cfg.merge(G4AtlasAlgCfg(ConfigFlags))
    
    from TileGeoG4SD.TileGeoG4SDToolConfig import TileGeoG4SDCalcCfg
    cfg.merge(TileGeoG4SDCalcCfg(ConfigFlags))

    #Add to item list 
    #TODO - make a separate function (combine with G4AtlasAlg one?)
    ItemList = ["EventInfo#*",
                "McEventCollection#TruthEvent",
                "JetCollection#*"]

    if ConfigFlags.Sim.IncludeParentsInG4Event:
        ItemList += ["McEventCollection#GEN_EVENT"]

    ItemList += ["xAOD::JetContainer#*",
                 "xAOD::JetAuxContainer#*"]

    if ConfigFlags.Detector.SimulateID:
        ItemList += ["SiHitCollection#*",
                     "TRTUncompressedHitCollection#*",
                     "TrackRecordCollection#CaloEntryLayer"]

    if ConfigFlags.Detector.SimulateITk:
        ItemList += ["SiHitCollection#*",
                     "TrackRecordCollection#CaloEntryLayer"]

    if ConfigFlags.Detector.SimulateCalo:
        ItemList += ["CaloCalibrationHitContainer#*",
                     "LArHitContainer#*",
                     "TileHitVector#*",
                     "TrackRecordCollection#MuonEntryLayer"]

    if ConfigFlags.Detector.SimulateMuon:
        ItemList += ["RPCSimHitCollection#*",
                     "TGCSimHitCollection#*",
                     "MDTSimHitCollection#*",
                     "TrackRecordCollection#MuonExitLayer"]
        if ConfigFlags.Detector.GeometryCSC:
            ItemList += ["CSCSimHitCollection#*"]
        if ConfigFlags.Detector.GeometrysTGC:
            ItemList += ["sTGCSimHitCollection#*"]
        if ConfigFlags.Detector.GeometryMM:
            ItemList += ["MMSimHitCollection#*"]

    if ConfigFlags.Detector.SimulateLucid:
        ItemList += ["LUCID_SimHitCollection#*"]

    if ConfigFlags.Detector.SimulateFwdRegion:
        ItemList += ["SimulationHitCollection#*"]

    if ConfigFlags.Detector.SimulateZDC:
        ItemList += ["ZDC_SimPixelHit_Collection#*",
                     "ZDC_SimStripHit_Collection#*"]

    if ConfigFlags.Detector.SimulateALFA:
        ItemList += ["ALFA_HitCollection#*",
                     "ALFA_ODHitCollection#*"]

    if ConfigFlags.Detector.SimulateAFP:
        ItemList += ["AFP_TDSimHitCollection#*",
                     "AFP_SIDSimHitCollection#*"]

    # TimingAlg
    ItemList += ["RecoTimingObj#EVNTtoHITS_timings"]

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=ItemList, disableEventTag=True) )

    # FIXME hack because deduplication is broken
    PoolAttributes = ["TREE_BRANCH_OFFSETTAB_LEN = '100'"]
    PoolAttributes += ["DatabaseName = '" + ConfigFlags.Output.HITSFileName + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '1'"]
    cfg.getService("AthenaPoolCnvSvc").PoolAttributes += PoolAttributes


    # Post-include
    processPostInclude(runArgs, ConfigFlags, cfg)

    # Post-exec
    processPostExec(runArgs, ConfigFlags, cfg)

    import time
    tic = time.time()
    # Run the final accumulator
    sc = cfg.run(maxEvents=evtMax)
    log.info("Run G4AtlasAlg in " + str(time.time()-tic) + " seconds")
    
    sys.exit(not sc.isSuccess())
