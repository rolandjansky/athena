################################################################################
# Package: ISF_HepMC_Interfaces
################################################################################

# Declare the package name:
atlas_subdir( ISF_HepMC_Interfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Simulation/ISF/ISF_Core/ISF_Event )

# Install files from the package:
atlas_install_headers( ISF_HepMC_Interfaces )

