################################################################################
# Package: MuonCablingTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCablingTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( CablingTools
                   src/*.cxx
                   PUBLIC_HEADERS MuonCablingTools
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_library( MuonCablingTools
                   src/*.cxx
                   PUBLIC_HEADERS MuonCablingTools
                   LINK_LIBRARIES CablingTools
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

