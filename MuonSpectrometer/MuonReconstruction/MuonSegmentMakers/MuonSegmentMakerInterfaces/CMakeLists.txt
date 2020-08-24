################################################################################
# Package: MuonSegmentMakerInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentMakerInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

atlas_add_library( MuonSegmentMakerInterfacesLib
                   MuonSegmentMakerInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonSegmentMakerInterfaces
                   LINK_LIBRARIES AthenaKernel GaudiKernel )

