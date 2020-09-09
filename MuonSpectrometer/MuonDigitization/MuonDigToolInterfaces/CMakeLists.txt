################################################################################
# Package: MuonDigToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonDigToolInterfaces )

atlas_add_library( MuonDigToolInterfacesLib
                   MuonDigToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonDigToolInterfaces
                   LINK_LIBRARIES GaudiKernel )
