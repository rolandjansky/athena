# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ByteStreamTest )

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
  atlas_add_test( ${testName}
                  SCRIPT POOL_OUTMSG_LEVEL=4 athena.py ByteStreamTest/${jo}.py
                  LOG_SELECT_PATTERN "RunEventTag|ByteStreamAtt|ByteStreamEvent|ByteStreamInput|ByteStreamOutput|EventSelector|StoreDump|StreamBSFileOutput"
                  LOG_IGNORE_PATTERN "/cvmfs|GUID|Request skipping event"
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
bytestreamtest_run_test( BadEvents )
bytestreamtest_run_test( Selectors )
