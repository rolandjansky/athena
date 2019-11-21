# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from G4AtlasServices.G4AtlasServicesConfigNew import DetectorGeometrySvcCfg
from ISF_Services.ISF_ServicesConfigNew import MC15aPlusTruthServiceCfg, GeoIDSvcCfg, InputConverterCfg
from G4AtlasTools.G4AtlasToolsConfigNew import SensitiveDetectorMasterToolCfg
from G4AtlasServices.G4AtlasUserActionConfigNew import UserActionSvcCfg
from G4AtlasApps.G4Atlas_MetadataNew import writeSimulationParametersMetadata


#todo - think about the flow, do we need if statements?!

from  G4AtlasAlg.G4AtlasAlgConf import G4AtlasAlg

def G4AtlasAlgCfg(ConfigFlags, name='G4AtlasAlg', **kwargs):
    #add Services to G4AtlasAlg
    result = DetectorGeometrySvcCfg(ConfigFlags)
    kwargs.setdefault('DetGeoSvc', result.getService("DetectorGeometrySvc"))

    
    kwargs.setdefault("InputTruthCollection", "BeamTruthEvent") #tocheck -are these string inputs?
    kwargs.setdefault("OutputTruthCollection", "TruthEvent")
    ## Killing neutrinos
    if ConfigFlags.Sim.ReleaseGeoModel:
        ## Don't drop the GeoModel
        kwargs.setdefault('ReleaseGeoModel' ,ConfigFlags.Sim.ReleaseGeoModel)

    if ConfigFlags.Sim.RecordFlux:
        ## Record the particle flux during the simulation
        kwargs.setdefault('RecordFlux' , ConfigFlags.Sim.RecordFlux)

    if ConfigFlags.Sim.FlagAbortedEvents:
        ## default false
        kwargs.setdefault('FlagAbortedEvents' ,ConfigFlags.Sim.FlagAbortedEvents)
        if ConfigFlags.Sim.FlagAbortedEvents and ConfigFlags.Sim.KillAbortedEvents:
            print('WARNING When G4AtlasAlg.FlagAbortedEvents is True G4AtlasAlg.KillAbortedEvents should be False!!! Setting G4AtlasAlg.KillAbortedEvents = False now!')
            kwargs.setdefault('KillAbortedEvents' ,False)
    if  ConfigFlags.Sim.KillAbortedEvents:
        ## default true
        kwargs.setdefault('KillAbortedEvents' ,ConfigFlags.Sim.KillAbortedEvents)

    from RngComps.RandomServices import AthEngines, RNG
    if ConfigFlags.Random.Engine in AthEngines.keys():
        result.merge(RNG(ConfigFlags.Random.Engine, name="AthRNGSvc"))
        kwargs.setdefault("AtRndmGenSvc",result.getService("AthRNGSvc"))

    kwargs.setdefault("RandomGenerator", "athena")

    # Multi-threading settinggs
    #is_hive = (concurrencyProps.ConcurrencyFlags.NumThreads() > 0)
    is_hive = ConfigFlags.Concurrency.NumThreads > 0
    kwargs.setdefault('MultiThreading', is_hive)


    accMCTruth = MC15aPlusTruthServiceCfg(ConfigFlags)
    result.merge(accMCTruth)
    kwargs.setdefault('TruthRecordService', result.getService("ISF_MC15aPlusTruthService"))
    #kwargs.setdefault('TruthRecordService', ConfigFlags.Sim.TruthStrategy) # TODO need to have manual override (simFlags.TruthStrategy.TruthServiceName())

    accGeoID = GeoIDSvcCfg(ConfigFlags)
    result.merge(accGeoID)
    kwargs.setdefault('GeoIDSvc', result.getService('ISF_GeoIDSvc'))

    #input converter
    accInputConverter = InputConverterCfg(ConfigFlags)
    result.merge(accInputConverter)
    kwargs.setdefault('InputConverter', result.getService("ISF_InputConverter"))

    #sensitive detector master tool
    accSensitiveDetector = SensitiveDetectorMasterToolCfg(ConfigFlags)
    result.merge(accSensitiveDetector)
    kwargs.setdefault('SenDetMasterTool', result.getPublicTool("SensitiveDetectorMasterTool")) #NOTE - is still a public tool

    #Write MetaData container
    result.merge(writeSimulationParametersMetadata(ConfigFlags))


    result.merge( UserActionSvcCfg(ConfigFlags) )
    kwargs.setdefault('UserActionSvc', result.getService( "G4UA::UserActionSvc") )


    ## G4AtlasAlg verbosities (available domains = Navigator, Propagator, Tracking, Stepping, Stacking, Event)
    ## Set stepper verbose = 1 if the Athena logging level is <= DEBUG
    # TODO: Why does it complain that G4AtlasAlgConf.G4AtlasAlg has no "Verbosities" object? Fix.
    verbosities=dict()
    #from AthenaCommon.AppMgr import ServiceMgr
    #if ServiceMgr.MessageSvc.OutputLevel <= 2:
    #    verbosities["Tracking"]='1'
    #    print verbosities
    kwargs.setdefault('Verbosities', verbosities)

    # Set commands for the G4AtlasAlg
    kwargs.setdefault("G4Commands", ConfigFlags.Sim.G4Commands)

    result.addEventAlgo(G4AtlasAlg(name, **kwargs))

    return result


