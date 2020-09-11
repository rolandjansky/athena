# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
# TODO: clean up flags, should only contain general settings but no alg config
# TODO : Add some exta levels?

def createInDetConfigFlags():
  icf=AthConfigFlags()

  icf.addFlag("InDet.doDBMstandalone",False)
  icf.addFlag("InDet.doDBM",False)
  icf.addFlag("InDet.doPrintConfigurables",False) # if this is on all the print(InDetXYZ) lines are activated
  # FIXME: Algorithm property instead of flag
  icf.addFlag("InDet.doNewTracking", True) # Turn running of newTracking on and off
  # FIXME: Flags shouldn't turn on/off individual algorithms
  icf.addFlag("InDet.doPseudoTracking", False ) # Turn running of the truth seeded pseudo tracking on and off
  icf.addFlag("InDet.doIdealPseudoTracking", True) # Run pseudoTracking with 100\% hit assignment efficiency
  icf.addFlag("InDet.doSplitReco", False    	 ) # Turn running of the truth seeded pseudo tracking only for pileup on and off. Only makes sense to run on RDO file where SplitDigi was used!
  icf.addFlag("InDet.preProcessing", True) # Turn running of pre processing on and off
  icf.addFlag("InDet.doPRDFormation", True) # Turn running of PRD formation on and off
  icf.addFlag("InDet.doPixelPRDFormation", True) # Turn running of pixel PRD formation on and off
  icf.addFlag("InDet.doSCT_PRDFormation", True) # Turn running of SCT PRD formation on and off
  icf.addFlag("InDet.doTRT_PRDFormation", True) # Turn running of TRT PRD formation on and off
  icf.addFlag("InDet.doSpacePointFormation", True) # Turn running of space point formation on and off
  icf.addFlag("InDet.doRefit", False) # Turn running of refitting on and off
  icf.addFlag("InDet.doSLHC", False) # Turn running of SLHC reconstruction on and off
  icf.addFlag("InDet.doIBL", False) # Turn running of IBL reconstruction on and off
  icf.addFlag("InDet.doHighPileup", False) # Turn running of high pile-up reconstruction on and off
  icf.addFlag("InDet.doMinimalReco", False) # Turn running of minimal reconstruction on and off
  icf.addFlag("InDet.doDVRetracking", False) # Turn running of large-d0 retracking mode on and off. This flag assumes that the processing is done from a (D)ESD file
  icf.addFlag("InDet.postProcessing", True) # Turn running of post processing on and off
  icf.addFlag("InDet.doTruth", True) # Turn running of truth matching on and off
  icf.addFlag("InDet.loadTools", True) # Turn loading of tools on and off
  icf.addFlag("InDet.doBackTracking", True) # Turn running of backtracking on and off
  icf.addFlag("InDet.doLowPt",True) # Turn running of doLowPt second pass on and off
  icf.addFlag("InDet.doVeryLowPt", False) # Turn running of doVeryLowPt thrid pass on and off
  icf.addFlag("InDet.doSLHCConversionFinding",True) # Turn running of doSLHCConversionFinding second pass on and off
  icf.addFlag("InDet.doForwardTracks", False) # Turn running of doForwardTracks pass on and off
  icf.addFlag("InDet.doLowPtLargeD0", False) # Turn running of doLargeD0 second pass down to 100 MeV on and off Turn running of doLargeD0 second pass on and off
  icf.addFlag("InDet.doLargeD0", False)
  icf.addFlag("InDet.useExistingTracksAsInput", False) # Use already processed Track from a (D)ESD input file. This flag is related with ProcessedESDTracks InDetKey 
  icf.addFlag("InDet.cutLevel", 19) # Control cuts and settings for different lumi to limit CPU and disk space 
  icf.addFlag("InDet.priVtxCutLevel", 3 ) # Control vertexing cuts and settings for different lumi to limit CPU and disk space 
  icf.addFlag("InDet.doBremRecovery", True) # Turn on running of Brem Recover in tracking
  icf.addFlag("InDet.doCaloSeededBrem", True) # Brem Recover in tracking restricted to Calo ROIs
  icf.addFlag("InDet.doHadCaloSeededSSS", False) # Use Recover SSS to Calo ROIs
  icf.addFlag("InDet.doCaloSeededAmbi", False) # Use Calo ROIs to seed specific cuts for the ambi
  icf.addFlag("InDet.doCaloSeededRefit", False) # Use Calo ROIs to seed refif for the ambi processor
  icf.addFlag("InDet.doBeamGas", False) # Turn running of BeamGas second pass on and off
  icf.addFlag("InDet.doBeamHalo", False) # Turn running of BeamHalo second pass on and off
  icf.addFlag("InDet.doVtxLumi", False) # Turn running of vertex lumi reconstruction on and off
  icf.addFlag("InDet.doVtxBeamSpot", False) # Turn running of vertex BeamSpot reconstruction on and off
  icf.addFlag("InDet.doHeavyIon", False) # Turn running of HeavyIons on and off
  icf.addFlag("InDet.doParticleCreation", True) # Turn running of particle creation on and off
  icf.addFlag("InDet.KeepParameters", True) # Keep extra parameters on slimmed tracks False to drop them
  icf.addFlag("InDet.KeepFirstParameters", False) # Keep the first set of track parameters in addition to the defining ones for TrackParticles. False to drop them
  icf.addFlag("InDet.doTrackSegmentsPixel", False) # Turn running of track segment creation in pixel on and off
  icf.addFlag("InDet.doTrackSegmentsSCT", False) # Turn running of track segment creation in SCT on and off
  icf.addFlag("InDet.doTrackSegmentsTRT", False) # Turn running of track segment creation in TRT on and off
  icf.addFlag("InDet.doMonitoringGlobal", False) # Use to turn on global monitoring 
  icf.addFlag("InDet.doMonitoringPrimaryVertexingEnhanced", False) # Use to turn on enhanced primary vertex monitoring 
  icf.addFlag("InDet.doMonitoringPixel", False) # Use to turn on Pixel monitoring 
  icf.addFlag("InDet.doMonitoringSCT", False) # Use to turn on SCT monitoring 
  icf.addFlag("InDet.doMonitoringTRT", False) # Use to turn on TRT monitoring 
  icf.addFlag("InDet.doMonitoringAlignment", False) # Use to turn on alignment monitoring 
  icf.addFlag("InDet.useDynamicAlignFolders",False) # Deprecated property - use InDetGeometryFlags directly to choose the alignment folder scheme 
  icf.addFlag("InDet.doPerfMon", False) # Use to turn on PerfMon 
  icf.addFlag("InDet.AODall", False) 
  icf.addFlag("InDet.useBeamConstraint", True) # use beam spot service in new tracking 
  icf.addFlag("InDet.kalmanUpdator", 'smatrix') # control which updator to load for KalmanFitter ("None"/"fast"/"smatrix"/"weight"/"amg") 
  icf.addFlag("InDet.magField", 'None') # control which field tool to use ("None"/"fast") 
  icf.addFlag("InDet.propagatorType", 'RungeKutta') # control which propagator to use ('RungeKutta'/'STEP') 
  icf.addFlag("InDet.trackFitterType", 'GlobalChi2Fitter') # control which fitter to be used: 'KalmanFitter', 'KalmanDNAFitter', 'DistributedKalmanFilter', 'GlobalChi2Fitter', 'GaussianSumFilter' 
  icf.addFlag("InDet.doHolesOnTrack", True) # do holes search from now on in summry tool 
  icf.addFlag("InDet.useZvertexTool", False) # start with Zvertex finding 
  icf.addFlag("InDet.doSiSPSeededTrackFinder", False) # use track finding in silicon 
  icf.addFlag("InDet.doTRTExtensionNew", True) # turn on / off TRT extensions 
  icf.addFlag("InDet.trtExtensionType", 'xk') # which extension type ("xk"/"DAF") 
  icf.addFlag("InDet.redoTRT_LR", True) # use smart TRT LR/tube hit creator and redo ROTs 
  icf.addFlag("InDet.doTrtSegments", True) # control to run TRT Segment finding (do it always after new tracking!) 
  icf.addFlag("InDet.doTRTPhaseCalculation", False) # control to run TRT phase calculation 
  icf.addFlag("InDet.doTRTSeededTrackFinder", False) # control running the back tracking 
  icf.addFlag("InDet.loadTRTSeededSPFinder", True) # control which SP finder is used exclusive with loadSimpleTRTSeededSPFinder control which SP finder is used 
  icf.addFlag("InDet.loadSimpleTRTSeededSPFinder", True)
  icf.addFlag("InDet.doResolveBackTracks", False) # control running the ambi on back tracking 
  icf.addFlag("InDet.doTRTStandalone", True) # control TRT Standalone 
  icf.addFlag("InDet.refitROT", True) # control if refit is done from PRD or ROT 
  icf.addFlag("InDet.doSlimming", True) # turn track slimming on/off 
  icf.addFlag("InDet.doSlimPoolTrack", True) # Slimming at converter level rather than creating a slim track collections; requires slimming to be on. 
  icf.addFlag("InDet.doWriteTracksToESD", True) # turn track slimming on/off 
  icf.addFlag("InDet.doVertexFinding", True) # Turn on the primary vertex reconstruction 
  icf.addFlag("InDet.primaryVertexSetup", 'IterativeFinding') # string to store the type of finder/fitter for pri vertexing, possible types: 'AdaptiveMultiFinding', 'IterativeFinding', 'AdaptiveFinding', 'DefaultFastFinding', 'DefaultFullFinding', 'DefaultKalmanFinding', 'DefaultAdaptiveFinding', 'DefaultVKalVrtFinding' 'MedImgMultiFinding' 'GaussIterativeFinding' 'GaussAdaptiveMultiFinding' 
  icf.addFlag("InDet.primaryVertexCutSetup", 'Offline') # string to store the type of cuts to be used in PV reconstruction: 'Offline', 'IBL', 'SLHC' 'HeavyIon' 
  icf.addFlag("InDet.vertexSeedFinder", 'SlidingWindowMultiSeedFinder') # string to store the type of seed finder, possible types: 'SlidingWindowMultiSeedFinder', 'HistogrammingMultiSeedFinder', 'DivisiveMultiSeedFinder' 
  icf.addFlag("InDet.primaryVertexSortingSetup", 'SumPt2Sorting') # string to store the type of sorting algorithm to separate signal and pile-up vertices, possible types: 'NoReSorting','SumPt2Sorting','VxProbSorting','NNSorting' 
  icf.addFlag("InDet.doPrimaryVertex3DFinding", True) # will be set to false automatically if beam constraint ON, otherwise true. Control if to use 3d seeding for primary vertex finding (useful in case of poor / no knowledge of the beam spot. Will be set to false automatically if beam constraint ON, otherwise true
  icf.addFlag("InDet.doVertexFindingForMonitoring", False) # Turn on the primary vertex reconstruction needed to run the enhanced vertex monitoring, this runs the iterative PV with no beam constraint 
  icf.addFlag("InDet.doSplitVertexFindingForMonitoring", False) # will be set to false automatically if beam constraint ON, otherwise true. Turn on the primary vertex reconstruction needed to run the enhanced vertex monitoring, this runs iterative PV in split mode 
  icf.addFlag("InDet.perigeeExpression", 'BeamLine'   ) # Express track parameters wrt. to : 'BeamLine','BeamSpot','Vertex' (first primary vertex) 
  icf.addFlag("InDet.secondaryVertexCutSetup", 'PileUp') # string to store the type of cuts to be used in PV reconstruction: 'StartUp', 'PileUp' 
  icf.addFlag("InDet.conversionVertexCutSetup", 'ConversionPileUp' ) # string to store the type of cuts to be used in conversion reconstruction: 'ConversionStartUp', 'ConversionPileUp' 
  icf.addFlag("InDet.doSharedHits", True) # control if the shared hits are recorded in TrackPatricles 
  icf.addFlag("InDet.doV0Finder", False) # switch on/off V0 finder 
  icf.addFlag("InDet.doSimpleV0Finder", False) # switch on/off simple V0 finder 
  icf.addFlag("InDet.useV0Fitter", False) # use V0 Fitter (alternative is VKalVrt) 
  icf.addFlag("InDet.doSecVertexFinder", False) # switch on/off conversion finder fitting V0s 
  icf.addFlag("InDet.doConversions", False) # switch on/off conversion finder 
  icf.addFlag("InDet.doStatistics", True)
  icf.addFlag("InDet.doPhysValMon", False) # Use to turn on Physics Validation Monitoring 
  icf.addFlag("InDet.materialInteractions", True) 
  icf.addFlag("InDet.materialInteractionsType", 3) # Control which type of particle hypothesis to use for the material interactions 0=non-interacting,1=electron,2=muon,3=pion,4=kaon,5=proton. See ParticleHypothesis.h for full definition.
  icf.addFlag("InDet.doSctClusterNtuple", False)
  icf.addFlag("InDet.doTrkNtuple", False)
  icf.addFlag("InDet.doPixelTrkNtuple", False)
  icf.addFlag("InDet.doSctTrkNtuple", False)
  icf.addFlag("InDet.doTrtTrkNtuple", False)
  icf.addFlag("InDet.doVtxNtuple", False)
  icf.addFlag("InDet.doConvVtxNtuple", False)
  icf.addFlag("InDet.doV0VtxNtuple", False)
  icf.addFlag("InDet.doTrkD3PD", False)
  icf.addFlag("InDet.doPixelTrkD3PD", False)
  icf.addFlag("InDet.doSctTrkD3PD", False)
  icf.addFlag("InDet.doTrtTrkD3PD", False)
  icf.addFlag("InDet.doVtxD3PD", False) # fills the D3PD part of the default primary vertex finder (which is in ESD/AOD or produced during reco, normally WITH beam constraint) 
  icf.addFlag("InDet.doVtxMonitoringD3PD", False) # fills the D3PD parts for the unconstrained PV and the split vtx, works only with iterative finder 
  icf.addFlag("InDet.doConvVtxD3PD", False)
  icf.addFlag("InDet.doV0VtxD3PD", False)
  icf.addFlag("InDet.doTriggerD3PD", False)
  icf.addFlag("InDet.removeTRTNoise", False)
  icf.addFlag("InDet.noTRTTiming", False)
  icf.addFlag("InDet.InDet25nsec", False )
  icf.addFlag("InDet.selectSCTIntimeHits", True) # defines if the X1X mode is used for the offline or not 
  icf.addFlag("InDet.cutSCTOccupancy", True )
  icf.addFlag("InDet.useDCS", True)
  icf.addFlag("InDet.truthMatchStrategy", 'TruthMatchRatio') # defines how truth matching is done. possible values TruthMatchRatio (old style) or TruthMatchTanimoto (new style)
  icf.addFlag("InDet.doFatras", False) # Switch for FATRAS running 
  icf.addFlag("InDet.doSGDeletion", False) # Drop contianers from SG ones finished with them 
  icf.addFlag("InDet.doLowBetaFinder", True) # Switch for running Low-Beta finer 
  icf.addFlag("InDet.useHVForSctDCS", False) # Temporary switch for using 20V HV limit for SCT DCS 
  icf.addFlag("InDet.disableTracking", False) # Disable all tracking algorithms 
  icf.addFlag("InDet.disableInDetReco", False) # Disable all ID reconstruction: pre-processing,tracking, post-processing etc. Still does the configuration: job porperties, cuts, loaign of tools and conditions
  icf.addFlag("InDet.doPixelClusterSplitting", True) # Try to split pixel clusters 
  icf.addFlag("InDet.pixelClusterSplittingType", 'NeuralNet') # choose splitter type: NeuralNet or AnalogClus
  icf.addFlag("InDet.pixelClusterSplitProb1", 0.6) # Cut value for splitting clusters into two parts 
  icf.addFlag("InDet.pixelClusterSplitProb2", 0.2) # Cut value for splitting clusters into three parts 
  icf.addFlag("InDet.pixelClusterSplitProb1_run1", 0.5) # Cut value for splitting clusters into two parts 
  icf.addFlag("InDet.pixelClusterSplitProb2_run1", 0.5) # Cut value for splitting clusters into three parts 
  icf.addFlag("InDet.pixelClusterSplitMinPt", 1000) # Min pt for tracks to try and split hits 
  icf.addFlag("InDet.pixelClusterBadClusterID", 3) # Select the mode to identify suspicous pixel clusteri 
  icf.addFlag("InDet.useBroadClusterErrors", False) # Use broad cluster errors for Pixel/SCT 
  #TODO: useBroadPixClusterErrors and ...SCT... were set to none such that they print a warning if they're accessed without being set. None will be interpreted as False (same behavior as old config) but defaults cannot be None 
  icf.addFlag("InDet.useBroadPixClusterErrors", False) # Use broad cluster errors for Pixel
  icf.addFlag("InDet.useBroadSCTClusterErrors", False) # Use broad cluster errors for SCT
  icf.addFlag("InDet.writeRDOs", False) # Write RDOs into ESD 
  icf.addFlag("InDet.writePRDs", True) # Write PRDs into ESD 
  icf.addFlag("InDet.doMinBias", False) # Switch for running MinBias settings 
  icf.addFlag("InDet.doLowMuRunSetup", False) # Switch for running MinBias runs at low lumi settings 
  icf.addFlag("InDet.doRobustReco", False) # Switch for running Robust settings 
  icf.addFlag("InDet.doSingleCollisionVertexReco", False) # Switch for running veretx reconstruction in single collision mode 
  icf.addFlag("InDet.useMBTSTimeDiff", False ) # Switch for skipping background events based on MBTS time info 
  icf.addFlag("InDet.useNewSiSPSeededTF", False ) # Switch for using new SiSPSeededTrackFinder strategy 
  icf.addFlag("InDet.doxAOD", True ) # Switch for running AOD to xAOD conversion algs 
  icf.addFlag("InDet.doCaloSeededTRTSegments", False ) # Switch for running AOD to xAOD conversion algs 
  icf.addFlag("InDet.doInnerDetectorCommissioning", False) # Switch for running looser settings in ID for commissioning 
  icf.addFlag("InDet.doTIDE_Ambi", True) # Switch for running TIDE Ambi 
  icf.addFlag("InDet.doRefitInvalidCov", False) # Try Kalman fitter if the track fit in the ambiguity processor produces non positive definitematrices.
  icf.addFlag("InDet.doRejectInvalidCov", False) # Reject all tracks which have a non positive definite covariance matrix after the refit.
  icf.addFlag("InDet.doTIDE_RescalePixelCovariances", False) # Switch for running TIDE pixel cluster covariance rescaling 
  icf.addFlag("InDet.doSSSfilter", True) # Switch for running SSS filter
  icf.addFlag("InDet.pT_SSScut", -1) # Pt cut for SSS filter [GeV]
  icf.addFlag("InDet.ForceCoraCool", False) # Use old (non CoolVectorPayload) SCT Conditions 
  icf.addFlag("InDet.ForceCoolVectorPayload", False) # Use new (CoolVectorPayload) SCT Conditions 
  icf.addFlag("InDet.doTrackSegmentsDisappearing", True) # Turn running of track segment creation in pixel after NewTracking, and with PRD association, on and off
  icf.addFlag("InDet.doSLHCVeryForward", False ) # Turn running of SLHC reconstruction for Very Forward extension on and off 
  icf.addFlag("InDet.doTRTGlobalOccupancy", False) # Turn running of Event Info TRT Occupancy Filling Alg on and off (also whether it is used in TRT PID calculation) 
  icf.addFlag("InDet.doNNToTCalibration", False ) # USe ToT calibration for NN clustering rather than Charge 
  icf.addFlag("InDet.keepAdditionalHitsOnTrackParticle", False) # Do not drop first/last hits on track (only for special cases - will blow up TrackParticle szie!!!) 
  icf.addFlag("InDet.doSCTModuleVeto", False) # Turn on SCT_ModuleVetoSvc, allowing it to be configured later 
  icf.addFlag("InDet.doParticleConversion", False) # In case anyone still wants to do Rec->xAOD TrackParticle Conversion 
  icf.addFlag("InDet.doStoreTrackSeeds", False) # Turn on to save the Track Seeds in a xAOD track collecting for development studies 
  icf.addFlag("InDet.doHIP300", False) # Switch for running MinBias settings with a 300 MeV pT cut (for Heavy Ion Proton)
  icf.addFlag("InDet.checkDeadElementsOnTrack", True) # Enable check for dead modules and FEs 
  icf.addFlag("InDet.doDigitalROTCreation",False) # use PixelClusterOnTrackToolDigital during ROT creation to save CPU 
  icf.addFlag("InDet.usePixelDCS",  lambda prevFlags : (prevFlags.InDet.useDCS and prevFlags.Detector.RecoPixel))
  icf.addFlag("InDet.useSctDCS",  lambda prevFlags : (prevFlags.InDet.useDCS and prevFlags.Detector.RecoSCT))

  from InDetConfig.TrackingCutsFlags import createTrackingFlags, createSLHCTrackingFlags, createIBLTrackingFlags, createHighPileupTrackingFlags, createMinBiasTrackingFlags, createLargeD0TrackingFlags, createR3LargeD0TrackingFlags, createLowPtLargeD0TrackingFlags, createLowPtTrackingFlags, createSLHCConversionFindingTrackingFlags, createVeryLowPtTrackingFlags, createForwardTracksTrackingFlags, createForwardSLHCTracksTrackingFlags, createVeryForwardSLHCTracksTrackingFlags, createBeamGasTrackingFlags, createVtxLumiTrackingFlags, createVtxBeamSpotTrackingFlags, createCosmicsTrackingFlags, createHeavyIonTrackingFlags, createPixelTrackingFlags, createDisappearingTrackingFlags, createSCTTrackingFlags, createTRTTrackingFlags, createSCTandTRTTrackingFlags, createDBMTrackingFlags

  icf.addFlagsCategory ("InDet.Tracking", createTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.SLHCTracking", createSLHCTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.IBLTracking", createIBLTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.HighPileupTracking", createHighPileupTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.MinBiasTracking", createMinBiasTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.LargeD0Tracking", createLargeD0TrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.R3LargeD0Tracking", createR3LargeD0TrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.LowPtLargeD0Tracking", createLowPtLargeD0TrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.LowPtTracking", createLowPtTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.SLHCConversionFindingTracking", createSLHCConversionFindingTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.VeryLowPtTracking", createVeryLowPtTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.ForwardTracksTracking", createForwardTracksTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.ForwardSLHCTracksTracking", createForwardSLHCTracksTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.VeryForwardSLHCTracksTracking", createVeryForwardSLHCTracksTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.BeamGasTracking", createBeamGasTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.VtxLumiTracking", createVtxLumiTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.VtxBeamSpotTracking", createVtxBeamSpotTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.CosmicsTracking", createCosmicsTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.HeavyIonTracking", createHeavyIonTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.PixelTracking", createPixelTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.DisappearingTracking", createDisappearingTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.SCTTracking", createSCTTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.TRTTracking", createTRTTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.SCTandTRTTracking", createSCTandTRTTrackingFlags, prefix=True)
  icf.addFlagsCategory ("InDet.DBMTracking", createDBMTrackingFlags, prefix=True)

  return icf
