################################################################################
# Package: tauRecTools
################################################################################

# Declare the package name:
atlas_subdir( tauRecTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTracking
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Calorimeter/CaloUtils
                          Control/AthContainers
                          Control/CxxUtils
                          DetectorDescription/AtlasDetDescr
                          Event/EventKernel
                          Event/FourMom
                          Event/FourMomUtils
                          Event/NavFourMom
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODPFlow
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          PhysicsAnalysis/AnalysisCommon/AnalysisUtils
                          Reconstruction/Jet/JetEDM
                          Reconstruction/Jet/JetEvent
                          Reconstruction/Particle
                          Reconstruction/RecoTools/ITrackToVertex
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/tauEvent
                          Tools/PathResolver
                          Tracking/TrkEvent/TrkLinks
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParametersIdentificationHelpers
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Tracking/TrkVertexFitter/TrkVertexFitters
                          Tracking/TrkVertexFitter/TrkVxEdmCnv )

# External dependencies:
find_package( AIDA )
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS TMVA Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( tauRecToolsLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS tauRecTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${AIDA_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthLinks AsgTools xAODTau xAODTracking CaloUtilsLib AnalysisUtilsLib TrkVertexFittersLib TrkVxEdmCnvLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloEvent CaloGeoHelpers CaloIdentifier AthContainers CxxUtils AtlasDetDescr EventKernel FourMom FourMomUtils NavFourMom xAODCaloEvent xAODEventInfo xAODJet xAODPFlow GaudiKernel InDetRecToolInterfaces JetEDM JetEvent Particle ITrackToVertex RecoToolInterfaces tauEvent PathResolver TrkLinks TrkParameters TrkParametersIdentificationHelpers TrkParticleBase TrkTrackSummary VxVertex TrkToolInterfaces TrkVertexFitterInterfaces )

atlas_add_component( tauRecTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${AIDA_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks AsgTools xAODTau xAODTracking CaloEvent CaloGeoHelpers CaloIdentifier CaloUtilsLib AthContainers CxxUtils AtlasDetDescr EventKernel FourMom FourMomUtils NavFourMom xAODCaloEvent xAODEventInfo xAODJet xAODPFlow GaudiKernel InDetRecToolInterfaces AnalysisUtilsLib JetEDM JetEvent Particle ITrackToVertex RecoToolInterfaces tauEvent PathResolver TrkLinks TrkParameters TrkParametersIdentificationHelpers TrkParticleBase TrkTrackSummary VxVertex TrkToolInterfaces TrkVertexFitterInterfaces TrkVertexFittersLib TrkVxEdmCnvLib tauRecToolsLib )

