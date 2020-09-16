################################################################################
# Package: MuonCablingTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCablingTools )

# Component(s) in the package:
atlas_add_library( CablingTools
                   src/*.cxx
                   PUBLIC_HEADERS MuonCablingTools
                   LINK_LIBRARIES AthenaKernel
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_library( MuonCablingTools
                   src/*.cxx
                   PUBLIC_HEADERS MuonCablingTools
                   LINK_LIBRARIES CablingTools AthenaKernel
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

