# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetPerformanceMonitoring )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO Physics Graf )

# Component(s) in the package:
atlas_add_component( InDetPerformanceMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringLib AtlasHepMCLib CxxUtils ElectronPhotonSelectorToolsLib EventPrimitives GaudiKernel ITrackToVertex IdDictDetDescr InDetBeamSpotServiceLib InDetIdentifier InDetPrepRawData InDetRIO_OnTrack InDetTrackSelectionToolLib JetInterface LWHists MuonAnalysisInterfacesLib MuonMomentumCorrectionsLib StoreGateLib TRT_ReadoutGeometry TrigDecisionToolLib TrkEventPrimitives TrkExInterfaces TrkParameters TrkParticleBase TrkTrack TrkTrackSummary TrkTruthData TrkV0Vertex TrkVertexAnalysisUtilsLib TrkVertexFitterInterfaces TrackVertexAssociationToolLib egammaEvent egammaInterfacesLib xAODCaloEvent xAODEgamma xAODEventInfo xAODJet xAODMissingET xAODMuon xAODTracking xAODTruth )

# Install files from the package:
atlas_install_joboptions( share/*.py )

