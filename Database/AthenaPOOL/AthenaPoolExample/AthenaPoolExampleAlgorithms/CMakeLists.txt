# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaPoolExampleAlgorithms )

# Component(s) in the package:
atlas_add_component( AthenaPoolExampleAlgorithms
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel AthenaPoolExampleData AthenaPoolUtilities EventBookkeeperMetaData EventInfo GaudiKernel PersistentDataModel StoreGateLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Function helping to set up the integration tests
function( _add_test testName toExecute )

   # Look for possible extra arguments:
   cmake_parse_arguments( ARG "" "PRE_EXEC;IGNORE"
      "DEPENDS" ${ARGN} )

   # Create the script that will run the test:
   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/test/athenarun_test.sh.in
      ${CMAKE_CURRENT_BINARY_DIR}/${testName}_test.sh @ONLY )

   # Helper variable setting extra options on the test:
   set( _options )
   if( ARG_PRE_EXEC )
      list( APPEND _options PRE_EXEC_SCRIPT
         "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_PRE_EXEC} ${testName}" )
   endif()
   if( ARG_IGNORE )
      list( APPEND _options LOG_IGNORE_PATTERN ${ARG_IGNORE} )
   endif()

   list( APPEND _options ENVIRONMENT "ATLAS_REFERENCE_TAG=AthenaPoolExampleAlgorithms/AthenaPoolExampleAlgorithms-01-00-03" )

   # Set up the test:
   atlas_add_test( ${testName}
       SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/${testName}_test.sh
       ${_options}
       DEPENDS ${ARG_DEPENDS}
       PROPERTIES TIMEOUT 600 )

endfunction( _add_test )

# Test(s) in the package:
# ---------------------------
set( TEST_IGNORE_STR "Warning in <TFile::Init>: no StreamerInfo found|BYTES_READ|READ_CALLS|DEBUG lookupPFN|DEBUG registered PFN|XMLCatalog +INFO|Found address:|DEBUG.*ThinningCacheTool|DEBUG setAttribute|DEBUG Failed to find TTree:.*to set POOL property|DEBUG.*failed process POOL.*attributes.|metadataItemList|Creating branch for new dynamic" )

# Write 'Hits', with multistreamand
_add_test( AthenaPoolExample_Write 
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_WriteJobOptions.py"
   IGNORE ${TEST_IGNORE_STR}
   PRE_EXEC test/pre_check.sh )

# Read 'Hits' and write 'Tracks'
_add_test( AthenaPoolExample_ReWrite
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RWJobOptions.py"
   DEPENDS AthenaPoolExample_Write
   IGNORE ${TEST_IGNORE_STR} )

# Read all output
_add_test( AthenaPoolExample_Read
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadJobOptions.py"
   DEPENDS AthenaPoolExample_ReWrite
   IGNORE ${TEST_IGNORE_STR} )

# Copy 'Hits' file without extending provenance
_add_test( AthenaPoolExample_Copy
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_CopyJobOptions.py"
   DEPENDS AthenaPoolExample_Read
   IGNORE ${TEST_IGNORE_STR} )

# Read copied 'Hits' and write 'Tracks'
_add_test( AthenaPoolExample_ReWriteAgain
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReWriteAgainJobOptions.py"
   DEPENDS AthenaPoolExample_Copy
   IGNORE ${TEST_IGNORE_STR} )
_add_test( AthenaPoolExample_ReWriteNext
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReWriteNextJobOptions.py"
   DEPENDS AthenaPoolExample_ReWriteAgain
   IGNORE ${TEST_IGNORE_STR} )

# Read all
_add_test( AthenaPoolExample_ReadAgain
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadAgainJobOptions.py"
   DEPENDS AthenaPoolExample_ReWriteNext
   IGNORE ${TEST_IGNORE_STR} )

# Concatenate jobs write 'Hits' and 'Tracks' to different streams
_add_test( AthenaPoolExample_Concat
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ConcatJobOptions.py"
   DEPENDS AthenaPoolExample_ReadAgain
   IGNORE ${TEST_IGNORE_STR}
   PRE_EXEC test/pre_check.sh )
_add_test( AthenaPoolExample_ReadConcat
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadJobOptions.py"
   DEPENDS AthenaPoolExample_Concat
   IGNORE ${TEST_IGNORE_STR} )

# Testing 'Conditions' I/O
_add_test( AthenaPoolExample_WCond
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_WCondJobOptions.py"
   DEPENDS AthenaPoolExample_ReadConcat
   IGNORE ${TEST_IGNORE_STR} )

_add_test( AthenaPoolExample_RCond
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RCondJobOptions.py"
   DEPENDS AthenaPoolExample_WCond
   IGNORE ${TEST_IGNORE_STR} )

# Testing 'Metadata' I/O
_add_test( AthenaPoolExample_WMeta
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_WMetaJobOptions.py"
   DEPENDS AthenaPoolExample_RCond
   IGNORE ${TEST_IGNORE_STR}
   PRE_EXEC test/pre_check.sh )
_add_test( AthenaPoolExample_RMeta
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RMetaJobOptions.py"
   DEPENDS AthenaPoolExample_WMeta
   IGNORE ${TEST_IGNORE_STR} )
