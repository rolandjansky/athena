################################################################################
# Package: MuonDigToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonDigToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

atlas_add_library( MuonDigToolInterfacesLib
                   MuonDigToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonDigToolInterfaces
                   LINK_LIBRARIES GaudiKernel )
