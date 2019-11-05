################################################################################
# Package: GeoWrite
################################################################################

# Declare the package name:
atlas_subdir( GeoWrite )

# Declare the package's dependencies:
# TODO: we can skip the dependency on GeoPrimitives when we convert all methods to Eigen (GeoTrf::Transform3D)
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          )

# External dependencies:
find_package( Qt5 COMPONENTS Core ) # needed for Qt containers
find_package( CLHEP )  # to be dropped when migrated to new Eigen-based GeoTrf
find_package( Eigen )
find_package( GeoModel )


if(CMAKE_BUILD_TYPE MATCHES Release)
    add_definitions(-DQT_NO_DEBUG_OUTPUT) # comment if you need debug messages in Release build
endif(CMAKE_BUILD_TYPE MATCHES Release)
if(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    add_definitions(-DQT_NO_DEBUG_OUTPUT) # comment if you need debug messages in RelWithDebInfo build
endif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DQT_NO_DEBUG_OUTPUT) # comment if you need debug messages in Debug build
endif(CMAKE_BUILD_TYPE MATCHES Debug)

# Component(s) in the package:
atlas_add_library( GeoWrite
                   src/*.cpp
                   PUBLIC_HEADERS GeoWrite
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} Qt5::Core ${GEOMODEL_LIBRARIES} GeoModelDBManager TFPersistification VP1Base )
