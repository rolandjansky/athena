################################################################################
# Package: ISF_HepMC_Interfaces
################################################################################

# Declare the package name:
atlas_subdir( ISF_HepMC_Interfaces )

atlas_add_library( ISF_HepMC_Interfaces
                   ISF_HepMC_Interfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS ISF_HepMC_Interfaces
                   LINK_LIBRARIES AtlasHepMCLib GaudiKernel ISF_Event )

