################################################################################
# Package: HepMC_Interfaces
################################################################################

# Declare the package name:
atlas_subdir( HepMC_Interfaces )

atlas_add_library( HepMC_InterfacesLib
                   HepMC_Interfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS HepMC_Interfaces
                   LINK_LIBRARIES AtlasHepMCLib GaudiKernel )

