################################################################################
# Package: ISF_HepMC_Event
################################################################################

# Declare the package name:
atlas_subdir( ISF_HepMC_Event )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Simulation/ISF/ISF_Core/ISF_Event )

# External dependencies:
find_package( HepMC )

# Install files from the package:
atlas_install_headers( ISF_HepMC_Event )

