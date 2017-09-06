# $Id: CMakeLists.txt 797343 2017-02-15 15:13:34Z ssnyder $
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

atlas_add_test( MD5_test
   SOURCES test/MD5_test.cxx
   LINK_LIBRARIES CxxUtils )

atlas_add_test( bitscan_test_portable
   SOURCES test/bitscan_test.cxx
   LINK_LIBRARIES CxxUtils )

if( TARGET CxxUtils_bitscan_test_portable )
  target_compile_definitions(CxxUtils_bitscan_test_portable
    PRIVATE "-DTEST_PORTABLE" )
endif()

# Set up the "simple" tests:
foreach( test sincos_test copyif_test ArrayScanner_test Arrayrep_test
      Array_test PackedArray_test pointer_list_test FloatPacker_test
      fpcompare_test StrFormat_test
      prefetch_test ClassName_test make_unique_test ones_test
      exctrace1_test exctrace2_test bitscan_test )
   atlas_add_test( ${test}
      SOURCES test/${test}.cxx
      LINK_LIBRARIES CxxUtils )
endforeach()


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
