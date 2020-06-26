# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigNavStructure )

# External dependencies:
find_package( Boost COMPONENTS regex )

# Extra dependencies, based on the build environment:
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_libs StoreGateLib )
endif()

# The library of the package:
atlas_add_library( TrigNavStructure
   TrigNavStructure/*.h Root/*.cxx
   PUBLIC_HEADERS TrigNavStructure
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AsgTools AthContainers CxxUtils xAODCore ${extra_libs} )

# Test(s) in the package:
foreach( test_name ut_iterators_test ut_build_trignav_test ut_features_test
      ut_serializer_test )
   atlas_add_test( ${test_name}
      SOURCES test/${test_name}.cxx
      LINK_LIBRARIES TrigNavStructure )
endforeach()
