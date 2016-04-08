################################################################################
# Package: TrigHypoCommonTools
################################################################################

# Declare the package name:
atlas_subdir( TrigHypoCommonTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/ByteStreamCnvSvcBase
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigSteer/TrigSteering
                          PRIVATE
                          DetectorDescription/RegionSelector
                          Event/EventInfo
                          GaudiKernel
                          Tools/PathResolver
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigHypoCommonTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ByteStreamCnvSvcBaseLib TrigInDetEvent TrigSteeringEvent TrigInterfacesLib TrigSteeringLib RegionSelectorLib EventInfo GaudiKernel PathResolver TrigT1Result )

# Install files from the package:
atlas_install_headers( TrigHypoCommonTools )

