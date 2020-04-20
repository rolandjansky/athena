################################################################################
# Package: AGDDKernel
################################################################################

# Declare the package name:
atlas_subdir( AGDDKernel )

atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( AGDDKernel
                   src/*.cxx
                   PUBLIC_HEADERS AGDDKernel
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES GeoPrimitives ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} )

