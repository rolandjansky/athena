################################################################################
# Package: AthenaPoolExampleAlgorithms
################################################################################

# Declare the package name:
atlas_subdir( AthenaPoolExampleAlgorithms )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/MinimalRunTime
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolExample/AthenaPoolExampleData
                          Database/AthenaPOOL/AthenaPoolKernel
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AthenaPOOL/DBDataModel
                          Event/ByteStreamData
                          Event/EventBookkeeperMetaData
                          Event/EventInfo
                          GaudiKernel
                          TestPolicy )

# Component(s) in the package:
atlas_add_component( AthenaPoolExampleAlgorithms
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolExampleData AthenaPoolUtilities DBDataModel ByteStreamData ByteStreamData_test EventBookkeeperMetaData EventInfo GaudiKernel )

# Install files from the package:
atlas_install_headers( AthenaPoolExampleAlgorithms )
atlas_install_joboptions( share/*.py )

# Function helping to set up the integration tests
function( _add_test testName toExecute )

   # Look for possible extra arguments:
   cmake_parse_arguments( ARG "" "POST_EXEC;PRE_EXEC;EXTRA_PATTERNS"
      "ENVIRONMENT;DEPENDS" ${ARGN} )

   # Create the script that will run the test:
   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/test/athenarun_test.sh.in
      ${CMAKE_CURRENT_BINARY_DIR}/${testName}_test.sh @ONLY )

   # Helper variable setting extra options on the test:
   set( _options )
   if( ARG_POST_EXEC )
      list( APPEND _options POST_EXEC_SCRIPT
         "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_POST_EXEC} ${testName}" )
   endif()
   if( ARG_PRE_EXEC )
      list( APPEND _options PRE_EXEC_SCRIPT
         "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_PRE_EXEC} ${testName}" )
   endif()
   if( ARG_ENVIRONMENT )
      list( APPEND _options ENVIRONMENT ${ARG_ENVIRONMENT} )
   endif()
   if( ARG_EXTRA_PATTERNS )
      list( APPEND _options EXTRA_PATTERNS ${ARG_EXTRA_PATTERNS} )
   endif()
   if( ARG_DEPENDS )
      list( APPEND _options PROPERTIES DEPENDS ${ARG_DEPENDS} )
   endif()

   # Set up the test:
   atlas_add_test( ${testName}
      SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/${testName}_test.sh
      ${_options} )

endfunction( _add_test )

# Test(s) in the package:
# Write 'Hits', with multistreamand TAGs
_add_test( AthenaPoolExample_Write 
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_WriteJobOptions.py"
   POST_EXEC test/post_check.sh PRE_EXEC test/pre_check.sh )
_add_test( AthenaPoolExample_Append
# Append to existing file
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_AppendJobOptions.py"
   DEPENDS AthenaPoolExample_Write_test_ctest
   POST_EXEC test/post_check.sh )
# Read 'Hits' and write 'Tracks'
_add_test( AthenaPoolExample_ReWrite
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RWJobOptions.py"
   DEPENDS AthenaPoolExample_Append_test_ctest
   POST_EXEC test/post_check.sh )
# Read all output
_add_test( AthenaPoolExample_Read
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadJobOptions.py"
   DEPENDS AthenaPoolExample_ReWrite_test_ctest
   POST_EXEC test/post_check.sh )
# Read all output via TAGs
_add_test( AthenaPoolExample_ReadTag
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadTagJobOptions.py"
   DEPENDS AthenaPoolExample_ReWrite_test_ctest
   POST_EXEC test/post_check.sh )
# Read all output including scoped BackNavigation
_add_test( AthenaPoolExample_ReadBN
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadBNJobOptions.py"
   DEPENDS AthenaPoolExample_ReWrite_test_ctest
   POST_EXEC test/post_check.sh )
# Read all output w/o BackNavigation
_add_test( AthenaPoolExample_ReadNoBN
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadNoBNJobOptions.py"
   DEPENDS AthenaPoolExample_ReWrite_test_ctest
   POST_EXEC test/post_check.sh )

# Copy 'Hits' file without extending provenance
_add_test( AthenaPoolExample_Copy
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_CopyJobOptions.py"
   DEPENDS AthenaPoolExample_Read_test_ctest
   DEPENDS AthenaPoolExample_ReadTag_test_ctest
   DEPENDS AthenaPoolExample_ReadBN_test_ctest
   DEPENDS AthenaPoolExample_ReadNoBN_test_ctest
   POST_EXEC test/post_check.sh )
# Read copied 'Hits' and write 'Tracks'
_add_test( AthenaPoolExample_ReWriteAgain
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReWriteAgainJobOptions.py"
   DEPENDS AthenaPoolExample_Copy_test_ctest
   POST_EXEC test/post_check.sh )
_add_test( AthenaPoolExample_ReWriteNext
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReWriteNextJobOptions.py"
   DEPENDS AthenaPoolExample_ReWriteAgain_test_ctest
   POST_EXEC test/post_check.sh )
# Read via TAGs and use query to filter events
_add_test( AthenaPoolExample_Filter
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_FilterJobOptions.py"
   DEPENDS AthenaPoolExample_ReWriteNext_test_ctest
   POST_EXEC test/post_check.sh )
_add_test( AthenaPoolExample_RFilter
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RFilterJobOptions.py"
   DEPENDS AthenaPoolExample_Filter_test_ctest
   POST_EXEC test/post_check.sh )
# Read all (including bad files, skipped for now expected failure)
_add_test( AthenaPoolExample_ReadAgain
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadAgainJobOptions.py"
   DEPENDS AthenaPoolExample_RFilter_test_ctest
   POST_EXEC test/post_check.sh )

# Concatenate jobs write 'Hits' and 'Tracks' to different streams
_add_test( AthenaPoolExample_Concat
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ConcatJobOptions.py"
   DEPENDS AthenaPoolExample_RFilter_test_ctest
   POST_EXEC test/post_check.sh PRE_EXEC test/pre_check.sh )
_add_test( AthenaPoolExample_ReadConcat
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadJobOptions.py"
   DEPENDS AthenaPoolExample_Concat_test_ctest
   POST_EXEC test/post_check.sh )

# Read ByteStream and write to APR/POOL
_add_test( AthenaPoolExample_RWBs
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RWBsJobOptions.py"
   DEPENDS AthenaPoolExample_ReadConcat_test_ctest
   POST_EXEC test/post_check.sh )
_add_test( AthenaPoolExample_ReadBs
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadBsJobOptions.py"
   DEPENDS AthenaPoolExample_RWBs_test_ctest
   POST_EXEC test/post_check.sh )
# Read ByteStream via TAGs
_add_test( AthenaPoolExample_ReadBsTag
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadBsTagJobOptions.py"
   DEPENDS AthenaPoolExample_ReadBs_test_ctest
   POST_EXEC test/post_check.sh PRE_EXEC test/pre_check.sh )

# Read compressed ByteStream and write to APR/POOL
_add_test( AthenaPoolExample_RWcBs
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RWcBsJobOptions.py"
   DEPENDS AthenaPoolExample_ReadBsTag_test_ctest
   POST_EXEC test/post_check.sh PRE_EXEC test/pre_check.sh )
_add_test( AthenaPoolExample_ReadcBs
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadBsJobOptions.py"
   DEPENDS AthenaPoolExample_RWcBs_test_ctest
   POST_EXEC test/post_check.sh )
# Read compressed ByteStream via TAGs
#_add_test( AthenaPoolExample_ReadcBsTag
#   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadBsTagJobOptions.py"
#   DEPENDS AthenaPoolExample_ReadcBs_test_ctest
#   POST_EXEC test/post_check.sh )

# Testing 'Conditions' I/O
_add_test( AthenaPoolExample_WCond
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_WCondJobOptions.py"
   DEPENDS AthenaPoolExample_ReadcBs_test_ctest
   POST_EXEC test/post_check.sh )
_add_test( AthenaPoolExample_ACond
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ACondJobOptions.py"
   DEPENDS AthenaPoolExample_WCond_test_ctest
   POST_EXEC test/post_check.sh )
_add_test( AthenaPoolExample_RCond
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RCondJobOptions.py"
   DEPENDS AthenaPoolExample_ACond_test_ctest
   POST_EXEC test/post_check.sh )

# Testing 'Metadata' I/O
_add_test( AthenaPoolExample_WMeta
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_WMetaJobOptions.py"
   DEPENDS AthenaPoolExample_RCond_test_ctest
   POST_EXEC test/post_check.sh PRE_EXEC test/pre_check.sh )
_add_test( AthenaPoolExample_AMeta
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_AMetaJobOptions.py"
   DEPENDS AthenaPoolExample_WMeta_test_ctest
   POST_EXEC test/post_check.sh )
_add_test( AthenaPoolExample_RMeta
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RMetaJobOptions.py"
   DEPENDS AthenaPoolExample_AMeta_test_ctest
   POST_EXEC test/post_check.sh )

# Testing APR/POOL 'fast' merge
_add_test( AthenaPoolExample_WriteFast
   "athena.py  AthenaPoolExampleAlgorithms/AthenaPoolExample_WriteFastJobOptions.py"
   DEPENDS AthenaPoolExample_RMeta_test_ctest
   POST_EXEC test/post_check.sh )
_add_test( AthenaPoolExample_AppendFast
   "athena.py  AthenaPoolExampleAlgorithms/AthenaPoolExample_AppendFastJobOptions.py"
   DEPENDS AthenaPoolExample_WriteFast_test_ctest
   POST_EXEC test/post_check.sh )
_add_test( AthenaPoolExample_ReWriteFast
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RWJobOptions.py"
   DEPENDS AthenaPoolExample_AppendFast_test_ctest
   POST_EXEC test/post_check.sh )
_add_test( AthenaPoolExample_ReadFast
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadJobOptions.py"
   DEPENDS AthenaPoolExample_ReWriteFast_test_ctest
   POST_EXEC test/post_check.sh )
#_add_test( AthenaPoolExample_ReWriteTagFast
#   "athena.py ./AthenaPoolExample_ReWriteTagFast.py"
#   POST_EXEC test/post_check.sh )
#_add_test( AthenaPoolExample_ReadTagFast
#   "athena.py ./AthenaPoolExample_ReadTagFast.py"
#   POST_EXEC test/post_check.sh )

