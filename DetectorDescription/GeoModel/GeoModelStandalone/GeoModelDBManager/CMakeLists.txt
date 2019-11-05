################################################################################
# Package: GeoModelDBManager
################################################################################

# Declare the package name:
atlas_subdir( GeoModelDBManager )

# comment if you want to get debug messages in Release
if(CMAKE_BUILD_TYPE MATCHES Release)
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif(CMAKE_BUILD_TYPE MATCHES Release)
if(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DQT_NO_DEBUG_OUTPUT) # comment if you need debug messages in Debug build
endif(CMAKE_BUILD_TYPE MATCHES Debug)


# External dependencies:
find_package( Qt5 COMPONENTS Core Sql )

# Component(s) in the package:
atlas_add_library( GeoModelDBManager
                   src/*.cpp
                   GeoModelDBManager/*.h
                   PUBLIC_HEADERS GeoModelDBManager
                   LINK_LIBRARIES Qt5::Sql )
