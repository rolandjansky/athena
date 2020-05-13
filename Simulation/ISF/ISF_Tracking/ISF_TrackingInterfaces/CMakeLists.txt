################################################################################
# Package: ISF_TrackingInterfaces
################################################################################

# Declare the package name:
atlas_subdir( ISF_TrackingInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

atlas_add_library( ISF_TrackingInterfacesLib
                   ISF_TrackingInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS ISF_TrackingInterfaces
                   LINK_LIBRARIES GaudiKernel )
