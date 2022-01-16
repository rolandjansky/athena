# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
# TODO: clean up flags, should only contain general settings but no alg config
# TODO : Add some exta levels?

def createInDetConfigFlags():
  icf=AthConfigFlags()

  ### Detector flags

  icf.addFlag("InDet.doSplitReco", False) # Turn running of the truth seeded pseudo tracking only for pileup on and off. Only makes sense to run on RDO file where SplitDigi was used!
  icf.addFlag("InDet.doTruth", lambda prevFlags: prevFlags.Input.isMC) # Turn running of truth matching on and off (by default on for MC off for data)
  icf.addFlag("InDet.selectSCTIntimeHits", lambda prevFlags: not(prevFlags.Beam.Type=="cosmics" or prevFlags.InDet.Tracking.doVtxBeamSpot) ) # defines if the X1X mode is used for the offline or not
  icf.addFlag("InDet.useDCS", True)
  icf.addFlag("InDet.usePixelDCS",  lambda prevFlags : (prevFlags.InDet.useDCS and prevFlags.Detector.EnablePixel))
  icf.addFlag("InDet.useSctDCS",  lambda prevFlags : (prevFlags.InDet.useDCS and prevFlags.Detector.EnableSCT))
  icf.addFlag("InDet.ForceCoraCool", False) # Use old (non CoolVectorPayload) SCT Conditions
  icf.addFlag("InDet.ForceCoolVectorPayload", False) # Use new (CoolVectorPayload) SCT Conditions
  icf.addFlag("InDet.doSCTModuleVeto", False) # Turn on SCT_ModuleVetoSvc, allowing it to be configured later
  icf.addFlag("InDet.checkDeadElementsOnTrack", True) # Enable check for dead modules and FEs
  icf.addFlag("InDet.doTRTGlobalOccupancy", False) # Turn running of Event Info TRT Occupancy Filling Alg on and off (also whether it is used in TRT PID calculation)
  icf.addFlag("InDet.noTRTTiming", lambda prevFlags: prevFlags.Beam.Type=="singlebeam")


  ### Tracking parameters

  icf.addFlag("InDet.Tracking.doStoreTrackSeeds", False) # Turn on to save the Track Seeds in a xAOD track collecting for development studies
  icf.addFlag("InDet.Tracking.materialInteractions", True)
  icf.addFlag("InDet.Tracking.materialInteractionsType", lambda prevFlags: 2 if prevFlags.Beam.Type=="cosmics" else 3) # Control which type of particle hypothesis to use for the material interactions 0=non-interacting,1=electron,2=muon,3=pion,4=kaon,5=proton. See ParticleHypothesis.h for full definition.
  icf.addFlag("InDet.Tracking.doDigitalROTCreation",False) # use PixelClusterOnTrackToolDigital during ROT creation to save CPU
  icf.addFlag("InDet.Tracking.holeSearchInGX2Fit", True)
  icf.addFlag("InDet.Tracking.trackFitterType", "GlobalChi2Fitter") # control which fitter to be used: 'KalmanFitter', 'KalmanDNAFitter', 'DistributedKalmanFilter', 'GlobalChi2Fitter', 'GaussianSumFilter'
  icf.addFlag("InDet.Tracking.kalmanUpdator", "smatrix") # control which updator to load for KalmanFitter ("None"/"fast"/"smatrix"/"weight"/"amg")
  icf.addFlag("InDet.Tracking.propagatorType", "RungeKutta") # control which propagator to use ('RungeKutta'/'STEP')
  icf.addFlag("InDet.Tracking.doSharedHits", True) # control if the shared hits are recorded in TrackPatricles
  icf.addFlag("InDet.Tracking.perigeeExpression", lambda prevFlags: "Vertex" if prevFlags.Reco.EnableHI else "BeamLine") # Express track parameters wrt. to : 'BeamLine','BeamSpot','Vertex' (first primary vertex)
  icf.addFlag("InDet.Tracking.cutLevel", 19) # Control cuts and settings for different lumi to limit CPU and disk space
  icf.addFlag("InDet.Tracking.doTIDE_Ambi", lambda prevFlags: not prevFlags.InDet.Tracking.doInnerDetectorCommissioning) # Switch for running TIDE Ambi
  icf.addFlag("InDet.Tracking.doBremRecovery", lambda prevFlags: not(prevFlags.InDet.Tracking.doVtxLumi or prevFlags.InDet.Tracking.doVtxBeamSpot or prevFlags.InDet.Tracking.doLowMu or prevFlags.Beam.Type!="collisions" or not prevFlags.BField.solenoidOn) ) # Turn on running of Brem Recover in tracking
  icf.addFlag("InDet.Tracking.doCaloSeededBrem", True) # Brem Recover in tracking restricted to Calo ROIs
  icf.addFlag("InDet.Tracking.doHadCaloSeededSSS", False) # Use Recover SSS to Calo ROIs
  icf.addFlag("InDet.Tracking.doCaloSeededAmbi", lambda prevFlags: prevFlags.Detector.EnableCalo) # Use Calo ROIs to seed specific cuts for the ambi
  icf.addFlag("InDet.Tracking.doPixelClusterSplitting", lambda prevFlags: not prevFlags.InDet.Tracking.doInnerDetectorCommissioning) # Try to split pixel clusters
  icf.addFlag("InDet.Tracking.pixelClusterSplittingType", "NeuralNet") # choose splitter type: NeuralNet or AnalogClus
  icf.addFlag("InDet.Tracking.pixelClusterSplitProb1", lambda prevFlags: 0.5 if prevFlags.GeoModel.Run == 'RUN1' else 0.55) # Cut value for splitting clusters into two parts
  icf.addFlag("InDet.Tracking.pixelClusterSplitProb2", lambda prevFlags: 0.5 if prevFlags.GeoModel.Run == 'RUN1' else 0.45) # Cut value for splitting clusters into three parts
  icf.addFlag("InDet.Tracking.useBeamSpotInfoNN", True) # use beam spot position in pixel NN
  icf.addFlag("InDet.Tracking.nnCutLargeD0Threshold", -1.0) # Enable check for dead modules and FEs
  icf.addFlag("InDet.Tracking.useBroadPixClusterErrors", False) # Use broad cluster errors for Pixel
  icf.addFlag("InDet.Tracking.useBroadSCTClusterErrors", False) # Use broad cluster errors for SCT
  icf.addFlag("InDet.Tracking.doRefitInvalidCov", False) # Try Kalman fitter if the track fit in the ambiguity processor produces non positive definitematrices.
  icf.addFlag("InDet.Tracking.trtExtensionType", "xk") # which extension type ("xk"/"DAF")
  icf.addFlag("InDet.Tracking.redoTRT_LR", lambda prevFlags: not prevFlags.InDet.Tracking.trtExtensionType=="DAF") # use smart TRT LR/tube hit creator and redo ROTs
  icf.addFlag("InDet.Tracking.doTRTPhaseCalculation", lambda prevFlags: prevFlags.Beam.Type=="cosmics") # control to run TRT phase calculation

  ### Tracking passes/configurations scheduled

  icf.addFlag("InDet.Tracking.doTrackSegmentsPixel", lambda prevFlags: prevFlags.InDet.Tracking.doMinBias or prevFlags.InDet.Tracking.doLowMu or prevFlags.InDet.Tracking.doInnerDetectorCommissioning) # Turn running of track segment creation in pixel on and off
  icf.addFlag("InDet.Tracking.doTrackSegmentsSCT", lambda prevFlags: prevFlags.InDet.Tracking.doLowMu or prevFlags.InDet.Tracking.doInnerDetectorCommissioning) # Turn running of track segment creation in SCT on and off
  icf.addFlag("InDet.Tracking.doTrackSegmentsTRT", lambda prevFlags: prevFlags.InDet.Tracking.doLowMu or prevFlags.InDet.Tracking.doInnerDetectorCommissioning) # Turn running of track segment creation in TRT on and off
  icf.addFlag("InDet.Tracking.doDBMstandalone",False)
  icf.addFlag("InDet.Tracking.doHighPileup", False) # Turn running of high pile-up reconstruction on and off
  icf.addFlag("InDet.Tracking.doTRTExtension", True) # turn on / off TRT extensions
  icf.addFlag("InDet.Tracking.doTRTSegments", lambda prevFlags: prevFlags.InDet.Tracking.doBackTracking or prevFlags.InDet.Tracking.doTRTStandalone) # control to run TRT Segment finding (do it always after new tracking!)
  icf.addFlag("InDet.Tracking.doBackTracking", lambda prevFlags: not(prevFlags.Beam.Type in ["singlebeam", "cosmics"] or prevFlags.Reco.EnableHI or prevFlags.InDet.Tracking.doHighPileup or prevFlags.InDet.Tracking.doVtxLumi or prevFlags.InDet.Tracking.doVtxBeamSpot) ) # Turn running of backtracking on and off
  icf.addFlag("InDet.Tracking.doLargeD0", False)
  icf.addFlag("InDet.Tracking.doR3LargeD0", lambda prevFlags: not(prevFlags.Beam.Type in ["singlebeam", "cosmics"] or prevFlags.Reco.EnableHI or prevFlags.InDet.Tracking.doHighPileup or prevFlags.InDet.Tracking.doVtxLumi or prevFlags.InDet.Tracking.doVtxBeamSpot) )
  icf.addFlag("InDet.Tracking.doLowPtLargeD0", False) # Turn running of doLargeD0 second pass down to 100 MeV on and off Turn running of doLargeD0 second pass on and off
  icf.addFlag("InDet.Tracking.storeSeparateLargeD0Container", True)
  icf.addFlag("InDet.Tracking.doLowPt", lambda prevFlags: prevFlags.InDet.Tracking.doLowMu) # Turn running of doLowPt second pass on and off
  icf.addFlag("InDet.Tracking.doVeryLowPt", False) # Turn running of doVeryLowPt thrid pass on and off
  icf.addFlag("InDet.Tracking.doTRTStandalone", lambda prevFlags: not(prevFlags.Reco.EnableHI or prevFlags.InDet.Tracking.doHighPileup or prevFlags.InDet.Tracking.doVtxLumi or prevFlags.InDet.Tracking.doVtxBeamSpot) ) # control TRT Standalone
  icf.addFlag("InDet.Tracking.doForwardTracks", lambda prevFlags: not(prevFlags.Beam.Type in ["singlebeam", "cosmics"] or prevFlags.Reco.EnableHI or prevFlags.InDet.Tracking.doHighPileup or prevFlags.InDet.Tracking.doVtxLumi or prevFlags.InDet.Tracking.doVtxBeamSpot or prevFlags.InDet.Tracking.doMinBias or prevFlags.InDet.Tracking.doLowMu)) # Turn running of doForwardTracks pass on and off
  icf.addFlag("InDet.Tracking.doTrackSegmentsDisappearing", lambda prevFlags: not(prevFlags.Reco.EnableHI or prevFlags.Reco.EnableHI or prevFlags.InDet.Tracking.doInnerDetectorCommissioning) )
  icf.addFlag("InDet.Tracking.doBeamGas", lambda prevFlags: prevFlags.Beam.Type=="singlebeam") # Turn running of BeamGas second pass on and off
  icf.addFlag("InDet.Tracking.doMinBias", False) # Switch for running MinBias settings
  icf.addFlag("InDet.Tracking.doHIP300", False) # Switch for running MinBias settings with a 300 MeV pT cut (for Heavy Ion Proton)
  icf.addFlag("InDet.Tracking.doRobustReco", False) # Switch for running Robust settings
  icf.addFlag("InDet.Tracking.doInnerDetectorCommissioning", False) # Switch for running looser settings in ID for commissioning
  icf.addFlag("InDet.Tracking.doBLS", False) # Special reconstruction for BLS physics
  icf.addFlag("InDet.Tracking.doVtxLumi", False) # Special reconstruction for vertex lumi measurement
  icf.addFlag("InDet.Tracking.doVtxBeamSpot", False) # Special reconstruction for vertex beamspot measurement
  icf.addFlag("InDet.Tracking.doLowMu", False) # Special configuration for low-mu runs

  from InDetConfig.TrackingPassFlags import createTrackingPassFlags, createHighPileupTrackingPassFlags, \
    createMinBiasTrackingPassFlags, createLargeD0TrackingPassFlags, createR3LargeD0TrackingPassFlags, createLowPtLargeD0TrackingPassFlags, \
    createLowPtTrackingPassFlags, createVeryLowPtTrackingPassFlags, createForwardTracksTrackingPassFlags, createBeamGasTrackingPassFlags, \
    createVtxLumiTrackingPassFlags, createVtxBeamSpotTrackingPassFlags, createCosmicsTrackingPassFlags, createHeavyIonTrackingPassFlags, \
    createPixelTrackingPassFlags, createDisappearingTrackingPassFlags, createSCTTrackingPassFlags, createTRTTrackingPassFlags, \
    createTRTStandaloneTrackingPassFlags, createDBMTrackingPassFlags, createRobustRecoTrackingPassFlags

  icf.addFlagsCategory ("InDet.Tracking.ActivePass", createTrackingPassFlags, prefix=True) # Set up for first tracking pass, updated for second passes
  icf.addFlagsCategory ("InDet.Tracking.HighPileupPass", createHighPileupTrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.MinBiasPass", createMinBiasTrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.LargeD0Pass", createLargeD0TrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.R3LargeD0Pass", createR3LargeD0TrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.LowPtLargeD0Pass", createLowPtLargeD0TrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.LowPtPass", createLowPtTrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.VeryLowPtPass", createVeryLowPtTrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.ForwardTracksPass", createForwardTracksTrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.BeamGasPass", createBeamGasTrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.VtxLumiPass", createVtxLumiTrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.VtxBeamSpotPass", createVtxBeamSpotTrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.CosmicsPass", createCosmicsTrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.HeavyIonPass", createHeavyIonTrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.PixelPass", createPixelTrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.DisappearingPass", createDisappearingTrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.SCTPass", createSCTTrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.TRTPass", createTRTTrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.TRTStandalonePass", createTRTStandaloneTrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.DBMPass", createDBMTrackingPassFlags, prefix=True)
  icf.addFlagsCategory ("InDet.Tracking.RobustRecoPass", createRobustRecoTrackingPassFlags, prefix=True)

  from InDetConfig.VertexFindingFlags import createSecVertexingFlags, createEGammaPileUpSecVertexingFlags, createPriVertexingFlags
  icf.addFlagsCategory("InDet.PriVertex", createPriVertexingFlags, prefix=True)
  icf.addFlagsCategory("InDet.SecVertex", createSecVertexingFlags, prefix=True)
  icf.addFlagsCategory("InDet.SecVertexEGammaPileUp", createEGammaPileUpSecVertexingFlags, prefix=True)
  return icf
