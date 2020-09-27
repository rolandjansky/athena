################################################################################
# Package: eflowRec
################################################################################

# Declare the package name:
atlas_subdir( eflowRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloRec
                          Control/AthContainers
                          Control/AthLinks
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CxxUtils
                          DetectorDescription/GeoPrimitives
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTau
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          InnerDetector/InDetRecTools/TrackVertexAssociationTool
                          Reconstruction/Particle
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloUtils
                          Control/StoreGate
                          Event/FourMomUtils
                          Tools/PathResolver
                          Tracking/TrkEvent/TrkCaloExtension
                          Tracking/TrkEvent/TrkParametersIdentificationHelpers )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( eflowRec
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} CaloEvent CaloIdentifier CaloRecLib AthContainers AthLinks AthenaBaseComps CxxUtils AthenaKernel GeoPrimitives xAODCaloEvent xAODCore xAODEgamma xAODMuon xAODPFlow xAODTau xAODTracking GaudiKernel InDetReadoutGeometry TRT_ReadoutGeometry TrackVertexAssociationToolLib Particle RecoToolInterfaces TrkParameters CaloDetDescrLib CaloUtilsLib StoreGateLib SGtests FourMomUtils PathResolver TrkCaloExtension TrkParametersIdentificationHelpers InDetTrackSelectionToolLib AthenaMonitoringKernelLib )

# Install files from the package:
atlas_install_headers( eflowRec )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/rmsvsdr.dat )
