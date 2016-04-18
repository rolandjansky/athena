################################################################################
# Package: IsolationTool
################################################################################

# Declare the package name:
atlas_subdir( IsolationTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloClusterCorrection
                          Calorimeter/CaloEvent
                          Calorimeter/CaloRec
                          Calorimeter/CaloUtils
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventShape
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODPrimitives
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          PhysicsAnalysis/AnalysisCommon/IsolationSelection
                          PhysicsAnalysis/ElectronPhotonID/IsolationCorrections
                          Reconstruction/RecoTools/ParticlesInConeTools
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/RecoTools/TrackToCalo
                          Tracking/TrkEvent/TrkCaloExtension
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParametersIdentificationHelpers
                          PRIVATE
                          Calorimeter/CaloGeoHelpers
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( IsolationToolLib
                   Root/*.cxx
                   src/*.cxx
                   src/components/*.cxx
                   PUBLIC_HEADERS IsolationTool
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloEvent AsgTools xAODBase xAODCaloEvent xAODCore xAODEgamma xAODEventShape xAODMuon xAODPFlow xAODPrimitives xAODTracking GaudiKernel RecoToolInterfaces TrkCaloExtension TrkParameters TrkParametersIdentificationHelpers CaloClusterCorrectionLib CaloRecLib CaloUtilsLib IsolationCorrectionsLib TrackToCaloLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} CaloGeoHelpers AthenaBaseComps xAODEventInfo xAODJet )

atlas_add_component( IsolationTool
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} CaloClusterCorrectionLib CaloEvent CaloRecLib CaloUtilsLib AsgTools xAODBase xAODCaloEvent xAODCore xAODEgamma xAODEventShape xAODMuon xAODPFlow xAODPrimitives xAODTracking GaudiKernel IsolationCorrectionsLib RecoToolInterfaces TrackToCaloLib TrkCaloExtension TrkParameters TrkParametersIdentificationHelpers CaloGeoHelpers AthenaBaseComps xAODEventInfo xAODJet IsolationToolLib )

