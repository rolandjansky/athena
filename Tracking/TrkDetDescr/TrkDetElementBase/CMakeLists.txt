################################################################################
# Package: TrkDetElementBase
################################################################################

# Declare the package name:
atlas_subdir( TrkDetElementBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives )

# External dependencies:
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( TrkDetElementBase
                   src/*.cxx
                   PUBLIC_HEADERS TrkDetElementBase
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} GeoPrimitives Identifier EventPrimitives )

