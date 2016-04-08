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

# Install files from the package:
atlas_install_headers( TrigInDetToolInterfaces )

