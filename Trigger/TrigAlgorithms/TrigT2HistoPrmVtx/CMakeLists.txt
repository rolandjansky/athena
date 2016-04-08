################################################################################
# Package: TrigT2HistoPrmVtx
################################################################################

# Declare the package name:
atlas_subdir( TrigT2HistoPrmVtx )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTracking
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Event/EventInfo
                          Event/EventPrimitives
                          Event/xAOD/xAODBase
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          Reconstruction/Particle
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigTools/TrigTimeAlgs )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigT2HistoPrmVtx
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} xAODTracking TrigInterfacesLib EventInfo EventPrimitives xAODBase GaudiKernel Particle TrigInDetEvent TrigNavigationLib TrigSteeringEvent TrigTimeAlgsLib )

# Install files from the package:
atlas_install_headers( TrigT2HistoPrmVtx )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

