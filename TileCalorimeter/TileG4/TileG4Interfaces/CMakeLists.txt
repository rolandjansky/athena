################################################################################
# Package: TileG4Interfaces
################################################################################

# Declare the package name:
atlas_subdir( TileG4Interfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          DetectorDescription/Identifier
                           )

# External dependencies:
find_package( Geant4 )

# Install files from the package:
atlas_install_headers( TileG4Interfaces )

