# $Id: CMakeLists.txt 729225 2016-03-11 16:24:53Z krasznaa $
################################################################################
# Package: Navigation
################################################################################

# Declare the package name:
atlas_subdir( Navigation )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthAllocators
   Control/AthLinks
   Control/AthenaKernel
   Control/CxxUtils
   PRIVATE
   Control/AthContainers
   AtlasTest/TestTools
   Control/StoreGate
   GaudiKernel )

# External dependencies:
find_package( Boost )
find_package( UUID )

# Component(s) in the package:
atlas_add_library( Navigation Navigation/*.h Navigation/*.icc src/*.cxx
   PUBLIC_HEADERS Navigation
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${UUID_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AthAllocators AthLinks AthenaKernel
   CxxUtils 
   PRIVATE_LINK_LIBRARIES ${UUID_LIBRARIES} )

atlas_add_dictionary( NavigationDict
   Navigation/NavigationDict.h Navigation/selection.xml
   LINK_LIBRARIES Navigation )

# Test(s) in the package:
atlas_add_test( NavigationToken_test
   SOURCES test/NavigationToken_test.cxx
   LINK_LIBRARIES Navigation )

atlas_add_test( NavigableIterator_test
   SOURCES test/NavigableIterator_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} CxxUtils AthContainers TestTools 
   StoreGateLib GaudiKernel Navigation
   EXTRA_PATTERNS "JobOptionsSvc +INFO"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( AthenaBarCodeImpl_test
   SOURCES test/AthenaBarCodeImpl_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} Navigation
   EXTRA_PATTERNS "found service JobIDSvc" )
