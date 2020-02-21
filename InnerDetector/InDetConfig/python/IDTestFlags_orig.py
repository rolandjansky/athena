def IDTestFlags_q221() :
    # from AthenaConfiguration.AthConfigFlags import AthConfigFlags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    acf = ConfigFlags
    acf.addFlag('CommonGeometryFlags.GeoType', 'UNDEFINED')
    acf.addFlag('CommonGeometryFlags.Run', 'RUN2')
    acf.addFlag('DQMonFlags.disableFilledBunchFilter', False)
    acf.addFlag('DQMonFlags.monManDataType', 'monteCarlo')
    acf.addFlag('DQMonFlags.monManEnvironment', 'tier0Raw')
    acf.addFlag('DQMonFlags.monManFileKey', 'CombinedMonitoring')
    acf.addFlag('DQMonFlags.monManLumiBlock', 1)
    acf.addFlag('DQMonFlags.monManManualDataTypeSetup', True)
    acf.addFlag('DQMonFlags.monManManualRunLBSetup', False)
    acf.addFlag('DQMonFlags.monManRun', 1)
    acf.addFlag('DetFlags.SCT_on', True)
    acf.addFlag('DetFlags.detdescr.Calo_allOn', True)
    acf.addFlag('DetFlags.haveRIO.Calo_on', True)
    acf.addFlag('DetFlags.haveRIO.ID_on', True)
    acf.addFlag('DetFlags.haveRIO.SCT_on', True)
    acf.addFlag('DetFlags.haveRIO.TRT_on', True)
    acf.addFlag('DetFlags.haveRIO.pixel_on', True)
    acf.addFlag('DetFlags.makeRIO.BCM_on', True)
    acf.addFlag('DetFlags.makeRIO.SCT_on', True)
    acf.addFlag('DetFlags.makeRIO.TRT_on', True)
    acf.addFlag('DetFlags.makeRIO.pixel_on', True)
    acf.addFlag('DetFlags.overlay.SCT_on', False)
    acf.addFlag('DetFlags.overlay.TRT_on', False)
    acf.addFlag('DetFlags.overlay.pixel_on', False)
    acf.addFlag('DetFlags.pixel_on', True)
    acf.addFlag('DetFlags.simulate.SCT_on', False)
    acf.addFlag('DetFlags.simulate.any_on', False)
    acf.addFlag('DetFlags.simulate.pixel_on', False)
    acf.addFlag('DetFlags.detdescr.pixel_on', True)
    acf.addFlag('DetFlags.detdescr.SCT_on', True)
    acf.addFlag('DetFlags.detdescr.TRT_on', True)
    acf.addFlag('InDetFlags.ForceCoolVectorPayload', False)  ##++++ ##111
    acf.addFlag('InDetFlags.ForceCoraCool', False)           ##++++ ##111
    acf.addFlag('InDetFlags.InDet25nsec', True)              ##++++ ##111
    acf.addFlag('InDetFlags.KeepFirstParameters', False)     ##++++ ##111
    acf.addFlag('InDetFlags.KeepParameters', True)           ##++++ ##111
    acf.addFlag('InDetFlags.checkDeadElementsOnTrack', True) ##++++   ##111
    acf.addFlag('InDetFlags.conversionVertexCutSetup', 'ConversionPileUp')  ##++++   ###111
    acf.addFlag('InDetFlags.cutSCTOccupancy', True)          ##++++ ##111
    acf.addFlag('InDetFlags.doAmbiSolving', True)            ##  no
    acf.addFlag('InDetFlags.doBackTracking', True)           ##+++  ##111
    acf.addFlag('InDetFlags.doBeamGas', False)               ##+++  ##111
    acf.addFlag('InDetFlags.doBeamHalo', False)              ##+++  ##111
    acf.addFlag('InDetFlags.doBremRecovery', True)           ##+++  ##111
    acf.addFlag('InDetFlags.doCaloSeededBrem', True)         ##+++  ##111
    acf.addFlag('InDetFlags.doCaloSeededTRTSegments', False) ##+++  ##111
    acf.addFlag('InDetFlags.doConvVtxNtuple', False)         ##+++  ##111
    acf.addFlag('InDetFlags.doConversions', False)           ##+++  ##111
    acf.addFlag('InDetFlags.doCosmics', False)               ##+++  ##111
    acf.addFlag('InDetFlags.doDBM', False)                   ##+++  ##111
    acf.addFlag('InDetFlags.doDBMstandalone', False)         ##+++  ##111
    acf.addFlag('InDetFlags.doDVRetracking', False)          ##+++  ##111
    acf.addFlag('InDetFlags.doExtensionProcessor', True)     ## no
    acf.addFlag('InDetFlags.doFatras', False)                ##+++  ##111
    acf.addFlag('InDetFlags.doForwardTracks', True)          ##+++  ##111
    acf.addFlag('InDetFlags.doHadCaloSeededSSS', False)      ##+++  ##111
    acf.addFlag('InDetFlags.doHeavyIon', False)              ##+++  ##111
    acf.addFlag('InDetFlags.doHighPileup', False)            ##+++  ##111
    acf.addFlag('InDetFlags.doLargeD0', False)               ##+++  ##111
    acf.addFlag('InDetFlags.doLowBetaFinder', True)          ##+++  ##111
    acf.addFlag('InDetFlags.doLowPt', False)                 ##+++  ##111
    acf.addFlag('InDetFlags.doLowPtLargeD0', False)          ##+++  ##111
    acf.addFlag('InDetFlags.doMinBias', False)               ##+++  ##111
    acf.addFlag('InDetFlags.doMonitoringGlobal', False)      ##---  ##111
    acf.addFlag('InDetFlags.doMonitoringPixel', False)       ##---  ##111
    acf.addFlag('InDetFlags.doMonitoringSCT', False)         ##---  ##111
    acf.addFlag('InDetFlags.doMonitoringTRT', False)         ##---  ##111
    acf.addFlag('InDetFlags.doNNToTCalibration', False)      ##+++  ##111
    acf.addFlag('InDetFlags.doNewTracking', True)            ##+++   ##111
    acf.addFlag('InDetFlags.doNewTrackingPattern', True)     ##no
    acf.addFlag('InDetFlags.doNtupleCreation', False)        ##no
    acf.addFlag('InDetFlags.doPRDFormation', True)           ##+++   ##111
    acf.addFlag('InDetFlags.doParticleConversion', False)    ##+++   ##111
    acf.addFlag('InDetFlags.doParticleCreation', True)       ##+++   ##111
    acf.addFlag('InDetFlags.doPattern', True)                ##no
    acf.addFlag('InDetFlags.doPhysValMon', False)            ##+++  ##111
    acf.addFlag('InDetFlags.doPixelClusterSplitting', True)  ##+++  ##111
    acf.addFlag('InDetFlags.doPixelPRDFormation', True)      ##+++  ##111
    acf.addFlag('InDetFlags.doPixelTrkNtuple', False)        ##+++  ##111
    acf.addFlag('InDetFlags.doPseudoTracking', False)        ##+++  ###111
    acf.addFlag('InDetFlags.doRefit', False)                 ##+++  ###111 true
    acf.addFlag('InDetFlags.doRefitInvalidCov', False)       ##+++  ##111
    acf.addFlag('InDetFlags.doRejectInvalidCov', False)      ##+++  ##111
    acf.addFlag('InDetFlags.doResolveBackTracks', True)      ##+++  ##1111
    acf.addFlag('InDetFlags.doRobustReco', False)            ##+++  ###111
    acf.addFlag('InDetFlags.doSCTModuleVeto', False)         ##+++  ##111
    acf.addFlag('InDetFlags.doSCT_PRDFormation', True)       ##+++  ###111
    acf.addFlag('InDetFlags.doSGDeletion', False)            ##+++  ##111
    acf.addFlag('InDetFlags.doSLHC', False)                  ##+++  ##111
    acf.addFlag('InDetFlags.doSLHCConversionFinding', False) ##+++  ##111
    acf.addFlag('InDetFlags.doSLHCVeryForward', False)       ##+++  ##111
    acf.addFlag('InDetFlags.doSSSfilter', True)              ##+++  ##111
    acf.addFlag('InDetFlags.doSctClusterNtuple', False)      ##+++  ##111
    acf.addFlag('InDetFlags.doSctTrkNtuple', False)          ##+++  ##111
    acf.addFlag('InDetFlags.doSecVertexFinder', False)       ##+++  ##111
    acf.addFlag('InDetFlags.doSharedHits', True)             ##+++  ##111
    acf.addFlag('InDetFlags.doSiSPSeededTrackFinder', True)  ##+++  ##111
    acf.addFlag('InDetFlags.doSimpleV0Finder', False)        ##+++  ##111
    acf.addFlag('InDetFlags.doSlimPoolTrack', True)          ##+++  ##111
    acf.addFlag('InDetFlags.doSlimming', True)               ##+++  ##111
    acf.addFlag('InDetFlags.doSpacePointFormation', True)    ##+++  ###111
    acf.addFlag('InDetFlags.doSplitReco', False)             ##+++  ###111
    ###acf.addFlag('InDetFlags.doSplitReco', True)
    acf.addFlag('InDetFlags.doSplitVertexFindingForMonitoring', False)  ##--- ##111
    acf.addFlag('InDetFlags.doStandardPlots', False)         ##+++  ##111
    acf.addFlag('InDetFlags.doStatistics', True)             ##+++  ##111
    acf.addFlag('InDetFlags.doStoreTrackSeeds', False)       ##+++  ##111
    acf.addFlag('InDetFlags.doTIDE_Ambi', True)              ##+++  ##111
    acf.addFlag('InDetFlags.doTIDE_AmbiTrackMonitoring', False)   ## no
    acf.addFlag('InDetFlags.doTIDE_RescalePixelCovariances', False) ##+++ ##111
    acf.addFlag('InDetFlags.doTRTExtension', True)           ## no
    acf.addFlag('InDetFlags.doTRTExtensionNew', True)        ##+++  ##111
    acf.addFlag('InDetFlags.doTRTGlobalOccupancy', False)    ##+++ ##111
    acf.addFlag('InDetFlags.doTRTPhaseCalculation', True)    ##---  ##111
    acf.addFlag('InDetFlags.doTRTSeededTrackFinder', True)   ##+++   ##111
    acf.addFlag('InDetFlags.doTRTStandalone', True)          ##+++   ##111
    acf.addFlag('InDetFlags.doTRT_PRDFormation', True)       ##+++   ##111
    acf.addFlag('InDetFlags.doTrackSegmentsPixel', False)    ##+++   ##111
    acf.addFlag('InDetFlags.doTrackSegmentsDisappearing', True)  ##+++  ##111
    acf.addFlag('InDetFlags.doTrackSegmentsSCT', False)      ##+++   ##111
    acf.addFlag('InDetFlags.doTrackSegmentsTRT', False)      ##+++   ##111
    acf.addFlag('InDetFlags.doTrkNtuple', False)             ##+++   ##111
    acf.addFlag('InDetFlags.doTrtSegments', True)            ##+++   ##111
    acf.addFlag('InDetFlags.doTrtTrkNtuple', False)          ##+++   ##111
    acf.addFlag('InDetFlags.doTruth', True)                  ##+++  ##111
    acf.addFlag('InDetFlags.doV0Finder', False)              ##+++  ##111
    acf.addFlag('InDetFlags.doV0VtxNtuple', False)           ##+++  ##111
    acf.addFlag('InDetFlags.doVertexFinding', True)          ##+++  ##111
    acf.addFlag('InDetFlags.doVertexFindingForMonitoring', False)  ##---  ##111
    acf.addFlag('InDetFlags.doVeryLowPt', False)             ##+++ ##111
    acf.addFlag('InDetFlags.doVtxNtuple', False)             ##+++ ##111
    acf.addFlag('InDetFlags.doiPatRec', False)               ##+++ ##111
    acf.addFlag('InDetFlags.doxAOD', True)                   ##+++ ##111
    acf.addFlag('InDetFlags.doxKalman', False)               ##+++ ##111
    acf.addFlag('InDetFlags.kalmanUpdator', 'smatrix')       ##+++ ##111
    acf.addFlag('InDetFlags.loadRotCreator', True)                ## no           
    acf.addFlag('InDetFlags.loadSimpleTRTSeededSPFinder', False)  ##+++  ##111
    acf.addFlag('InDetFlags.loadTRTSeededSPFinder', True)    ##+++ ##111
    acf.addFlag('InDetFlags.materialInteractions', True)     ##+++ ##111
    acf.addFlag('InDetFlags.materialInteractionsType', '3')  ##+++ ##111
    acf.addFlag('InDetFlags.noTRTTiming', False)             ##+++ ##111
    acf.addFlag('InDetFlags.pT_SSScut', '-1')                ##+++ ##111
    acf.addFlag('InDetFlags.perigeeExpression', 'BeamLine')  ##+++  ##111
    acf.addFlag('InDetFlags.pixelClusterBadClusterID', '3')  ##+++  ###111
    acf.addFlag('InDetFlags.pixelClusterSplitMinPt', '1000') ##+++  ##111
    acf.addFlag('InDetFlags.pixelClusterSplitProb1', '0.6')  ##+++  ###111
    acf.addFlag('InDetFlags.pixelClusterSplitProb1_run1', '0.5') ##+++  ###111
    acf.addFlag('InDetFlags.pixelClusterSplitProb2', '0.2')  ##+++  ##111
    acf.addFlag('InDetFlags.pixelClusterSplitProb2_run1', '0.5') ##+++
    acf.addFlag('InDetFlags.pixelClusterSplittingType', 'NeuralNet') ##+++   ###111
    acf.addFlag('InDetFlags.propagatorType', 'RungeKutta')   ##+++     ##111
    acf.addFlag('InDetFlags.redoTRT_LR', True)               ##+++     ##111
    acf.addFlag('InDetFlags.refitROT', True)                 ##+++    ##111
    acf.addFlag('InDetFlags.removeTRTNoise', False)          ##+++    ##111
    acf.addFlag('InDetFlags.secondaryVertexCutSetup', 'PileUp') ##+++ ##111
    acf.addFlag('InDetFlags.selectSCTIntimeHits', True)      ##+++    ##111
    acf.addFlag('InDetFlags.solenoidOn', True)               ##  no
    acf.addFlag('InDetFlags.trackFitterType', 'GlobalChi2Fitter')  ##+++ ##1111
    acf.addFlag('InDetFlags.trtExtensionType', 'xk')         ##+++       ##111
    acf.addFlag('InDetFlags.truthMatchStrategy', 'TruthMatchRatio')  ##+++ ##111
    acf.addFlag('InDetFlags.useBeamConstraint', True)        ##+++   ##111
    acf.addFlag('InDetFlags.useBroadClusterErrors', False)   ##+++   ##111
    acf.addFlag('InDetFlags.useBroadPixClusterErrors', 'None') ##+++ ##111
    acf.addFlag('InDetFlags.useBroadSCTClusterErrors', 'None') ##+++ ##111
    acf.addFlag('InDetFlags.useDynamicAlignFolders', False)  ##+++ ##1111
    acf.addFlag('InDetFlags.useExistingTracksAsInput', False) ##+++   ##111
    acf.addFlag('InDetFlags.useHVForSctDCS', False)          ##+++    ##111
    acf.addFlag('InDetFlags.useMBTSTimeDiff', False)         ##+++    ##111
    acf.addFlag('InDetFlags.useNewSiSPSeededTF', False)      ##+++    ##111
    acf.addFlag('InDetFlags.usePixelDCS', True)              ##  no
    acf.addFlag('InDetFlags.useSctDCS', True)                ##  no
    acf.addFlag('InDetFlags.useTrtDCS', True)                ##  no
    acf.addFlag('InDetFlags.useV0Fitter', False)             ##+++  ##111
    acf.addFlag('InDetFlags.useZvertexTool', False)          ##+++    ##111
    acf.addFlag('InDetGeometryFlags.useDynamicAlignFolders', False)   ##+++
    acf.addFlag('InDetKeys.AliasToTracks', 'none')           ##+++
    acf.addFlag('InDetKeys.BCM_RDOs', 'BCM_RDOs')            ##+++
    acf.addFlag('InDetKeys.CaloCellContainer', 'AllCalo')     ##+++
    acf.addFlag('InDetKeys.CaloClusterContainer', 'LArClusterEM') ##+++
    acf.addFlag('InDetKeys.CaloClusterROIContainer', 'InDetCaloClusterROIs') ##+++
    acf.addFlag('InDetKeys.CombinedCosmicTracks', 'CombinedCosmicTracks') ##+++
    acf.addFlag('InDetKeys.Conversions', 'ConversionCandidate') ##+++
    acf.addFlag('InDetKeys.DBMDetailedTracksTruth', 'DBMTracksDetailedTruth') ##+++
    acf.addFlag('InDetKeys.DBMTracks', 'ResolvedDBMTracks')   ##+++
    acf.addFlag('InDetKeys.DBMTracksTruth', 'DBMTracksTruthCollection') ##+++
    acf.addFlag('InDetKeys.DetailedTracksTruth', 'DetailedTrackTruth') ##+++
    acf.addFlag('InDetKeys.ExtendedBeamGasTracks', 'ExtendedBeamGasTracks')  ##+++
    acf.addFlag('InDetKeys.ExtendedLargeD0Tracks', 'ExtendedLargeD0Tracks') ##+++
    acf.addFlag('InDetKeys.ExtendedLowPtTracks', 'ExtendedLowPtTracks') ##+++
    acf.addFlag('InDetKeys.ExtendedSLHCTracks', 'ExtendedSLHCTracks')  ##+++
    acf.addFlag('InDetKeys.ExtendedTracks', 'ExtendedTracks') ##+++
    acf.addFlag('InDetKeys.ExtendedTracksMap', 'ExtendedTracksMap') ##+++
    acf.addFlag('InDetKeys.ExtendedTracksMapBeamGas', 'ExtendedTracksMapBeamGas') ##+++
    acf.addFlag('InDetKeys.ExtendedTracksMapLargeD0', 'ExtendedTracksMapLargeD0') ##+++
    acf.addFlag('InDetKeys.ExtendedTracksMapLowPt', 'ExtendedTracksMapLowPt')   ##+++
    acf.addFlag('InDetKeys.ExtendedTracksMapPhase', 'ExtendedTracksMapPhase')   ##+++
    acf.addFlag('InDetKeys.ExtendedTracksMapDisappearing', 'ExtendedTracksMapDisappearing') ##+++
    acf.addFlag('InDetKeys.ExtendedTracksMapSLHC', 'ExtendedTracksMapSLHC') ##+++
    acf.addFlag('InDetKeys.ExtendedTracksPhase', 'ExtendedTracksPhase')  ##+++
    acf.addFlag('InDetKeys.ExtendedTracksDisappearing', 'ExtendedTracksDisappearing') ##+++
    acf.addFlag('InDetKeys.GangedPixelMap', 'PixelClusterAmbiguitiesMap')  ##+++
    acf.addFlag('InDetKeys.HadCaloCellContainer', 'AllCalo')   ##+++
    acf.addFlag('InDetKeys.HadCaloClusterContainer', 'CaloCalTopoCluster')  ##+++
    acf.addFlag('InDetKeys.HadCaloClusterROIContainer', 'InDetHadCaloClusterROIs') ##+++
    acf.addFlag('InDetKeys.IPatConvertedTracks', 'ConvertedIPatTracks')   ##+++
    acf.addFlag('InDetKeys.LowBetaCandidates', 'InDetLowBetaCandidates')##+++  
    acf.addFlag('InDetKeys.McEventCollection', 'TruthEvent') ##+++
    acf.addFlag('InDetKeys.OverlapSpacePoints', 'OverlapSpacePoints')##+++
    acf.addFlag('InDetKeys.PixelClusters', 'PixelClusters') ##+++
    acf.addFlag('InDetKeys.PixelClustersTruth', 'PRD_MultiTruthPixel') ##+++
    acf.addFlag('InDetKeys.PixelDetailedTracksTruth', 'ResolvedPixelTrackDetailedTruth') ##+++
    acf.addFlag('InDetKeys.PixelManager', 'Pixel') ##+++
    acf.addFlag('InDetKeys.PixelPUClusters', 'PixelPUClusters')##+++
    acf.addFlag('InDetKeys.PixelPURDOs', 'Pixel_PU_RDOs') ##+++
    acf.addFlag('InDetKeys.PixelPUSDOs', 'Pixel_PU_SDO_Map') ##+++
    acf.addFlag('InDetKeys.DisappearingDetailedTracksTruth', 'DisappearingTracksDetailedTruth')  ##+++
    acf.addFlag('InDetKeys.DisappearingTracks', 'DisappearingTracks')   ##+++
    acf.addFlag('InDetKeys.DisappearingTracksTruth', 'DisappearingTracksTruthCollection') ##+++
    acf.addFlag('InDetKeys.PixelRDOs', 'PixelRDOs')  ##+++
    acf.addFlag('InDetKeys.PixelSDOs', 'PixelSDO_Map') ##+++
    acf.addFlag('InDetKeys.PixelSpacePoints', 'PixelSpacePoints') ##+++
    acf.addFlag('InDetKeys.PixelTracks', 'ResolvedPixelTracks') ##+++
    acf.addFlag('InDetKeys.PixelTracksTruth', 'ResolvedPixelTrackTruthCollection') ##+++
    acf.addFlag('InDetKeys.PrimaryVertices', 'VxPrimaryCandidate') ##+++
    acf.addFlag('InDetKeys.ProcessedESDTracks', 'Tracks') ##+++
    acf.addFlag('InDetKeys.PseudoTracks', 'InDetPseudoTracks') ##+++
    acf.addFlag('InDetKeys.PseudoTracksTruth', 'InDetPseudoTrackTruthCollection') ##+++
    acf.addFlag('InDetKeys.RefittedDetailedTracksTruth', 'RefittedTracksDetailedTruth') ##+++
    acf.addFlag('InDetKeys.RefittedTracks', 'RefittedTracks') ##+++
    acf.addFlag('InDetKeys.RefittedTracksTruth', 'RefittedTracksTruthCollection') ##+++
    acf.addFlag('InDetKeys.ResolvedBeamGasTracks', 'ResolvedBeamGasTracks') ##+++
    acf.addFlag('InDetKeys.ResolvedForwardTrackParticles', 'ResolvedForwardTrackParticles') ##+++
    acf.addFlag('InDetKeys.ResolvedForwardTrackParticlesTruth', 'ResolvedForwardTrackParticlesTruthCollection') ##+++
    acf.addFlag('InDetKeys.ResolvedForwardTracks', 'ResolvedForwardTracks')##+++
    acf.addFlag('InDetKeys.ResolvedForwardTracksTruth', 'ResolvedForwardTracksTruthCollection') ##+++
    acf.addFlag('InDetKeys.ResolvedLargeD0Tracks', 'ResolvedLargeD0Tracks') ##+++
    acf.addFlag('InDetKeys.ResolvedLowPtTracks', 'ResolvedLowPtTracks') ##+++
    acf.addFlag('InDetKeys.ResolvedDisappearingTracks', 'ResolvedDisappearingTracks') ##+++
    acf.addFlag('InDetKeys.ResolvedSLHCConversionFindingTracks', 'ResolvedSLHCConversionFindingTracks') ##+++
    acf.addFlag('InDetKeys.ResolvedTRTSeededTracks', 'ResolvedTRTSeededTracks') ##+++
    acf.addFlag('InDetKeys.ResolvedTracks', 'ResolvedTracks')  ##no
    acf.addFlag('InDetKeys.ResolvedVeryLowPtTracks', 'ResolvedVeryLowPtTracks') ##no
    acf.addFlag('InDetKeys.SCTTracks', 'ResolvedSCTTracks') ##+++
    acf.addFlag('InDetKeys.SCT_Clusters', 'SCT_Clusters') ##+++
    acf.addFlag('InDetKeys.SCT_ClustersTruth', 'PRD_MultiTruthSCT') ##+++
    acf.addFlag('InDetKeys.SCT_Manager', 'SCT') ##+++
    acf.addFlag('InDetKeys.SCT_PU_Clusters', 'SCT_PU_Clusters')##+++
    acf.addFlag('InDetKeys.SCT_PU_ClustersTruth', 'PRD_PU_MultiTruthSCT') ##+++
    acf.addFlag('InDetKeys.SCT_PU_SDOs', 'SCT_PU_SDO_Map') ##+++
    acf.addFlag('InDetKeys.SCT_RDOs', 'SCT_RDOs') ##+++
    acf.addFlag('InDetKeys.SCT_SDOs', 'SCT_SDO_Map') ##+++
    acf.addFlag('InDetKeys.SCT_SpacePoints', 'SCT_SpacePoints') ##+++
    acf.addFlag('InDetKeys.SecVertices', 'SecVertices')  ###+++
    acf.addFlag('InDetKeys.SiSPSeedSegments', 'SiSPSeedSegments') ##+++
    acf.addFlag('InDetKeys.SiSpSeededBeamGasTracks', 'SiSPSeededBeamGasTracks') ##+++
    acf.addFlag('InDetKeys.SiSpSeededDBMTracks', 'SiSPSeededDBMTracks') ##+++
    acf.addFlag('InDetKeys.SiSpSeededForwardTracks', 'SiSpSeededForwardTracks') ##+++
    acf.addFlag('InDetKeys.SiSpSeededLargeD0Tracks', 'SiSpSeededLargeD0Tracks')  ##+++
    acf.addFlag('InDetKeys.SiSpSeededLowPtTracks', 'SiSPSeededLowPtTracks') ##+++
    acf.addFlag('InDetKeys.SiSpSeededPixelTracks', 'SiSPSeededPixelTracks') ##+++
    acf.addFlag('InDetKeys.SiSpSeededSCTTracks', 'SiSPSeededSCTTracks') ##+++
    acf.addFlag('InDetKeys.SiSpSeededSLHCConversionFindingTracks', 'SiSpSeededSLHCConversionFindingTracks') ##+++
    acf.addFlag('InDetKeys.SiSpSeededSLHCTracks', 'SiSPSeededSLHCTracks') ##+++
    acf.addFlag('InDetKeys.SiSpSeededTracks', 'SiSPSeededTracks')##+++
    acf.addFlag('InDetKeys.SiSpSeededVeryLowPtTracks', 'SiSPSeededVeryLowPtTracks') ##+++
    acf.addFlag('InDetKeys.SplitClusterAmbiguityMap', 'SplitClusterAmbiguityMap') ##+++
    acf.addFlag('InDetKeys.StandardPlotHistName', 'InDetStandardPlots.root') ##+++
    acf.addFlag('InDetKeys.TRTSeededTracks', 'TRTSeededTracks') ##+++
    acf.addFlag('InDetKeys.TRTTracks', 'StandaloneTRTTracks') ##+++
    acf.addFlag('InDetKeys.TRTTracks_NewT', 'TRTStandaloneTracks') ##+++
    acf.addFlag('InDetKeys.TRT_DriftCircles', 'TRT_DriftCircles') ##+++
    acf.addFlag('InDetKeys.TRT_DriftCirclesTruth', 'PRD_MultiTruthTRT') ##+++
    acf.addFlag('InDetKeys.TRT_DriftCirclesUncalibrated', 'TRT_DriftCirclesUncalibrated') ##+++
    acf.addFlag('InDetKeys.TRT_Manager', 'TRT') ##+++
    acf.addFlag('InDetKeys.TRT_PU_DriftCircles', 'TRT_PU_DriftCircles') ##+++
    acf.addFlag('InDetKeys.TRT_PU_DriftCirclesTruth', 'PRD_PU_MultiTruthTRT') ##+++
    acf.addFlag('InDetKeys.TRT_PU_DriftCirclesUncalibrated', 'TRT_PU_DriftCirclesUncalibrated') ##+++
    acf.addFlag('InDetKeys.TRT_PU_RDOs', 'TRT_PU_RDOs') ##+++
    acf.addFlag('InDetKeys.TRT_PU_SDOs', 'TRT_PU_SDO_Map') ##+++
    acf.addFlag('InDetKeys.TRT_RDOs', 'TRT_RDOs') ##+++
    acf.addFlag('InDetKeys.TRT_SDOs', 'TRT_SDO_Map') #+++
    acf.addFlag('InDetKeys.TRT_Segments', 'TRTSegments') ##+++
    acf.addFlag('InDetKeys.TRT_SegmentsTRT', 'TRTSegmentsTRT') #+++
    acf.addFlag('InDetKeys.TRT_Segments_Phase', 'TRTSegments_Phase')##+++
    acf.addFlag('InDetKeys.TRT_Tracks_Phase', 'TRTTracks_Phase') ##+++
    acf.addFlag('InDetKeys.TrackParticles', 'TrackParticleCandidate') ##+++
    acf.addFlag('InDetKeys.TrackParticlesTruth', 'TrackParticleTruthCollection') ##+++
    acf.addFlag('InDetKeys.Tracks', 'Tracks') ##+++
    acf.addFlag('InDetKeys.TracksTruth', 'TrackTruthCollection') ##+++
    acf.addFlag('InDetKeys.UnslimmedDetailedTracksTruth', 'CombinedInDetTracksDetailedTruth')   ##+++
    acf.addFlag('InDetKeys.UnslimmedTracks', 'CombinedInDetTracks') ##+++
    acf.addFlag('InDetKeys.UnslimmedTracksTruth', 'CombinedInDetTracksTruthCollection') ##+++
    acf.addFlag('InDetKeys.V0Candidates', 'V0Candidates') ##+++
    acf.addFlag('InDetKeys.XKalConvertedTracks', 'ConvertedXKalmanTracks') ##+++
    acf.addFlag('InDetKeys.trkValidationNtupleName', 'TrkValidation.root') ##+++
    acf.addFlag('InDetKeys.xAODDBMTrackParticleContainer', 'InDetDBMTrackParticles')  ##+++
    acf.addFlag('InDetKeys.xAODForwardTrackParticleContainer', 'InDetForwardTrackParticles') ##+++
    acf.addFlag('InDetKeys.xAODKshortVertexContainer', 'V0KshortVertices') ##+++
    acf.addFlag('InDetKeys.xAODLambdaVertexContainer', 'V0LambdaVertices') ##+++
    acf.addFlag('InDetKeys.xAODLambdabarVertexContainer', 'V0LambdabarVertices') ##+++
    acf.addFlag('InDetKeys.xAODDisappearingTrackParticleContainer', 'InDetDisappearingTrackParticles') ##+++
    acf.addFlag('InDetKeys.xAODPixelTrackParticleContainer', 'InDetPixelTrackParticles') ##+++
    acf.addFlag('InDetKeys.xAODPseudoTrackParticleContainer', 'InDetPseudoTrackParticles') ##+++
    acf.addFlag('InDetKeys.xAODSCTTrackParticleContainer', 'InDetSCTTrackParticles') ##+++
    acf.addFlag('InDetKeys.xAODTRTTrackParticleContainer', 'InDetTRTTrackParticles') ##+++
    acf.addFlag('InDetKeys.xAODTrackParticleContainer', 'InDetTrackParticles') ##+++
    acf.addFlag('InDetKeys.xAODV0VertexContainer', 'V0UnconstrVertices') ##+++
    acf.addFlag('InDetKeys.xAODVertexContainer', 'PrimaryVertices') ##+++
    from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
    acf.addFlag('InDetNewTrackingCuts', lambda ignored : ConfiguredNewTrackingCuts('Offline'))
    acf.addFlag('InDetPrimaryVertexingCuts.IPd0Max', '4.0')
    acf.addFlag('InDetPrimaryVertexingCuts.IPz0Max', '1000.0')
    acf.addFlag('InDetPrimaryVertexingCuts.TrackCutLevel', 'TightPrimary')
    acf.addFlag('InDetPrimaryVertexingCuts.etaMax', '9999.0')
    acf.addFlag('InDetPrimaryVertexingCuts.fitChi2OnNdfMax', '3.5')
    acf.addFlag('InDetPrimaryVertexingCuts.minPT', '400.0')
    acf.addFlag('InDetPrimaryVertexingCuts.nHitInnermostLayer', 0)
    acf.addFlag('InDetPrimaryVertexingCuts.nHitPix', 1)
    acf.addFlag('InDetPrimaryVertexingCuts.nHitSct', '4')
    acf.addFlag('InDetPrimaryVertexingCuts.nHitSi', '6')
    acf.addFlag('InDetPrimaryVertexingCuts.nHitTrt', 0)
    acf.addFlag('InDetPrimaryVertexingCuts.nHolesPix', 1)
    acf.addFlag('InDetPrimaryVertexingCuts.sigIPd0Max', '5.0')
    acf.addFlag('InDetPrimaryVertexingCuts.sigIPz0Max', '10.0')
    acf.addFlag('InDetPrimaryVertexingCuts.z0Max', '1000.0')
    acf.addFlag('TriggerFlags.doTriggerConfigOnly', False)
    acf.addFlag('athenaCommonFlags.EvtMax', '-1')
    acf.addFlag('athenaCommonFlags.isOnline', False)
    acf.addFlag('conddb.dbdata', 'auto')
    acf.addFlag('conddb.dbmc', 'OFLP200')
    ###acf.addFlag('flagsHash', '<bound method Run2FlagsContainer.flagsHash of <InDetRecExample.IDCommon.Run2FlagsContainer object at 0x7fae89cc1e10>>')
    acf.addFlag('geoFlags.IBLLayout', '3D')
    acf.addFlag('geoFlags.isDBM', True)
    acf.addFlag('geoFlags.isIBL', True)
    acf.addFlag('geoFlags.isSLHC', False)
    acf.addFlag('globalflags.DataSource', 'geant4')
    acf.addFlag('globalflags.DetDescrVersion', 'ATLAS-R2-2016-01-00-01')
    # acf.addFlag('GeoModel.AtlasVersion','ATLAS-R2-2016-01-00-01')
    # acf.addFlag('globalflags.InputFormat', 'pool')
    acf.addFlag('globalflags.isOverlay', False)
    
    acf.addFlag('jobproperties.Beam.beamType', 'collisions')
    acf.addFlag('rec.Commissioning', True)
    acf.addFlag('rec.Production', True)
    acf.addFlag('rec.doAODMerging', False)
    acf.addFlag('rec.doRDOTrigger', False)
    acf.addFlag('rec.doTrigger', False)
    acf.addFlag('rec.triggerStream', '')
    acf.addFlag('recAlgs.doTrigger', False)
    ###acf.addFlag('IOVDb.DatabaseInstance', 'OFLP200')
    # acf.addFlag('Input.isMC',True)
    # acf.addFlag('IOVDb.GlobalTag','OFLCOND-MC16-SDR-27') # ('CONDBR2-BLKPA-2018-03')
    #from IOVDbSvc.IOVDbAutoCfgFlags import getDatabaseInstanceDefault
    #acf.addFlag("IOVDb.DatabaseInstance",getDatabaseInstanceDefault)
    return acf