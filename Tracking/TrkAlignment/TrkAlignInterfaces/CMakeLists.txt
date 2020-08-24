################################################################################
# Package: TrkAlignInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrkAlignInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          GaudiKernel
                          Tracking/TrkAlignment/TrkAlignEvent
                          Tracking/TrkEvent/TrkTrack )

atlas_add_library( TrkAlignInterfaces
                   TrkAlignInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrkAlignInterfaces
                   LINK_LIBRARIES GaudiKernel AthContainers TrkAlignEvent TrkTrack )

