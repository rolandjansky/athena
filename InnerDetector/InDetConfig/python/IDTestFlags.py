def IDTestFlags_q221() :
    # from AthenaConfiguration.AthConfigFlags import AthConfigFlags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    acf = ConfigFlags
    acf.addFlag('DetFlags.SCT_on', True)
    acf.addFlag('DetFlags.pixel_on', True)
    acf.addFlag('InDetFlags.InDet25nsec', True)              ##++++ ##111
    acf.addFlag('InDetFlags.doNNToTCalibration', False)      ##+++  ##111
    acf.addFlag('InDetFlags.doPixelClusterSplitting', True)  ##+++  ##111
    acf.addFlag('InDetFlags.doPixelPRDFormation', True)      ##+++  ##111
    acf.addFlag('InDetFlags.doSCT_PRDFormation', True)       ##+++  ###111
    acf.addFlag('InDetFlags.doSLHC', False)                  ##+++  ##111
    acf.addFlag('InDetFlags.doSplitReco', False)             ##+++  ###111
    acf.addFlag('InDetFlags.doTIDE_Ambi', True)              ##+++  ##111
    acf.addFlag('InDetFlags.pixelClusterBadClusterID', '3')  ##+++  ###111
    acf.addFlag('InDetFlags.pixelClusterSplitMinPt', '1000') ##+++  ##111
    acf.addFlag('InDetFlags.pixelClusterSplitProb1', '0.6')  ##+++  ###111
    acf.addFlag('InDetFlags.pixelClusterSplitProb1_run1', '0.5') ##+++  ###111
    acf.addFlag('InDetFlags.pixelClusterSplitProb2', '0.2')  ##+++  ##111
    acf.addFlag('InDetFlags.pixelClusterSplitProb2_run1', '0.5') ##+++
    acf.addFlag('InDetFlags.pixelClusterSplittingType', 'NeuralNet') ##+++   ###111
    acf.addFlag('InDetFlags.propagatorType', 'RungeKutta')   ##+++     ##111
    acf.addFlag('InDetFlags.selectSCTIntimeHits', True)      ##+++    ##111
    acf.addFlag('InDetFlags.useBeamConstraint', True)        ##+++   ##111
    acf.addFlag('InDetGeometryFlags.useDynamicAlignFolders', False)   ##+++
