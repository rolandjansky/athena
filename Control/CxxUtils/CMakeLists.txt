# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CxxUtils )

# External dependencies:
find_package( Boost COMPONENTS program_options regex filesystem thread system timer unit_test_framework )
find_package( TBB )

# The main library of the package:
atlas_add_library( CxxUtils
   Root/*.cxx
   PUBLIC_HEADERS CxxUtils
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES}
   PRIVATE_LINK_LIBRARIES ${CMAKE_DL_LIBS} TestTools )

# Additional libraries in the package:
atlas_add_library( exctrace_collector src/exctrace/exctrace_collector.cxx
   PUBLIC_HEADERS CxxUtils
   PRIVATE_LINK_LIBRARIES ${CMAKE_DL_LIBS} )

atlas_add_library( calg src/libcalg/*.c
   PUBLIC_HEADERS CxxUtils )

atlas_add_library( AthDSoCallBacks src/AthDsoCbk.c
   PUBLIC_HEADERS CxxUtils
   LINK_LIBRARIES calg
   PRIVATE_LINK_LIBRARIES ${CMAKE_DL_LIBS} )

# Unit tests in the package:
atlas_add_test( read_athena_statm_test
   SOURCES test/read_athena_statm_test.cxx
   LINK_LIBRARIES CxxUtils
   LOG_IGNORE_PATTERN "read_athena_statm reports process size" )

atlas_add_test( PageAccessControl_test
   SOURCES test/PageAccessControl_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} CxxUtils )

atlas_add_test( SEGVHandler_test
   SOURCES test/SEGVHandler_test.cxx
   LINK_LIBRARIES CxxUtils
   LOG_IGNORE_PATTERN "page fault|FIXME NOT Freeing memory" )

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

if( NOT XAOD_STANDALONE )
   atlas_add_test( stacktrace_test
      SOURCES test/stacktrace_test.cxx
      LINK_LIBRARIES CxxUtils ${CMAKE_DL_LIBS} )
endif()

atlas_add_test( MD5_test
   SOURCES test/MD5_test.cxx
   LINK_LIBRARIES CxxUtils )

atlas_add_test( bitscan_test_portable
   SOURCES test/bitscan_test.cxx
   LINK_LIBRARIES CxxUtils )

atlas_add_test( ut_cxxutils_floatcompression_test
   SOURCES test/ut_cxxutils_floatcompression_test.cxx
   LINK_LIBRARIES CxxUtils )

if( TARGET CxxUtils_bitscan_test_portable )
  target_compile_definitions(CxxUtils_bitscan_test_portable
    PRIVATE "-DTEST_PORTABLE" )
endif()

if ( IS_DIRECTORY /usr/include/ck )
  set( CK_LIBRARIES ck )
  set( CK_INCLUDE /usr/include/ck )
else()
  set( CK_LIBRARIES )
  set( CK_INCLUDE )
endif()
atlas_add_test( ConcurrentBitset_test
  SOURCES test/ConcurrentBitset_test.cxx
  INCLUDE_DIRS ${TBB_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CK_INCLUDE}
  LINK_LIBRARIES CxxUtils TestTools ${TBB_LIBRARIES} ${Boost_LIBRARIES} ${CK_LIBRARIES} )
if ( IS_DIRECTORY /usr/include/ck )
  target_compile_definitions(CxxUtils_ConcurrentBitset_test
    PRIVATE "-DHAVE_CK" )
endif()

atlas_add_test( exctrace2_test
   SOURCES test/exctrace2_test.cxx
   LINK_LIBRARIES CxxUtils
   LOG_IGNORE_PATTERN "no version information available" )

atlas_add_test(
   OptionalContainer_test
   SOURCES test/OptionalContainer_test.cxx
   LINK_LIBRARIES CxxUtils
)

# Set up the "simple" tests:
foreach( test sincos_test ArrayScanner_test Arrayrep_test
      Array_test PackedArray_test pointer_list_test FloatPacker_test
      fpcompare_test StrFormat_test phihelper_test
      prefetch_test ClassName_test ones_test
      exctrace1_test bitscan_test ConcurrentRangeMap_test
      CachedValue_test CachedPointer_test CachedUniquePtr_test
      atomic_fetch_minmax_test
      MurmurHash2_test bitmask_test crc64_test Ring_test
      restrict_test vectorize_test get_unaligned_test aligned_vector_test
      vec_int_test vec_float_test vec_fb_int_test vec_fb_float_test)
   atlas_add_test( ${test}
      SOURCES test/${test}.cxx
      LOG_IGNORE_PATTERN "no version information available"
      LINK_LIBRARIES CxxUtils TestTools )
endforeach()
# Needed because this executable directly uses multiversioned functions.
set_property( TARGET CxxUtils_crc64_test PROPERTY POSITION_INDEPENDENT_CODE ON )

# If we are in release rebuilding mode, stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Install the ubsan.supp file already during the CMake configuration.
# This is necessary because the file may be needer very early during the
# build, and we don't explicitly make everything else wait for this
# file's installation. Creating a race condition.
file( INSTALL share/ubsan.supp
   DESTINATION ${CMAKE_SHARE_OUTPUT_DIRECTORY} )
install( FILES share/ubsan.supp
   DESTINATION ${CMAKE_INSTALL_SHAREDIR} )

# Set the directory to put the configured environment setup file into:
set( CxxUtilsEnvironment_DIR
   ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}
   CACHE PATH "Location of CxxUtilsEnvironmentConfig.cmake" )

# Configure the environment setup module:
configure_file(
   ${CMAKE_CURRENT_SOURCE_DIR}/CxxUtilsEnvironmentConfig.cmake.in
   ${CxxUtilsEnvironment_DIR}/CxxUtilsEnvironmentConfig.cmake
   @ONLY )

# And now "find" it:
find_package( CxxUtilsEnvironment REQUIRED )
