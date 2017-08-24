################################################################################
# Package: TrigFastTrackFinder
################################################################################

# Declare the package name:
atlas_subdir( TrigFastTrackFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigInDetPattRecoTools
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/IRegionSelector
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetRecEvent/SiSpacePointsSeed
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigEvent/TrigInDetPattRecoEvent
                          Trigger/TrigFTK/FTK_DataProviderInterfaces
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          Trigger/TrigTools/TrigTimeAlgs )

# External dependencies:
find_package( TBB )

# Component(s) in the package:
atlas_add_component( TrigFastTrackFinder
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TBB_LIBRARIES} GaudiKernel TrkEventPrimitives TrigInDetEvent TrigSteeringEvent TrigInterfacesLib TrigInDetPattRecoTools AthenaBaseComps IRegionSelector InDetIdentifier InDetPrepRawData InDetRIO_OnTrack SiSpacePointsSeed InDetRecToolInterfaces TrkParameters TrkRIO_OnTrack TrkTrack TrkTrackSummary TrkToolInterfaces TrigInDetPattRecoEvent FTK_DataProviderInterfaces TrigTimeAlgsLib )

# Install files from the package:
atlas_install_headers( TrigFastTrackFinder )
atlas_install_python_modules( python/*.py )

