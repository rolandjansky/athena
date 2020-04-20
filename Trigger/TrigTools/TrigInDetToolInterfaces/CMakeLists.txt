################################################################################
# Package: TrigInDetToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/IRegionSelector
                          DetectorDescription/Identifier
                          Event/ByteStreamData
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigInDetPattRecoEvent
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TrigInDetToolInterfacesLib
                   TrigInDetToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrigInDetToolInterfaces
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ByteStreamData GaudiKernel IRegionSelector Identifier InDetPrepRawData TrigInDetEvent TrigInDetPattRecoEvent TrigSteeringEvent TrkEventPrimitives TrkTrack )
