################################################################################
# Package: GeoModelKernel
################################################################################

# Declare the package name:
atlas_subdir( GeoModelKernel )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( GeoModelKernel
                   src/*.cxx
                   PUBLIC_HEADERS GeoModelKernel
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} )

