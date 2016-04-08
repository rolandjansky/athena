################################################################################
# Package: AGDDModel
################################################################################

# Declare the package name:
atlas_subdir( AGDDModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/AGDD/AGDDKernel )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( AGDDModel
                   src/*.cxx
                   PUBLIC_HEADERS AGDDModel
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AGDDKernel )

