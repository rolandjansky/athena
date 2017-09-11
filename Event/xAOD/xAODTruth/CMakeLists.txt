################################################################################
# Package: xAODTruth
################################################################################

# Declare the package name:
atlas_subdir( xAODTruth )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   Event/xAOD/xAODBase
   Event/xAOD/xAODCore
   PRIVATE
   Generators/TruthUtils )

# Component(s) in the package:
atlas_add_library( xAODTruth
   xAODTruth/*.h xAODTruth/versions/*.h Root/*.h Root/*.cxx
   PUBLIC_HEADERS xAODTruth
   LINK_LIBRARIES AthContainers AthLinks xAODBase xAODCore
   PRIVATE_LINK_LIBRARIES TruthUtils )

atlas_add_dictionary( xAODTruthDict
   xAODTruth/xAODTruthDict.h
   xAODTruth/selection.xml
   LINK_LIBRARIES xAODTruth
   EXTRA_FILES Root/dict/*.cxx )

atlas_generate_cliddb( xAODTruth )

# Test(s) in the package:
atlas_add_test( ut_xaodtruth_particle_test
   SOURCES test/ut_xaodtruth_particle_test.cxx
   LINK_LIBRARIES xAODTruth )

if( XAOD_STANDALONE )
   atlas_add_test( ut_xaodtruth_helpers_test
      SCRIPT test/ut_xaodtruth_helpers_test.py )
endif()
