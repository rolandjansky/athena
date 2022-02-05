# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AsgTesting )

# External dependencies:
find_package( Boost COMPONENTS regex )
find_package( GTest )
find_package( GMock )

# Component(s) in the package:
atlas_add_library( AsgTestingLib
   AsgTesting/*.h AsgTesting/*.icc Root/*.cxx
   PUBLIC_HEADERS AsgTesting
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES AsgTools AsgMessagingLib ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES}
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} )

atlas_add_test( gt_UnitTest_test
   SOURCES test/gt_UnitTest_test.cxx
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools AsgMessagingLib AsgTestingLib
   POST_EXEC_SCRIPT nopost.sh )
