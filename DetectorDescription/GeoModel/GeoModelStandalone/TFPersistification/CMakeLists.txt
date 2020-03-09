################################################################################
# Package: TFPersistification
################################################################################

# Declare the package name:
atlas_subdir( TFPersistification )

# Declare the package's dependencies:
# TODO: we can skip the dependency on GeoPrimitives when we convert all methods to Eigen (GeoTrf::Transform3D)
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          )
                          
# External dependencies:
find_package( CLHEP ) # TODO: we can skip the dependency on CLHEP when we convert all methods to Eigen (GeoTrf::Transform3D)
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( TFPersistification
                   src/*.cpp
                   TFPersistification/*.h
                   PUBLIC_HEADERS TFPersistification
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} GeoPrimitives )
