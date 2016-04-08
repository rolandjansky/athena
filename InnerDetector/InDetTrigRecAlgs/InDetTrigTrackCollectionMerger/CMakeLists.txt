################################################################################
# Package: InDetTrigTrackCollectionMerger
################################################################################

# Declare the package name:
atlas_subdir( InDetTrigTrackCollectionMerger )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigEvent/TrigParticle )

# Component(s) in the package:
atlas_add_component( InDetTrigTrackCollectionMerger
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrkTrack TrigInterfacesLib TrkToolInterfaces TrigParticle )

# Install files from the package:
atlas_install_headers( InDetTrigTrackCollectionMerger )

