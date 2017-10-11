################################################################################
# Package: TrackCaloClusterRecInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrackCaloClusterRecInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Tracking/TrkEvent/TrkCaloExtension
                          Tracking/TrkEvent/TrkEventPrimitives )
                          
# Declare library:
atlas_add_library( TrackCaloClusterRecInterfacesLib
                   TrackCaloClusterRecInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrackCaloClusterRecInterfaces
                   LINK_LIBRARIES GaudiKernel xAODBase xAODCaloEvent TrkCaloExtension TrkEventPrimitives )
