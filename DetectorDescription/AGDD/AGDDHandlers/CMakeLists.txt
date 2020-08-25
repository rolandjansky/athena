################################################################################
# Package: AGDDHandlers
################################################################################

# Declare the package name:
atlas_subdir( AGDDHandlers )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( AGDDHandlers
                   src/*.cxx
                   PUBLIC_HEADERS AGDDHandlers
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AGDDControl AGDDKernel
                   PRIVATE_LINK_LIBRARIES AGDDModel )

