# $Id: CMakeLists.txt 777942 2016-10-12 01:14:44Z krasznaa $
################################################################################
# Package: TrigConfBase
################################################################################

# Declare the package name:
atlas_subdir( TrigConfBase )

# External dependencies:
find_package( Boost COMPONENTS regex )

# Component(s) in the package:
atlas_add_library( TrigConfBase
   TrigConfBase/*.h src/*.cxx Root/*.cxx
   PUBLIC_HEADERS TrigConfBase
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} )

atlas_add_executable( trigconf_msgsvc_test
   test/trigconf_msgsvc_test.cxx
   LINK_LIBRARIES TrigConfBase )
