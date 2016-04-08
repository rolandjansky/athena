################################################################################
# Package: AGDDHandlers
################################################################################

# Declare the package name:
atlas_subdir( AGDDHandlers )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/AGDD/AGDDControl
                          DetectorDescription/AGDD/AGDDKernel
                          PRIVATE
                          DetectorDescription/AGDD/AGDDModel )

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

