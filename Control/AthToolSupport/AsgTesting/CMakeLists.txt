# Declare the package name:
atlas_subdir( AsgTesting )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC Control/AthToolSupport/AsgTools )

# External dependencies:
find_package( Boost COMPONENTS regex )
find_package( GTest )
find_package( GMock )

# Component(s) in the package:
atlas_add_library( AsgTestingLib
   AsgTesting/*.h AsgTesting/*.icc Root/*.cxx
   PUBLIC_HEADERS AsgTesting
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES AsgTools
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} )

atlas_add_test( gt_UnitTest_test
   SOURCES test/gt_UnitTest_test.cxx
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTestingLib
   EXTRA_PATTERNS "\([0-9]+ ms\)|\([0-9]+ ms total\)" )
