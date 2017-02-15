# $Id: CMakeLists.txt 796875 2017-02-13 15:04:31Z fwinkl $
################################################################################
# Package: CxxUtils
################################################################################

# Declare the package name:
atlas_subdir( CxxUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE AtlasTest/TestTools )

# External dependencies:
find_package( Boost COMPONENTS program_options regex filesystem thread system )

# The main library of the package:
atlas_add_library( CxxUtils
   Root/*.cxx
   PUBLIC_HEADERS CxxUtils
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES}
   PRIVATE_LINK_LIBRARIES TestTools )

# Additional libraries in the package:
atlas_add_library( exctrace_collector src/exctrace/exctrace_collector.cxx
   PUBLIC_HEADERS CxxUtils )

atlas_add_library( calg src/libcalg/*.c
   PUBLIC_HEADERS CxxUtils )

atlas_add_library( AthDSoCallBacks src/AthDsoCbk.c
   PUBLIC_HEADERS CxxUtils
   LINK_LIBRARIES calg )

# Unit tests in the package:
atlas_add_test( read_athena_statm_test
   SOURCES test/read_athena_statm_test.cxx
   LINK_LIBRARIES CxxUtils
   EXTRA_PATTERNS "read_athena_statm reports process size" )

atlas_add_test( PageAccessControl_test
   SOURCES test/PageAccessControl_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} CxxUtils )

atlas_add_test( SEGVHandler_test
   SOURCES test/SEGVHandler_test.cxx
   LINK_LIBRARIES CxxUtils
   EXTRA_PATTERNS "page fault|FIXME NOT Freeing memory" )

atlas_add_test( procmaps_test
   SOURCES test/procmaps_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} CxxUtils )

atlas_add_test( copy_bounded_test
   SOURCES test/copy_bounded_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} CxxUtils )

atlas_add_test( BitPackerUnpacker_test
   SOURCES test/BitPackerUnpacker_test.cxx
   LINK_LIBRARIES TestTools CxxUtils )

atlas_add_test( stacktrace_test
   SOURCES test/stacktrace_test.cxx
   LINK_LIBRARIES CxxUtils dl )

# Set up the "simple" tests:
foreach( test sincos_test copyif_test ArrayScanner_test Arrayrep_test
      Array_test PackedArray_test pointer_list_test FloatPacker_test
      hashtable_test fpcompare_test StrFormat_test
      prefetch_test ClassName_test make_unique_test ones_test
      exctrace1_test exctrace2_test bitscan_test )
   atlas_add_test( ${test}
      SOURCES test/${test}.cxx
      LINK_LIBRARIES CxxUtils )
endforeach()
