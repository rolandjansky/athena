################################################################################
# Package: ISF_Geant4Interfaces
################################################################################

# Declare the package name:
atlas_subdir( ISF_Geant4Interfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

# Install files from the package:
atlas_install_headers( ISF_Geant4Interfaces )

