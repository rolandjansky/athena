################################################################################
# Package: tauRecTools
################################################################################

# Declare the package name:
atlas_subdir( tauRecTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloUtils
                          Control/AthLinks
                          Control/AthToolSupport/AsgTools
                          Control/CxxUtils
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTracking
                          PRIVATE
                          Calorimeter/CaloInterface
                          Control/AthContainers
                          Event/FourMomUtils
                          Event/NavFourMom
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODPFlow
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          PhysicsAnalysis/TauID/TauAnalysisTools
                          Reconstruction/Jet/JetEDM
                          Reconstruction/Particle
                          Reconstruction/RecoTools/ITrackToVertex
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Tools/PathResolver
                          Tracking/TrkEvent/TrkLinks
                          Tracking/TrkEvent/TrkParametersIdentificationHelpers
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Tracking/TrkVertexFitter/TrkVertexFitters
                          Tracking/TrkVertexFitter/TrkVxEdmCnv )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS TMVA Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( tauRecToolsLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS tauRecTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthLinks AsgTools CxxUtils xAODEventInfo xAODTau xAODTracking CaloUtilsLib TrkVertexFittersLib TrkVxEdmCnvLib
                   PRIVATE_LINK_LIBRARIES AthContainers FourMomUtils NavFourMom xAODCaloEvent xAODJet xAODPFlow GaudiKernel InDetRecToolInterfaces JetEDM Particle ITrackToVertex RecoToolInterfaces PathResolver TrkLinks TrkParametersIdentificationHelpers TrkTrackSummary VxVertex TrkToolInterfaces TrkVertexFitterInterfaces )

atlas_add_component( tauRecTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} CaloUtilsLib AthLinks AsgTools CxxUtils xAODEventInfo xAODTau xAODTracking AthContainers FourMomUtils NavFourMom xAODCaloEvent xAODJet xAODPFlow GaudiKernel InDetRecToolInterfaces JetEDM Particle ITrackToVertex RecoToolInterfaces PathResolver TrkLinks TrkParametersIdentificationHelpers TrkTrackSummary VxVertex TrkToolInterfaces TrkVertexFitterInterfaces TrkVertexFittersLib TrkVxEdmCnvLib tauRecToolsLib )

# Install files from the package:
atlas_install_runtime( share/*.xml )
atlas_install_data( share/* )

