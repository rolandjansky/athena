# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( Navigation )

# External dependencies:
find_package( Boost )
find_package( UUID )

# Component(s) in the package:
atlas_add_library( Navigation Navigation/*.h Navigation/*.icc src/*.cxx
   PUBLIC_HEADERS Navigation
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${UUID_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AthAllocators AthLinks AthenaKernel
   PRIVATE_LINK_LIBRARIES ${UUID_LIBRARIES} GaudiKernel )

atlas_add_dictionary( NavigationDict
   Navigation/NavigationDict.h Navigation/selection.xml
   LINK_LIBRARIES Navigation )

# Test(s) in the package:
atlas_add_test( NavigationToken_test
   SOURCES test/NavigationToken_test.cxx
   LINK_LIBRARIES Navigation )

atlas_add_test( NavigableIterator_test
   SOURCES test/NavigableIterator_test.cxx
   LINK_LIBRARIES AthContainers TestTools StoreGateLib GaudiKernel Navigation
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( AthenaBarCodeImpl_test
   SOURCES test/AthenaBarCodeImpl_test.cxx
   LINK_LIBRARIES Navigation
   POST_EXEC_SCRIPT nopost.sh )
