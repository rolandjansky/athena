# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetTagTools )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS TMVA Core Tree MathCore Hist RIO pthread
    MathMore Minuit Minuit2 Matrix Physics HistPainter Rint RooFitCore RooFit )

# Component(s) in the package:
atlas_add_library( JetTagToolsLib
    JetTagTools/*.h
    src/TrackSelector.cxx
    src/HistoHelperRoot.cxx src/NewLikelihoodTool.cxx
    src/BasicTrackGradeFactory.cxx src/DetailedTrackGradeFactory.cxx
    src/GradedTrack.cxx src/IPTag.cxx
    src/SVForIPTool.cxx src/SVTag.cxx src/JetFitterTag.cxx
    src/JetFitterNtupleWriter.cxx src/JetFitterNNTool.cxx
    src/JetFitterInputWriter.cxx src/JetFitterDummyClassifier.cxx
    src/MV1Tag.cxx src/MV2Tag.cxx
    src/JetFitterVariablesFactory.cxx src/MSVVariablesFactory.cxx
    src/MultiSVTag.cxx src/SoftMuonTag.cxx
    src/MultivariateTagManager.cxx
    src/TagNtupleDumper.cxx

    PUBLIC_HEADERS JetTagTools
    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
    PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
    PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
    LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils GaudiKernel
    GeoPrimitives ITrackToVertex InDetRecToolInterfaces InDetSVWithMuonToolLib JetTagCalibrationLib JetTagInfo
    MVAUtils StoreGateLib xAODBTagging xAODJet xAODTracking
    PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} InDetTrackSelectionToolLib MuonAnalysisInterfacesLib
    Navigation Particle ParticleJetToolsLib TrkEventPrimitives TrkLinks TrkNeuralNetworkUtilsLib
    TrkParameters TrkTrack TrkTrackLink TrkTrackSummary TrkVertexFitterInterfaces VxJetVertex
    VxSecVertex VxVertex xAODMuon )

atlas_add_component( JetTagTools
    src/components/*.cxx
    LINK_LIBRARIES JetTagToolsLib )

# Install files from the package:
atlas_install_runtime( share/*.root share/*.txt )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
