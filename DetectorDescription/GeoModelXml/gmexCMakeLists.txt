cmake_minimum_required( VERSION 3.1 )

# Set up the project.
project( "GeoModelXml" VERSION 1.0.0 LANGUAGES CXX )

find_package( GeoModelCore REQUIRED )
find_package( Eigen3 REQUIRED )
find_package( GeoModelTools REQUIRED )
find_package( Eigen3 REQUIRED )
find_package( XercesC REQUIRED )
find_package( ZLIB REQUIRED )


# Set default build options.
set( CMAKE_BUILD_TYPE "Release" CACHE STRING "CMake build mode to use" )
set( CMAKE_CXX_STANDARD 14 CACHE STRING "C++ standard used for the build" )
set( CMAKE_CXX_EXTENSIONS FALSE CACHE BOOL "(Dis)allow using GNU extensions" )

# Use the GNU install directory names.
include( GNUInstallDirs )

# Find the header and source files.
file( GLOB SOURCES src/*.cxx )
file( GLOB HEADERS GeoModelXml/*.h )
file( GLOB SHAPEHEADERS GeoModelXml/shape/*.h )


# Create the library.
add_library( GeoModelXml SHARED ${HEADERS} ${SOURCES} )
set_property( TARGET GeoModelXml
   PROPERTY PUBLIC_HEADER ${HEADERS} )

target_compile_definitions( GeoModelXml PUBLIC -DSTANDALONE_GMX )
target_link_libraries( GeoModelXml PUBLIC GeoModelCore::GeoModelKernel GeoModelTools::ExpressionEvaluator XercesC::XercesC ZLIB::ZLIB  )
#  target_include_directories( AGDDKernel SYSTEM PUBLIC ${EIGEN3_INCLUDE_DIR} ) )
target_include_directories( GeoModelXml PUBLIC ${EIGEN3_INCLUDE_DIR}
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
   $<INSTALL_INTERFACE:include> )
source_group( "GeoModelXml" FILES ${HEADERS} )
source_group( "src" FILES ${SOURCES} )


# 
install( TARGETS GeoModelXml EXPORT GeoModelXml-export LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/GeoModelXml )
install( FILES ${SHAPEHEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/GeoModelXml/shape)
#
# Version the shared library. (Please update when cutting a new release!)
#
set(MYLIB_VERSION_MAJOR 1)
set(MYLIB_VERSION_MINOR 1)
set(MYLIB_VERSION_PATCH 0)
set(MYLIB_VERSION_STRING ${MYLIB_VERSION_MAJOR}.${MYLIB_VERSION_MINOR}.${MYLIB_VERSION_PATCH})

set_target_properties(GeoModelXml PROPERTIES VERSION ${MYLIB_VERSION_STRING} SOVERSION ${MYLIB_VERSION_MAJOR})

