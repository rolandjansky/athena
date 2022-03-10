# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
# TODO : Add some exta levels?

def createITkConfigFlags():
  itkcf = AthConfigFlags()

  # take geometry XML files from local instance rather than Detector Database, for development
  itkcf.addFlag("ITk.Geometry.AllLocal", False)
  itkcf.addFlag("ITk.Geometry.PixelLocal", lambda prevFlags: prevFlags.ITk.Geometry.AllLocal)
  itkcf.addFlag("ITk.Geometry.PixelFilename", "ITKLayouts/Pixel/ITkPixel.gmx")
  itkcf.addFlag("ITk.Geometry.PixelClobOutputName", "")
  itkcf.addFlag("ITk.Geometry.StripLocal", lambda prevFlags: prevFlags.ITk.Geometry.AllLocal)
  itkcf.addFlag("ITk.Geometry.StripFilename", "ITKLayouts/Strip/ITkStrip.gmx")
  itkcf.addFlag("ITk.Geometry.StripClobOutputName", "")
  itkcf.addFlag("ITk.Geometry.BCMPrimeLocal", lambda prevFlags: prevFlags.ITk.Geometry.AllLocal)
  itkcf.addFlag("ITk.Geometry.BCMPrimeFilename", "ITKLayouts/Pixel/BCMPrime.gmx")
  itkcf.addFlag("ITk.Geometry.BCMPrimeClobOutputName", "")
  itkcf.addFlag("ITk.Geometry.PLRLocal", lambda prevFlags: prevFlags.ITk.Geometry.AllLocal)
  itkcf.addFlag("ITk.Geometry.PLRFilename", "ITKLayouts/PLR/PLR.gmx")
  itkcf.addFlag("ITk.Geometry.PLRClobOutputName", "")
  itkcf.addFlag("ITk.Geometry.DictionaryLocal", lambda prevFlags: prevFlags.ITk.Geometry.AllLocal)
  itkcf.addFlag("ITk.Geometry.DictionaryFilename", "ITKLayouts/IdDictInnerDetector_ITK_LOCAL.xml")
  itkcf.addFlag("ITk.Geometry.isLocal", lambda prevFlags : (prevFlags.ITk.Geometry.PixelLocal
                                                         or prevFlags.ITk.Geometry.StripLocal
                                                         or prevFlags.ITk.Geometry.BCMPrimeLocal
                                                         or prevFlags.ITk.Geometry.PLRLocal))

  itkcf.addFlag("ITk.doStripModuleVeto", False) # Turn on SCT_ModuleVetoSvc, allowing it to be configured later
  itkcf.addFlag("ITk.checkDeadPixelsOnTrack", True) # Enable check for dead modules and FEs

  itkcf.addFlag("ITk.Tracking.doStoreTrackSeeds", False) # Turn on to save the Track Seeds in a xAOD track collecting for development studies
  itkcf.addFlag("ITk.Tracking.doDigitalROTCreation", False) # use PixelClusterOnTrackToolDigital during ROT creation to save CPU
  itkcf.addFlag("ITk.Tracking.trackFitterType", "GlobalChi2Fitter") # control which fitter to be used: 'KalmanFitter', 'KalmanDNAFitter', 'DistributedKalmanFilter', 'GlobalChi2Fitter', 'GaussianSumFilter'
  itkcf.addFlag("ITk.Tracking.doFastTracking", False) # Turn running of ITk FastTracking on and off
  itkcf.addFlag("ITk.Tracking.doConversionFinding",True) # Turn running of ConversionFinding second pass on and off
  itkcf.addFlag("ITk.Tracking.doLargeD0", False)
  itkcf.addFlag("ITk.Tracking.storeSeparateLargeD0Container", False)
  itkcf.addFlag("ITk.Tracking.doBremRecovery", True) # Turn on running of Brem Recover in tracking
  itkcf.addFlag("ITk.Tracking.doCaloSeededBrem", True) # Brem Recover in tracking restricted to Calo ROIs
  itkcf.addFlag("ITk.Tracking.doHadCaloSeededSSS", False) # Use Recover SSS to Calo ROIs
  itkcf.addFlag("ITk.Tracking.doCaloSeededAmbi", lambda prevFlags: prevFlags.Detector.EnableCalo) # Use Calo ROIs to seed specific cuts for the ambi
  itkcf.addFlag("ITk.Tracking.doTruth", lambda f: f.Input.isMC) # Turn running of truth matching on and off (by default on for MC off for data)
  itkcf.addFlag("ITk.Tracking.kalmanUpdator", "smatrix") # control which updator to load for KalmanFitter ("None"/"fast"/"smatrix"/"weight"/"amg
  itkcf.addFlag("ITk.Tracking.doPixelClusterSplitting", True) # Try to split pixel clusters
  itkcf.addFlag("ITk.Tracking.pixelClusterSplittingType", "Truth") # choose splitter type: NeuralNet, AnalogClus or Truth
  itkcf.addFlag("ITk.Tracking.pixelClusterSplitProb1", 0.55) # Cut value for splitting clusters into two parts
  itkcf.addFlag("ITk.Tracking.pixelClusterSplitProb2", 0.45) # Cut value for splitting clusters into three parts
  itkcf.addFlag("ITk.Tracking.perigeeExpression", "BeamLine"   ) # Express track parameters wrt. to : 'BeamLine','BeamSpot','Vertex' (first primary vertex)
  itkcf.addFlag("ITk.Tracking.doSharedHits", True) # control if the shared hits are recorded in TrackParticles
  itkcf.addFlag("ITk.Tracking.materialInteractions", True)
  itkcf.addFlag("ITk.Tracking.writeSeedValNtuple", False) # Turn writing of seed validation ntuple on and off
  
  # config flags for tracking geometry configuration
  from InDetConfig.TrackingGeometryFlags import createITkTrackingGeometryFlags
  itkcf.addFlagsCategory ("ITk.trackingGeometry", createITkTrackingGeometryFlags, prefix=True)

  # config flags for tracking cuts
  from InDetConfig.TrackingPassFlags import createITkTrackingPassFlags, createITkLargeD0TrackingPassFlags, createITkConversionFindingTrackingPassFlags, createITkFastTrackingPassFlags, createITkLargeD0FastTrackingPassFlags

  itkcf.addFlagsCategory ("ITk.Tracking.ActivePass", createITkTrackingPassFlags, prefix=True) # Set up for first tracking pass, updated for second passes
  itkcf.addFlagsCategory ("ITk.Tracking.LargeD0Pass", createITkLargeD0TrackingPassFlags, prefix=True)
  itkcf.addFlagsCategory ("ITk.Tracking.ConversionFindingPass", createITkConversionFindingTrackingPassFlags, prefix=True)
  itkcf.addFlagsCategory ("ITk.Tracking.FastPass", createITkFastTrackingPassFlags, prefix=True)
  itkcf.addFlagsCategory ("ITk.Tracking.LargeD0FastPass", createITkLargeD0FastTrackingPassFlags, prefix=True)

  from InDetConfig.VertexFindingFlags import createITkPriVertexingFlags
  itkcf.addFlagsCategory("ITk.PriVertex", createITkPriVertexingFlags, prefix=True)


  return itkcf
