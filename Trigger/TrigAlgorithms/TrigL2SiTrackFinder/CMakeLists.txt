################################################################################
# Package: TrigL2SiTrackFinder
################################################################################

# Declare the package name:
atlas_subdir( TrigL2SiTrackFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/IRegionSelector
                          GaudiKernel
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Control/StoreGate
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigTools/TrigInDetRecoTools
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          Trigger/TrigTools/TrigSiSpacePointTool
                          Trigger/TrigTools/TrigTimeAlgs )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TrigL2SiTrackFinder
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} IRegionSelector GaudiKernel TrkTrack TrigInDetEvent TrigSteeringEvent TrigInterfacesLib StoreGateLib SGtests InDetIdentifier InDetPrepRawData InDetRIO_OnTrack TrkToolInterfaces TrigTimeAlgsLib )

# Install files from the package:
atlas_install_headers( TrigL2SiTrackFinder )
atlas_install_python_modules( python/*.py )

