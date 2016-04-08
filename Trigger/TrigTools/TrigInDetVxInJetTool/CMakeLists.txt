################################################################################
# Package: TrigInDetVxInJetTool
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetVxInJetTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Trigger/TrigTools/TrigVKalFitter
                          PRIVATE
                          PhysicsAnalysis/AnalysisCommon/AnalysisUtils
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigSteer/TrigInterfaces )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigInDetVxInJetTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel TrigVKalFitterLib AnalysisUtilsLib VxVertex TrigInDetEvent TrigInterfacesLib )

# Install files from the package:
atlas_install_headers( TrigInDetVxInJetTool )

