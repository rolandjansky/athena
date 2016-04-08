################################################################################
# Package: AGDDKernel
################################################################################

# Declare the package name:
atlas_subdir( AGDDKernel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoModel/GeoModelKernel )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( AGDDKernel
                   src/*.cxx
                   PUBLIC_HEADERS AGDDKernel
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} GeoModelKernel )

