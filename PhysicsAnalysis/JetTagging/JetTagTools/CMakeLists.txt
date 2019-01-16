# $Id: CMakeLists.txt 793433 2017-01-23 10:57:26Z dguest $
################################################################################
# Package: JetTagTools
################################################################################

# Declare the package name:
atlas_subdir( JetTagTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
    PUBLIC
    Control/AthToolSupport/AsgTools
    Control/AthenaBaseComps
    Control/SGTools
    DetectorDescription/GeoPrimitives
    Event/xAOD/xAODBTagging
    Event/xAOD/xAODJet
    Event/xAOD/xAODTracking
    GaudiKernel
    PhysicsAnalysis/JetTagging/JetTagInfo
    Reconstruction/Jet/JetRec
    Reconstruction/Jet/JetSubStructureMomentTools
    Reconstruction/Jet/JetSubStructureUtils
    Reconstruction/egamma/egammaMVACalib
    Reconstruction/MVAUtils
    Tracking/TrkEvent/TrkParameters
    PhysicsAnalysis/Interfaces/MuonAnalysisInterfaces

    PRIVATE
    Control/AthenaKernel
    Control/Navigation
    Event/xAOD/xAODMuon
    InnerDetector/InDetRecTools/InDetTrackSelectionTool
    InnerDetector/InDetRecTools/TrackVertexAssociationTool
    PhysicsAnalysis/AnalysisCommon/ParticleJetTools
    PhysicsAnalysis/JetTagging/JetTagCalibration
    PhysicsAnalysis/MuonID/MuonSelectorTools
    PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonMomentumCorrections
    Reconstruction/Jet/JetEDM
    Reconstruction/Particle
    Reconstruction/RecoTools/ITrackToVertex
    Tracking/TrkEvent/TrkEventPrimitives
    Tracking/TrkEvent/TrkLinks
    Tracking/TrkEvent/TrkTrack
    Tracking/TrkEvent/TrkTrackLink
    Tracking/TrkEvent/TrkTrackSummary
    Tracking/TrkEvent/VxJetVertex
    Tracking/TrkEvent/VxSecVertex
    Tracking/TrkEvent/VxVertex
    Tracking/TrkUtilityPackages/TrkNeuralNetworkUtils
    Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
    )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Eigen )
find_package( lwtnn )
find_package( ROOT COMPONENTS TMVA Core Tree MathCore Hist RIO pthread
    MathMore Minuit Minuit2 Matrix Physics HistPainter Rint RooFitCore RooFit )

# Switch off the -Werror=return-type flag explicitly as a temporary solution
# to a build issue against Boost.
foreach( mode RELEASE RELWITHDEBINFO )
    string( REPLACE "-Werror=return-type" "" CMAKE_CXX_FLAGS_${mode}
        "${CMAKE_CXX_FLAGS_${mode}}" )
endforeach()

# Component(s) in the package:
atlas_add_library(
    JetTagToolsLib

    JetTagTools/*.h
    src/DummyTag.cxx src/TrackSelector.cxx
    src/HistoHelperRoot.cxx src/NewLikelihoodTool.cxx
    src/BasicTrackGradeFactory.cxx src/DetailedTrackGradeFactory.cxx
    src/GradedTrack.cxx src/IPTag.cxx src/JetTagUtils.cxx
    src/SVForIPTool.cxx src/SVTag.cxx src/JetFitterTag.cxx
    src/JetFitterNtupleWriter.cxx src/JetFitterNNTool.cxx
    src/JetFitterInputWriter.cxx src/JetFitterDummyClassifier.cxx
    src/MV1Tag.cxx src/MV1cTag.cxx src/MV2Tag.cxx
    src/MyTag.cxx
    src/JetFitterVariablesFactory.cxx src/MSVVariablesFactory.cxx
    src/MultiSVTag.cxx src/JetVertexCharge.cxx src/SoftMuonTag.cxx
    src/ExKtbbTag.cxx src/ExKtbbTagTool.cxx src/MultivariateTagManager.cxx
    src/DL1Tag.cxx src/LightweightNeuralNetwork.cxx src/TagNtupleDumper.cxx
    src/parse_json.cxx src/Stack.cxx src/RNNIPTag.cxx

    PUBLIC_HEADERS JetTagTools
    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
    ${EIGEN_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
    PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS}
    DEFINITIONS ${CLHEP_DEFINITIONS}

    LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES}
    ${LWTNN_LIBRARIES}
    AsgTools AthenaBaseComps SGTools GeoPrimitives xAODBTagging xAODJet
    xAODTracking GaudiKernel JetTagInfo JetSubStructureUtils TrkParameters
    JetRecLib JetSubStructureMomentToolsLib egammaMVACalibLib MVAUtils
    TrackVertexAssociationToolLib ParticleJetToolsLib JetTagCalibrationLib
    MuonSelectorToolsLib MuonMomentumCorrectionsLib TrkNeuralNetworkUtilsLib
    InDetSVWithMuonToolLib MuonAnalysisInterfacesLib

    PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${FASTJET_LIBRARIES} AthenaKernel
    Navigation xAODMuon JetEDM Particle ITrackToVertex TrkEventPrimitives
    TrkLinks TrkTrack TrkTrackSummary VxJetVertex VxVertex VxSecVertex
    TrkVertexFitterInterfaces
    )

atlas_add_component( JetTagTools
    src/components/*.cxx
    LINK_LIBRARIES GaudiKernel JetTagToolsLib )

# Install files from the package:
atlas_install_runtime( share/*.root share/*.txt )

