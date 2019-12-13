#!/usr/bin/env python
"""Run tests on G4AtlasAlgConfigNew

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
if __name__ == '__main__':

  import time
  a = time.time()

  from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
  import os

  # Set up logging and config behaviour
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  from AthenaCommon.Configurable import Configurable
  log.setLevel(DEBUG)
  Configurable.configurableRun3Behavior = 1


  #import and set config flags
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  ConfigFlags.Input.Files = defaultTestFiles.EVNT
  ConfigFlags.Output.HITSFileName = "myHITSnew.pool.root"

  #Sim ConfigFlags
  ConfigFlags.Sim.WorldRRange = 15000
  ConfigFlags.Sim.WorldZRange = 27000 #change defaults?
  ConfigFlags.Sim.CalibrationRun = False # 'DeadLAr'
  ConfigFlags.Sim.RecordStepInfo = False
  ConfigFlags.Sim.CavernBG = "Signal"
  ConfigFlags.Sim.ISFRun = False

  ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
  ConfigFlags.GeoModel.Align.Dynamic = False
  #ConfigFlags.Input.RunNumber = 222510 #Isn't updating - todo: investigate


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

  #LAr
  ConfigFlags.Detector.SimulateLAr = True 
  ConfigFlags.Detector.GeometryLAr = True
  ConfigFlags.Detector.SimulateTile = True
  ConfigFlags.Detector.GeometryTile = True

  ConfigFlags.Detector.SimulateHGTD = False
  #ConfigFlags.Detector.GeometryHGTD = False #isn't a flag -- is it needed?



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
  cfg = MainServicesSerialCfg()

  # Add configuration to read EVNT pool file
  from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
  cfg.merge(PoolReadCfg(ConfigFlags))


  #add BeamEffectsAlg
  from BeamEffects.BeamEffectsAlgConfig import BeamEffectsAlgCfg
  acc, alg = BeamEffectsAlgCfg(ConfigFlags) #todo - update BeamEffectsAlg
  cfg.merge(acc)
  cfg.addEventAlgo(alg, sequenceName="AthAlgSeq")
  
  #add the G4AtlasAlg
  from G4AtlasAlg.G4AtlasAlgConfigNew import G4AtlasAlgCfg
  cfg.merge(G4AtlasAlgCfg(ConfigFlags))

  #write the output stream
  from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

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
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["TileHitVector#MBTSHits"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["CaloCalibrationHitContainer#LArCalibrationHitDeadMaterial"]))

  #muon
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["RPCSimHitCollection#RPC_Hits"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["TGCSimHitCollection#TGC_Hits"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["MDTSimHitCollection#MDT_Hits"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["CSCSimHitCollection#CSC_Hits"]))

  #Extra information
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["EventInfo#McEventInfo"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["McEventCollection#TruthEvent"]))
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["IOVMetaDataContainer#Simulation_Parameters"]))

  #TrackCollections
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["TrackRecordCollection#CaloEntryLayer"])) 
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["TrackRecordCollection#MuonEntryLayer"])) 
  cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=["TrackRecordCollection#MuonExitLayer"])) 


  # Dump config
  cfg.getService("StoreGateSvc").Dump = True
  cfg.getService("ConditionStore").Dump = True
  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()


  # Execute and finish
  sc = cfg.run(maxEvents=1)


  b = time.time()
  from AthenaCommon import Logging
  log.info("Run G4AtlasAlg in " + str(b-a) + " seconds")

  # Success should be 0
  os.sys.exit(not sc.isSuccess())

  f=open("test.pkl","wb")
  cfg.store(f) 
  f.close()