if __name__ == '__main__':
  from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
  import os

  # Set up logging and config behaviour
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  from AthenaCommon.Configurable import Configurable
  log.setLevel(DEBUG)
  Configurable.configurableRun3Behavior = 1


  #import config flags
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  ConfigFlags.Input.Files = defaultTestFiles.EVNT

  ConfigFlags.Sim.WorldRRange = 15000
  ConfigFlags.Sim.WorldZRange = 27000 #change defaults?

  ConfigFlags.Sim.ISF.Run = False 
  ConfigFlags.Detector.SimulateForward = False

  ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
  ConfigFlags.GeoModel.Align.Dynamic = False

  #set up to simulate all sensitive detectors
  #ConfigFlags.Detector.SimulateBpipe = True #issue with AthRNGSvc?
  ConfigFlags.Detector.SimulateBCM = True
  ConfigFlags.Detector.SimulateDBM = True
  ConfigFlags.Detector.SimulatePixel = True
  ConfigFlags.Detector.SimulateSCT = True
  ConfigFlags.Detector.SimulateTRT = True 
  #ConfigFlags.Detector.SimulateCavern = True

  #ConfigFlags.Detector.GeometryBpipe = True
  ConfigFlags.Detector.GeometryBCM = True
  ConfigFlags.Detector.GeometryDBM = True
  ConfigFlags.Detector.GeometryPixel = True
  ConfigFlags.Detector.GeometrySCT = True
  ConfigFlags.Detector.GeometryTRT = True 
  #ConfigFlags.Detector.GeometryCavern = True


  #muon
  ConfigFlags.Detector.SimulateMuon = True
  ConfigFlags.Detector.GeometryMuon = True
  ConfigFlags.Detector.SimulateMDT = True
  ConfigFlags.Detector.GeometryMDT = True
  ConfigFlags.Detector.SimulateRPC = True
  ConfigFlags.Detector.GeometryRPC = True
  ConfigFlags.Detector.SimulateTGC = True
  ConfigFlags.Detector.GeometryTGC = True
  ConfigFlags.Detector.SimulateCSC = True
  ConfigFlags.Detector.GeometryCSC = True

  #muon not migrated yet
  ConfigFlags.Detector.SimulateMM = False 

  #LAr
  ConfigFlags.Detector.SimulateLAr = True 
  ConfigFlags.Detector.GeometryLAr = True # geometry wasnt set up -> so one pointer didn't exist so it didn't work...!

  ConfigFlags.Detector.SimulateHGTD = False
  #ConfigFlags.Detector.GeometryHGTD = False #isn't a flag -- is it needed?

  ConfigFlags.Detector.SimulateTile = True
  ConfigFlags.Detector.GeometryTile = True

  ConfigFlags.Sim.CalibrationRun = 'DeadLAr'
  



  ConfigFlags.Sim.RecordStepInfo = False

  #forward region not migrated yet
  ConfigFlags.Detector.SimulateLucid = False
  ConfigFlags.Detector.SimulateZDC = False
  ConfigFlags.Detector.SimulateALFA = False
  ConfigFlags.Detector.SimulateAFP = False
  ConfigFlags.Detector.SimulateFwdRegion = False

  ConfigFlags.Sim.CavernBG = "Signal"
  #ConfigFlags.GeoModel.Align.Dynamic = True

  #to match old style
  #ConfigFlags.Input.RunNumber = 222510 #Isn't updating - todo: investigate

  # Finalize 
  ConfigFlags.lock()

  ## Initialize a new component accumulator
  cfg = MainServicesSerialCfg()

  from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
  from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
  # Add configuration to read EVNT pool file
  cfg.merge(PoolReadCfg(ConfigFlags))


  #add BeamEffectsAlg
  from BeamEffects.BeamEffectsAlgConfig import BeamEffectsAlgCfg
  acc, alg = BeamEffectsAlgCfg(ConfigFlags) #todo - update BeamEffectsAlg
  cfg.merge(acc)
  cfg.addEventAlgo(alg, sequenceName="AthAlgSeq")
  
  #add the G4AtlasAlg
  cfg.merge(G4AtlasAlgCfg(ConfigFlags))

  #inner detector
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["SiHitCollection#SCT_Hits"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["SiHitCollection#BCMHits"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["SiHitCollection#BLMHits"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["SiHitCollection#PixelHits"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["TRTUncompressedHitCollection#TRTUncompressedHits"]))

  #LAr + Tile Calo
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["LArHitContainer#LArHitEMB"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["LArHitContainer#LArHitEMEC"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["LArHitContainer#LArHitFCAL"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["LArHitContainer#LArHitHEC"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["LArHitContainer#LArHitMiniFCAL"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["TileHitVector#TileHitVec"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["CaloCalibrationHitContainer#LArCalibrationHitDeadMaterial"]))


  
  #muon
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["RPCSimHitCollection#RPC_Hits"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["TGCSimHitCollection#TGC_Hits"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["MDTSimHitCollection#MDT_Hits"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["CSCSimHitCollection#CSC_Hits"]))

  #assorted
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["EventInfo#McEventInfo"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["McEventCollection#TruthEvent"]))

  #not working?
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["TrackRecordCollection#CaloEntryLayer"])) 
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["TrackRecordCollection#MuonEntryLayer"])) 
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["TrackRecordCollection#MuonExitLayer"])) 


  #not working? 
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["IOVMetaDataContainer#Simulation_Parameters"]))


  #try
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["TileHitVector#MBTSHits"]))
  
  #          if (CommonGeometryFlags.Run() in ["RUN3", "RUN4"]):
  #              stream1.ItemList += ["sTGCSimHitCollection#*"]
  #              stream1.ItemList += ["MMSimHitCollection#*"]



  # Dump config
  cfg.getService("StoreGateSvc").Dump = True
  cfg.getService("ConditionStore").Dump = True
  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()


  # Execute and finish
  sc = cfg.run(maxEvents=1)
  # Success should be 0
  os.sys.exit(not sc.isSuccess())

  #f=open("test.pkl","w")
  #cfg.store(f) 
  #f.close()



  print(cfg._publicTools)
  print("-----------------finished----------------------")
