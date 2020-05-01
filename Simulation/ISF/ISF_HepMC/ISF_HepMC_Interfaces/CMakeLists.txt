################################################################################
# Package: ISF_HepMC_Interfaces
################################################################################

# Declare the package name:
atlas_subdir( ISF_HepMC_Interfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Generators/AtlasHepMC
                          Simulation/ISF/ISF_Core/ISF_Event )

atlas_add_library( ISF_HepMC_Interfaces
                   ISF_HepMC_Interfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS ISF_HepMC_Interfaces
                   LINK_LIBRARIES AtlasHepMCLib GaudiKernel ISF_Event )

