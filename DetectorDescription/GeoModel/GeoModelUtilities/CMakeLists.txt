################################################################################
# Package: GeoModelUtilities
################################################################################

# Declare the package name:
atlas_subdir( GeoModelUtilities )

if(NOT BUILDVP1LIGHT)
# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Database/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( GeoModelUtilities
                   src/*.cxx
                   PUBLIC_HEADERS GeoModelUtilities
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaBaseComps SGTools GeoModelInterfaces
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_test( GeoGetIds_test
  SOURCES test/GeoGetIds_test.cxx
  LINK_LIBRARIES GeoModelUtilities )

atlas_add_test( GeoVisitVolumes_test
  SOURCES test/GeoVisitVolumes_test.cxx
  LINK_LIBRARIES GeoModelUtilities )

endif()
if(BUILDVP1LIGHT)
# Declare the package's dependencies:
#atlas_depends_on_subdirs( PUBLIC
#                          DetectorDescription/GeoModel/GeoModelKernel )
                          
# External dependencies:
find_package( CLHEP )
find_package( GeoModelCore )
find_package( Eigen )

file(GLOB SOURCES src/GeoMPVEntry.cxx
            src/GeoModelExperiment.cxx
            src/GeoOpticalSurface.cxx
            src/GeoBorderSurface.cxx
            src/GeoMaterialPropertiesTable.cxx
            src/StoredAlignX.cxx
            src/GeoExtendedMaterial.cxx
            src/GeoMaterialPropertyVector.cxx
            src/GeoOpticalPhysVol.cxx
            src/StoredPhysVol.cxx)
            
# Component(s) in the package:
atlas_add_library( GeoModelUtilities
                   ${SOURCES}
                   PUBLIC_HEADERS GeoModelUtilities
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} )
endif()

# Code in this file makes heavy use of eigen and runs orders of magnitude
# more slowly without optimization.  So force this to be optimized even
# in debug builds.  If you need to debug it you might want to change this.
# Specifying optimization via an attribute on the particular
# function didn't work, because that still didn't allow inlining.
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  set_source_files_properties(
     ${CMAKE_CURRENT_SOURCE_DIR}/src/GeoVisitVolumes.cxx
     PROPERTIES
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}"
     COMPILE_DEFINITIONS "FLATTEN" )
endif()
