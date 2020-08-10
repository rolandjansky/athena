################################################################################
# Package: AGDDKernel
################################################################################

# Declare the package name:
atlas_subdir( AGDDKernel )

atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives )

# External dependencies:
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( AGDDKernel
                   src/*.cxx
                   PUBLIC_HEADERS AGDDKernel
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES GeoPrimitives ${GEOMODELCORE_LIBRARIES} )

