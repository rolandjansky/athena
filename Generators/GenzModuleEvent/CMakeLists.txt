################################################################################
# Package: GenzModuleEvent
################################################################################

# Declare the package name:
atlas_subdir( GenzModuleEvent )

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   Generators/AtlasHepMC)

# Component(s) in the package:
atlas_add_library( GenzModuleEvent
                   src/*.cxx
                   PUBLIC_HEADERS GenzModuleEvent
                   LINK_LIBRARIES AtlasHepMCLib )

