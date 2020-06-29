################################################################################
# Package: ISF_TrackingInterfaces
################################################################################

# Declare the package name:
atlas_subdir( ISF_TrackingInterfaces )

atlas_add_library( ISF_TrackingInterfacesLib
                   ISF_TrackingInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS ISF_TrackingInterfaces
                   LINK_LIBRARIES GaudiKernel )
