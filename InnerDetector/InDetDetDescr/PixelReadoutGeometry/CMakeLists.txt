# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PixelReadoutGeometry )

find_package( GTest )
find_package( GMock )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( PixelReadoutGeometryLib
  src/PixelDetectorManager.cxx
  src/PixelModuleDesign.cxx
  PUBLIC_HEADERS PixelReadoutGeometry
  LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaKernel CxxUtils GaudiKernel GeoPrimitives InDetIdentifier InDetReadoutGeometry PixelReadoutDefinitionsLib ReadoutGeometryBase TrkDetElementBase TrkSurfaces
  PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities DetDescrConditions Identifier StoreGateLib TrkEventPrimitives InDetGeoModelUtils)

atlas_add_component( PixelReadoutGeometry
  src/PixelReadoutManager.cxx
  src/ITkPixelReadoutManager.cxx
  src/components/*.cxx
  LINK_LIBRARIES AthenaBaseComps Identifier InDetIdentifier PixelReadoutGeometryLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Test(s) in the package:
atlas_add_test( PixelReadoutGeometryConfig_test
                SCRIPT test/PixelReadoutGeometryConfig_test.py
                POST_EXEC_SCRIPT nopost.sh )

if( NOT SIMULATIONBASE )
  # disabled in AthSimulation
  atlas_add_test( ITkPixelReadoutManager_test
                  SOURCES test/ITkPixelReadoutManager_test.cxx src/ITkPixelReadoutManager.cxx
                  INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS}
                  LINK_LIBRARIES ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils GaudiKernel PixelReadoutGeometryLib
                  POST_EXEC_SCRIPT nopost.sh
                  ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/test" )
endif()
