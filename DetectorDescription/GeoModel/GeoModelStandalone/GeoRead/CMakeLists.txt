################################################################################
# Package: GeoRead
################################################################################

# Declare the package name:
atlas_subdir( GeoRead )

# Declare the package's dependencies:
# TODO: we can skip the dependency on GeoPrimitives when we convert all methods to Eigen (GeoTrf::Transform3D)
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          )

# External dependencies:
find_package( Qt5 COMPONENTS Core ) # needed for QDebug
find_package( CLHEP ) # to be dropped when migrated all methods to the new Eigen-based GeoTrf
find_package( Eigen )
find_package( GeoModelCore )



if(CMAKE_BUILD_TYPE MATCHES Release)
    add_definitions(-DQT_NO_DEBUG_OUTPUT) # comment out if you want to get debug messages in Release
endif(CMAKE_BUILD_TYPE MATCHES Release)
if(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    add_definitions(-DQT_NO_DEBUG_OUTPUT) # comment out if you want to get debug messages in RelWithDebInfo
endif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)

# Component(s) in the package:
atlas_add_library( GeoRead
                   src/*.cpp
                   PUBLIC_HEADERS GeoRead
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} GeoModelDBManager TFPersistification)
                   #LINK_LIBRARIES ${CLHEP_LIBRARIES} Qt5::Widgets GeoModelKernel GeoModelDBManager TFPersistification VP1Base )
# TODO: we should get rid of VP1Base::VP1Msg dependency, since GeoRead should not depend on VP1 packages. Maybe we can move VP1Msg to a standalone package.


