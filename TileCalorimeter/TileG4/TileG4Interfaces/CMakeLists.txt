################################################################################
# Package: TileG4Interfaces
################################################################################

# Declare the package name:
atlas_subdir( TileG4Interfaces )

# External dependencies:
find_package( Geant4 )

atlas_add_library( TileG4InterfacesLib
                   TileG4Interfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS TileG4Interfaces
                   LINK_LIBRARIES GaudiKernel Identifier )
