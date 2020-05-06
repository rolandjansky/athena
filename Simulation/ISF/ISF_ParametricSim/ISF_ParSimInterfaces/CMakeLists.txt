################################################################################
# Package: ISF_ParSimInterfaces
################################################################################

# Declare the package name:
atlas_subdir( ISF_ParSimInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters )

atlas_add_library( ISF_ParSimInterfacesLib
                   ISF_ParSimInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS ISF_ParSimInterfaces
                   LINK_LIBRARIES GaudiKernel TrkParameters xAODTracking )
