################################################################################
# Package: ISF_ParSimInterfaces
################################################################################

# Declare the package name:
atlas_subdir( ISF_ParSimInterfaces )

atlas_add_library( ISF_ParSimInterfacesLib
                   ISF_ParSimInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS ISF_ParSimInterfaces
                   LINK_LIBRARIES GaudiKernel TrkParameters xAODTracking )
