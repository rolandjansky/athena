################################################################################
# Package: TrigSecVtxFinder
################################################################################

# Declare the package name:
atlas_subdir( TrigSecVtxFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigInDetVxInJetTool
                          Trigger/TrigTools/TrigTimeAlgs
                          Trigger/TrigTools/TrigTrackJetFinderTool
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          PRIVATE
                          Control/CxxUtils
                          DetectorDescription/GeoPrimitives
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TrigSecVtxFinder
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} GaudiKernel TrigInterfacesLib TrigTimeAlgsLib GeoPrimitives TrigInDetEvent TrigSteeringEvent )

# Install files from the package:
atlas_install_headers( TrigSecVtxFinder )
atlas_install_python_modules( python/*.py )