####################################################################################
####################################################################################
    ###acf.addFlag('InDetKeys.AliasToTracks', 'none')           ##+++
    acf.addFlag('InDetKeys.BCM_RDOs', 'BCM_RDOs')            ##+++
    ###acf.addFlag('InDetKeys.CaloCellContainer', 'AllCalo')     ##+++
    ###acf.addFlag('InDetKeys.CaloClusterContainer', 'LArClusterEM') ##+++
    ###acf.addFlag('InDetKeys.CaloClusterROIContainer', 'InDetCaloClusterROIs') ##+++
    ###acf.addFlag('InDetKeys.CombinedCosmicTracks', 'CombinedCosmicTracks') ##+++
    ###acf.addFlag('InDetKeys.Conversions', 'ConversionCandidate') ##+++
    ###acf.addFlag('InDetKeys.DBMDetailedTracksTruth', 'DBMTracksDetailedTruth') ##+++
    ###acf.addFlag('InDetKeys.DBMTracks', 'ResolvedDBMTracks')   ##+++
    ###acf.addFlag('InDetKeys.DBMTracksTruth', 'DBMTracksTruthCollection') ##+++
    ###acf.addFlag('InDetKeys.DetailedTracksTruth', 'DetailedTrackTruth') ##+++
    ###acf.addFlag('InDetKeys.ExtendedBeamGasTracks', 'ExtendedBeamGasTracks')  ##+++
    ###acf.addFlag('InDetKeys.ExtendedLargeD0Tracks', 'ExtendedLargeD0Tracks') ##+++
    ###acf.addFlag('InDetKeys.ExtendedLowPtTracks', 'ExtendedLowPtTracks') ##+++
    ###acf.addFlag('InDetKeys.ExtendedSLHCTracks', 'ExtendedSLHCTracks')  ##+++
    ###acf.addFlag('InDetKeys.ExtendedTracks', 'ExtendedTracks') ##+++
    #acf.addFlag('InDetKeys.ExtendedTracksMap', 'ExtendedTracksMap') ##+++ 
    #acf.addFlag('InDetKeys.ExtendedTracksMapBeamGas', 'ExtendedTracksMapBeamGas') ##+++
    #acf.addFlag('InDetKeys.ExtendedTracksMapLargeD0', 'ExtendedTracksMapLargeD0') ##+++
    #acf.addFlag('InDetKeys.ExtendedTracksMapLowPt', 'ExtendedTracksMapLowPt')   ##+++
    #acf.addFlag('InDetKeys.ExtendedTracksMapPhase', 'ExtendedTracksMapPhase')   ##+++
    #acf.addFlag('InDetKeys.ExtendedTracksMapDisappearing', 'ExtendedTracksMapDisappearing') ##+++
    #acf.addFlag('InDetKeys.ExtendedTracksMapSLHC', 'ExtendedTracksMapSLHC') ##+++
    ##acf.addFlag('InDetKeys.ExtendedTracksPhase', 'ExtendedTracksPhase')  ##+++
    ##acf.addFlag('InDetKeys.ExtendedTracksDisappearing', 'ExtendedTracksDisappearing') ##+++
    ##acf.addFlag('InDetKeys.GangedPixelMap', 'PixelClusterAmbiguitiesMap')  ##+++
    #acf.addFlag('InDetKeys.HadCaloCellContainer', 'AllCalo')   ##+++
    #acf.addFlag('InDetKeys.HadCaloClusterContainer', 'CaloCalTopoCluster')  ##+++
    #acf.addFlag('InDetKeys.HadCaloClusterROIContainer', 'InDetHadCaloClusterROIs') ##+++
    #acf.addFlag('InDetKeys.IPatConvertedTracks', 'ConvertedIPatTracks')   ##+++
    #acf.addFlag('InDetKeys.LowBetaCandidates', 'InDetLowBetaCandidates')##+++  
    #acf.addFlag('InDetKeys.McEventCollection', 'TruthEvent') ##+++
    #acf.addFlag('InDetKeys.OverlapSpacePoints', 'OverlapSpacePoints')##+++
    acf.addFlag('InDetKeys.PixelClusters', 'PixelClusters') ##+++
    #acf.addFlag('InDetKeys.PixelClustersTruth', 'PRD_MultiTruthPixel') ##+++
    #acf.addFlag('InDetKeys.PixelDetailedTracksTruth', 'ResolvedPixelTrackDetailedTruth') ##+++
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
    #acf.addFlag('InDetKeys.TRTSeededTracks', 'TRTSeededTracks') ##+++
    #acf.addFlag('InDetKeys.TRTTracks', 'StandaloneTRTTracks') ##+++
    #acf.addFlag('InDetKeys.TRTTracks_NewT', 'TRTStandaloneTracks') ##+++
    #acf.addFlag('InDetKeys.TRT_DriftCircles', 'TRT_DriftCircles') ##+++
    #acf.addFlag('InDetKeys.TRT_DriftCirclesTruth', 'PRD_MultiTruthTRT') ##+++
    #acf.addFlag('InDetKeys.TRT_DriftCirclesUncalibrated', 'TRT_DriftCirclesUncalibrated') ##+++
    #acf.addFlag('InDetKeys.TRT_Manager', 'TRT') ##+++
    #acf.addFlag('InDetKeys.TRT_PU_DriftCircles', 'TRT_PU_DriftCircles') ##+++
    #acf.addFlag('InDetKeys.TRT_PU_DriftCirclesTruth', 'PRD_PU_MultiTruthTRT') ##+++
    #acf.addFlag('InDetKeys.TRT_PU_DriftCirclesUncalibrated', 'TRT_PU_DriftCirclesUncalibrated') ##+++
    #acf.addFlag('InDetKeys.TRT_PU_RDOs', 'TRT_PU_RDOs') ##+++
    #acf.addFlag('InDetKeys.TRT_PU_SDOs', 'TRT_PU_SDO_Map') ##+++
    #acf.addFlag('InDetKeys.TRT_RDOs', 'TRT_RDOs') ##+++
    #acf.addFlag('InDetKeys.TRT_SDOs', 'TRT_SDO_Map') #+++
    #acf.addFlag('InDetKeys.TRT_Segments', 'TRTSegments') ##+++
    #acf.addFlag('InDetKeys.TRT_SegmentsTRT', 'TRTSegmentsTRT') #+++
    #acf.addFlag('InDetKeys.TRT_Segments_Phase', 'TRTSegments_Phase')##+++
    #acf.addFlag('InDetKeys.TRT_Tracks_Phase', 'TRTTracks_Phase') ##+++
    #acf.addFlag('InDetKeys.TrackParticles', 'TrackParticleCandidate') ##+++
    #acf.addFlag('InDetKeys.TrackParticlesTruth', 'TrackParticleTruthCollection') ##+++
    #acf.addFlag('InDetKeys.Tracks', 'Tracks') ##+++
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
    ##acf.addFlag('geoFlags.IBLLayout', '3D')
    ##acf.addFlag('geoFlags.isDBM', True)
    ##acf.addFlag('geoFlags.isIBL', True)
    ##acf.addFlag('geoFlags.isSLHC', False)
    ##acf.addFlag('globalflags.DataSource', 'geant4')
    ##acf.addFlag('globalflags.DetDescrVersion', 'ATLAS-R2-2016-01-00-01')
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
    ##acf.addFlag('IOVDb.GlobalTag','OFLCOND-MC16-SDR-16') # ('CONDBR2-BLKPA-2018-03')
    #from IOVDbSvc.IOVDbAutoCfgFlags import getDatabaseInstanceDefault
    #acf.addFlag("IOVDb.DatabaseInstance",getDatabaseInstanceDefault)
    return acf