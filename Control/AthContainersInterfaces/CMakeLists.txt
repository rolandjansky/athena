# $Id: CMakeLists.txt 781615 2016-11-01 16:20:50Z ssnyder $
################################################################################
# Package: AthContainersInterfaces
################################################################################

# Declare the package name:
atlas_subdir( AthContainersInterfaces )

# Extra dependencies, based on the build environment:
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/SGTools )
   set( extra_libs SGTools )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   ${extra_deps} )

# External dependencies:
find_package( Boost )

# Interface library for the package:
atlas_add_library( AthContainersInterfaces
   AthContainersInterfaces/*.h AthContainersInterfaces/*.icc
   INTERFACE
   PUBLIC_HEADERS AthContainersInterfaces
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${extra_libs} )

# Test(s) in the package:
atlas_add_test( AuxStore_traits_test
   SOURCES test/AuxStore_traits_test.cxx
   LINK_LIBRARIES AthContainersInterfaces )

atlas_add_test( AuxDataOption_test
   SOURCES test/AuxDataOption_test.cxx
   LINK_LIBRARIES AthContainersInterfaces )
