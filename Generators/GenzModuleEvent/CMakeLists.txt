################################################################################
# Package: GenzModuleEvent
################################################################################

# Declare the package name:
atlas_subdir( GenzModuleEvent )

# External dependencies:
find_package( HepMC )

# Component(s) in the package:
atlas_add_library( GenzModuleEvent
                   src/*.cxx
                   PUBLIC_HEADERS GenzModuleEvent
                   INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                   LINK_LIBRARIES ${HEPMC_LIBRARIES} )

