################################################################################
# Package: MuonSegmentMakerInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentMakerInterfaces )

atlas_add_library( MuonSegmentMakerInterfacesLib
                   MuonSegmentMakerInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonSegmentMakerInterfaces
                   LINK_LIBRARIES AthenaKernel GaudiKernel )

