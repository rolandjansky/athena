################################################################################
# Package: ByteStreamTest
################################################################################

# Declare the package name:
atlas_subdir( ByteStreamTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/MinimalRunTime
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/ByteStreamCnvSvc
                          Event/ByteStreamData
                          GaudiKernel
                          TestPolicy )

# Component(s) in the package:
atlas_add_component( ByteStreamTest
                     src/EvenEventsSelectorTool.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel AthenaPoolUtilities GaudiKernel )

# Install files from the package:
atlas_install_joboptions( share/*.py )


function (bytestreamtest_run_test jo)
  cmake_parse_arguments( ARG "" "DEPENDS" "" ${ARGN} )

  set( testName "ByteStreamTest${jo}" )

  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/test/bytestreamtest_test.sh.in
                  ${CMAKE_CURRENT_BINARY_DIR}/bytestreamtest_${testName}.sh
                  @ONLY )
  atlas_add_test( ${testName}
                  SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/bytestreamtest_${testName}.sh
                  POST_EXEC_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test/post_check_bs.sh ${testName}"
                  PROPERTIES TIMEOUT 300
                   )
  if( ARG_DEPENDS )
    set_tests_properties( ByteStreamTest_${testName}_ctest
                          PROPERTIES DEPENDS ByteStreamTest_ByteStreamTest${ARG_DEPENDS}_ctest )
  endif()
endfunction (bytestreamtest_run_test)


#bytestreamtest_run_test( SkipAll )
bytestreamtest_run_test( SkipOne )
bytestreamtest_run_test( SkipNone )
bytestreamtest_run_test( MetaWrite )
bytestreamtest_run_test( MetaRead DEPENDS MetaWrite )
bytestreamtest_run_test( BadEvents )
bytestreamtest_run_test( Selectors )

