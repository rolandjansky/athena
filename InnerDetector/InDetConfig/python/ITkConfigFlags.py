# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
# TODO : Add some exta levels?

def createITkConfigFlags():
  itkcf = AthConfigFlags()

  # take geometry XML files from local instance rather than Detector Database, for development
  itkcf.addFlag("ITk.pixelGeometryFilename", "ITKLayouts/Pixel/ITkPixel.gmx")
  itkcf.addFlag("ITk.stripGeometryFilename", "ITKLayouts/Strip/ITkStrip.gmx")

  itkcf.addFlag("ITk.doPrintConfigurables",False) # if this is on all the print(ITkXYZ) lines are activated
  itkcf.addFlag("ITk.doSplitReco", False    	 ) # Turn running of the truth seeded pseudo tracking only for pileup on and off. Only makes sense to run on RDO file where SplitDigi was used!
  itkcf.addFlag("ITk.doPRDFormation", True) # Turn running of PRD formation on and off
  itkcf.addFlag("ITk.doPixelPRDFormation", True) # Turn running of pixel PRD formation on and off
  itkcf.addFlag("ITk.doStripPRDFormation", True) # Turn running of Strip PRD formation on and off
  itkcf.addFlag("ITk.doSpacePointFormation", True) # Turn running of space point formation on and off
  itkcf.addFlag("ITk.doRefit", False) # Turn running of refitting on and off
  itkcf.addFlag("ITk.doTIDE_Ambi", True) # Switch for running TIDE Ambi 
  itkcf.addFlag("ITk.doRefitInvalidCov", False) # Try Kalman fitter if the track fit in the ambiguity processor produces non positive definitematrices.
  itkcf.addFlag("ITk.doSSSfilter", True) # Switch for running SSS filter
  itkcf.addFlag("ITk.doNNToTCalibration", False ) # USe ToT calibration for NN clustering rather than Charge 
  itkcf.addFlag("ITk.useNNTTrainedNetworks", True ) # Use older NNs stored as TTrainedNetworks in place of default MDNs/other more recent networks. This is necessary for older configuration tags where the trainings were not available.
  itkcf.addFlag("ITk.doStripModuleVeto", False) # Turn on SCT_ModuleVetoSvc, allowing it to be configured later
  itkcf.addFlag("ITk.doStoreTrackSeeds", False) # Turn on to save the Track Seeds in a xAOD track collecting for development studies 
  itkcf.addFlag("ITk.checkDeadElementsOnTrack", True) # Enable check for dead modules and FEs   
  itkcf.addFlag("ITk.doDigitalROTCreation",False) # use PixelClusterOnTrackToolDigital during ROT creation to save CPU 
  itkcf.addFlag("ITk.nnCutLargeD0Threshold", -1.0) # Enable check for dead modules and FEs
  itkcf.addFlag("ITk.propagatorType", 'RungeKutta') # control which propagator to use ('RungeKutta'/'STEP') 
  itkcf.addFlag("ITk.trackFitterType", 'GlobalChi2Fitter') # control which fitter to be used: 'KalmanFitter', 'KalmanDNAFitter', 'DistributedKalmanFilter', 'GlobalChi2Fitter', 'GaussianSumFilter' 
  itkcf.addFlag("ITk.useHolesFromPattern", False) 
  itkcf.addFlag("ITk.useZvertexTool", False) # start with Zvertex finding 
  itkcf.addFlag("ITk.doSiSPSeededTrackFinder", False) # use track finding in silicon 
  itkcf.addFlag("ITk.doBeamHalo", False) # Turn running of BeamHalo second pass on and off
  itkcf.addFlag("ITk.doHeavyIon", False) # Turn running of HeavyIons on and off
  itkcf.addFlag("ITk.doFastTracking", False) # Turn running of ITk FastTracking on and off
  itkcf.addFlag("ITk.doITkConversionFinding",True) # Turn running of doITkConversionFinding second pass on and off
  itkcf.addFlag("ITk.doITkLargeD0", False)
  itkcf.addFlag("ITk.doBremRecovery", True) # Turn on running of Brem Recover in tracking
  itkcf.addFlag("ITk.doCaloSeededBrem", True) # Brem Recover in tracking restricted to Calo ROIs
  itkcf.addFlag("ITk.doHadCaloSeededSSS", False) # Use Recover SSS to Calo ROIs
  itkcf.addFlag("ITk.doCaloSeededAmbi", False) # Use Calo ROIs to seed specific cuts for the ambi
  itkcf.addFlag("ITk.doTruth", lambda f: f.Input.isMC) # Turn running of truth matching on and off (by default on for MC off for data)
  itkcf.addFlag("ITk.loadRotCreator", True) # Turn loading of ROT_Creator tool on and off
  itkcf.addFlag("ITk.useBeamConstraint", True) # use beam spot service in new tracking 
  itkcf.addFlag("ITk.kalmanUpdator", 'smatrix') # control which updator to load for KalmanFitter ("None"/"fast"/"smatrix"/"weight"/"amg
  itkcf.addFlag("ITk.doPixelClusterSplitting", True) # Try to split pixel clusters
  itkcf.addFlag("ITk.pixelClusterSplittingType", 'NeuralNet') # choose splitter type: NeuralNet or AnalogClus
  itkcf.addFlag("ITk.pixelClusterSplitProb1", 0.55) # Cut value for splitting clusters into two parts 
  itkcf.addFlag("ITk.pixelClusterSplitProb2", 0.45) # Cut value for splitting clusters into three parts 
  itkcf.addFlag("ITk.pixelClusterSplitMinPt", 1000) # Min pt for tracks to try and split hits
  itkcf.addFlag("ITk.perigeeExpression", 'BeamLine'   ) # Express track parameters wrt. to : 'BeamLine','BeamSpot','Vertex' (first primary vertex)
  itkcf.addFlag("ITk.doSharedHits", True) # control if the shared hits are recorded in TrackPatricles 
  itkcf.addFlag("ITk.materialInteractions", True) 
  itkcf.addFlag("ITk.materialInteractionsType", 3) # Control which type of particle hypothesis to use for the material interactions 0=non-interacting,1=electron,2=muon,3=pion,4=kaon,5=proton. See ParticleHypothesis.h for full definition.
  itkcf.addFlag("ITk.pixelClusterBadClusterID", 3) # Select the mode to identify suspicous pixel clusteri 
  itkcf.addFlag("ITk.KeepFirstParameters", False) # Keep the first set of track parameters in addition to the defining ones for TrackParticles. False to drop them
  itkcf.addFlag("ITk.doVertexFinding", False) # Turn on the primary vertex reconstruction, temporarily disabled
  itkcf.addFlag("ITk.useBroadClusterErrors", False) # Use broad cluster errors for Pixel/Strip
  itkcf.addFlag("ITk.useBroadPixClusterErrors", False) # Use broad cluster errors for Pixel
  itkcf.addFlag("ITk.useBroadStripClusterErrors", False) # Use broad cluster errors for Strip
  itkcf.addFlag("ITk.doMinBias", False) # Switch for running MinBias settings 
  itkcf.addFlag("ITk.doRobustReco", False) # Switch for running Robust settings
  itkcf.addFlag("ITk.useNewSiSPSeededTF", False ) # Switch for using new SiSPSeededTrackFinder strategy

  from InDetConfig.TrackingCutsFlags import createITkTrackingFlags, createITkLargeD0TrackingFlags, createITkConversionFindingTrackingFlags, createITkFastTrackingFlags, createITkLargeD0FastTrackingFlags

  itkcf.addFlagsCategory ("ITk.Tracking", createITkTrackingFlags, prefix=True)
  itkcf.addFlagsCategory ("ITk.LargeD0Tracking", createITkLargeD0TrackingFlags, prefix=True)
  itkcf.addFlagsCategory ("ITk.ConversionFindingTracking", createITkConversionFindingTrackingFlags, prefix=True)
  itkcf.addFlagsCategory ("ITk.FastTracking", createITkFastTrackingFlags, prefix=True)
  itkcf.addFlagsCategory ("ITk.LargeD0FastTracking", createITkLargeD0FastTrackingFlags, prefix=True)

  return itkcf
