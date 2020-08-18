################################################################################
# Package: D3PDMakerInterfaces
################################################################################

# Declare the package name:
atlas_subdir( D3PDMakerInterfaces )

atlas_add_library( D3PDMakerInterfaces
                   D3PDMakerInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS D3PDMakerInterfaces
                   LINK_LIBRARIES GaudiKernel )
