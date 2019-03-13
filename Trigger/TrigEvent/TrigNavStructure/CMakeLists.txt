################################################################################
# Package: TrigNavStructure
################################################################################

# Declare the package name:
atlas_subdir( TrigNavStructure )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthToolSupport/AsgTools
   Control/CxxUtils )

# External dependencies:
find_package( Boost COMPONENTS regex )

# The library of the package:
atlas_add_library( TrigNavStructure
   TrigNavStructure/*.h Root/*.cxx
   PUBLIC_HEADERS TrigNavStructure
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AthContainers AsgTools )

# Test(s) in the package:
foreach( test_name ut_iterators_test ut_build_trignav_test ut_features_test
      ut_serializer_test )
   atlas_add_test( ${test_name}
      SOURCES test/${test_name}.cxx
      LINK_LIBRARIES TrigNavStructure )
endforeach()
