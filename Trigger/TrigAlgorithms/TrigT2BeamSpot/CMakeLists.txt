################################################################################
# Package: TrigT2BeamSpot
################################################################################

# Declare the package name:
atlas_subdir( TrigT2BeamSpot )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Control/DataModel
                          Event/EventInfo
                          Event/EventPrimitives
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          Trigger/TrigTools/TrigTimeAlgs )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigT2BeamSpot
                     src/*.cxx
                     src/components/TrigT2*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} TrigSteeringEvent TrigInterfacesLib DataModel EventInfo EventPrimitives GaudiKernel TrkParameters TrkTrack TrkTrackSummary TrigInDetEvent TrigNavigationLib TrigTimeAlgsLib )

# Install files from the package:
atlas_install_headers( TrigT2BeamSpot )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

