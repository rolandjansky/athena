# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RngComps )

# External dependencies:
find_package( Boost )
find_package( CLHEP )
find_package( TBB )

# Component(s) in the package:
atlas_add_component( RngComps src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES}
   AthenaBaseComps AthenaKernel StoreGateLib GaudiKernel
   AtlasCLHEP_RandomGenerators )

# Test(s) in the package:
set( RNGCOMPS_REFERENCE_TAG RngComps/RngCompsReference-01-00-03 ) 
 
atlas_add_test( AtRndmGen_test
   SOURCES test/AtRndmGen_test.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} TestTools AthenaKernel StoreGateLib
   GaudiKernel
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( AtRanlux_test
   SOURCES test/AtRanlux_test.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} TestTools AthenaKernel StoreGateLib
   GaudiKernel
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( AtDSFMT_test
   SOURCES test/AtDSFMT_test.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} TestTools AthenaKernel StoreGateLib
   GaudiKernel
   LOG_IGNORE_PATTERN "^ServiceManager *DEBUG Initializing service"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share"  )
set_property( TEST RngComps_AtDSFMT_test_ctest APPEND PROPERTY
              ENVIRONMENT "ATLAS_REFERENCE_TAG=${RNGCOMPS_REFERENCE_TAG}" )

atlas_add_test( TestSeedRunEvent
   SCRIPT test/TestSeedRunEvent.sh
   ENVIRONMENT "ATLAS_REFERENCE_TAG=${RNGCOMPS_REFERENCE_TAG}"
   PROPERTIES TIMEOUT 500 )

atlas_add_test( RNGWrapper_test
   SOURCES test/RNGWrapper_test.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${TBB_LIBRARIES} AthenaKernel AtlasCLHEP_RandomGenerators TestTools )

atlas_add_test( RandomServices_test
                SCRIPT test/RandomServices_test.py )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py test/*.py )
