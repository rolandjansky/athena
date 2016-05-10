# $Id: CMakeLists.txt 739245 2016-04-11 08:05:59Z krasznaa $
################################################################################
# Package: RootCoreUtils
################################################################################

# Declare the package name:
atlas_subdir( RootCoreUtils )

# External dependencies:
find_package( Boost COMPONENTS regex )
find_package( ROOT COMPONENTS Hist Tree RIO Core )

# Component(s) in the package:
atlas_add_library( RootCoreUtils
   RootCoreUtils/*.h Root/*.cxx
   PUBLIC_HEADERS RootCoreUtils
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES}
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

# Application(s) in the package:
atlas_add_executable( rcu_locate util/rcu_locate.cxx
   LINK_LIBRARIES RootCoreUtils )

# Test(s) in the package:
atlas_add_test( test_message_svc
   SOURCES test/test_message_svc.cxx
   LINK_LIBRARIES RootCoreUtils
   PROPERTIES WILL_FAIL TRUE )

atlas_add_test( ut_expression
   SOURCES test/ut_expression.cxx
   LINK_LIBRARIES RootCoreUtils )

atlas_add_test( ut_ran_packages_false
   SOURCES test/ut_ran_packages_false.cxx
   LINK_LIBRARIES RootCoreUtils )
