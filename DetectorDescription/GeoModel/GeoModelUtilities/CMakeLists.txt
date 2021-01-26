# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( GeoModelUtilities )

# External dependencies.
find_package( Eigen ) # Needed by GeoModelKernel
find_package( GeoModel COMPONENTS GeoGenericFunctions GeoModelKernel )

# The list of sources, and list of linked libraries depends on what project is
# being built.
if( BUILDVP1LIGHT )
   set( sources src/GeoMPVEntry.cxx src/GeoModelExperiment.cxx
      src/GeoOpticalSurface.cxx src/GeoBorderSurface.cxx
      src/GeoMaterialPropertiesTable.cxx src/StoredAlignX.cxx
      src/GeoExtendedMaterial.cxx src/GeoMaterialPropertyVector.cxx
      src/GeoOpticalPhysVol.cxx src/StoredPhysVol.cxx )
   set( libraries )
else()
   set( sources src/*.cxx )
   set( libraries AthenaBaseComps AthenaKernel GaudiKernel GeoModelInterfaces
      RDBAccessSvcLib )
endif()

# Component(s) in the package:
atlas_add_library( GeoModelUtilities
   GeoModelUtilities/*.h ${sources}
   PUBLIC_HEADERS GeoModelUtilities
   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
   LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODEL_LIBRARIES} ${libraries} )

atlas_add_test( GeoGetIds_test
   SOURCES test/GeoGetIds_test.cxx
   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
   LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODEL_LIBRARIES} GeoModelUtilities )

atlas_add_test( GeoVisitVolumes_test
   SOURCES test/GeoVisitVolumes_test.cxx
   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
   LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODEL_LIBRARIES} GeoModelUtilities )

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
