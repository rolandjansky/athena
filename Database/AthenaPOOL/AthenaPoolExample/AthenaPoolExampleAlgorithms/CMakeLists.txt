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
      "DEPENDS" ${ARGN} )

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
   if( ARG_EXTRA_PATTERNS )
      list( APPEND _options EXTRA_PATTERNS ${ARG_EXTRA_PATTERNS} )
   endif()

   list( APPEND _options ENVIRONMENT "ATLAS_REFERENCE_TAG=AthenaPoolExampleAlgorithms/AthenaPoolExampleAlgorithms-01-00-01" )

   # Set up the test:
   atlas_add_test( ${testName}
       SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/${testName}_test.sh
       ${_options} )

   if( ARG_DEPENDS )
      set_tests_properties( AthenaPoolExampleAlgorithms_${testName}_ctest
                            PROPERTIES DEPENDS "${ARG_DEPENDS}" )
   endif()
   set_tests_properties( AthenaPoolExampleAlgorithms_${testName}_ctest
                         PROPERTIES TIMEOUT 600 )

endfunction( _add_test )

# Test(s) in the package:
# Write 'Hits', with multistreamand TAGs
_add_test( AthenaPoolExample_Write 
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_WriteJobOptions.py"
   EXTRA_PATTERNS "RootCollection DEBUG File|found catalog guid|from the catalog"
   PRE_EXEC test/pre_check.sh )
# Append to existing file
_add_test( AthenaPoolExample_Append
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_AppendJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_Write_ctest )
# Read 'Hits' and write 'Tracks'
_add_test( AthenaPoolExample_ReWrite
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RWJobOptions.py"
   EXTRA_PATTERNS "RootCollection DEBUG File|found catalog guid|from the catalog"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_Append_ctest )
# Read all output
_add_test( AthenaPoolExample_Read
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadJobOptions.py"
   EXTRA_PATTERNS "BYTES_READ"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReWrite_ctest )
# Read all output via TAGs
_add_test( AthenaPoolExample_ReadTag
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadTagJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReWrite_ctest )
# Read all output including scoped BackNavigation
_add_test( AthenaPoolExample_ReadBN
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadBNJobOptions.py"
   EXTRA_PATTERNS "BYTES_READ"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReWrite_ctest )
# Read all output w/o BackNavigation
_add_test( AthenaPoolExample_ReadNoBN
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadNoBNJobOptions.py"
   EXTRA_PATTERNS "BYTES_READ"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReWrite_ctest )

# Copy 'Hits' file without extending provenance
_add_test( AthenaPoolExample_Copy
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_CopyJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_Read_ctest
           AthenaPoolExampleAlgorithms_AthenaPoolExample_ReadTag_ctest
           AthenaPoolExampleAlgorithms_AthenaPoolExample_ReadBN_ctest
           AthenaPoolExampleAlgorithms_AthenaPoolExample_ReadNoBN_ctest )
# Read copied 'Hits' and write 'Tracks'
_add_test( AthenaPoolExample_ReWriteAgain
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReWriteAgainJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_Copy_ctest )
_add_test( AthenaPoolExample_ReWriteNext
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReWriteNextJobOptions.py"
   EXTRA_PATTERNS "RootCollection DEBUG File|found catalog guid|from the catalog"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReWriteAgain_ctest )
# Read via TAGs and use query to filter events
_add_test( AthenaPoolExample_Filter
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_FilterJobOptions.py"
   EXTRA_PATTERNS "RootCollection DEBUG File|found catalog guid|from the catalog"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReWriteNext_ctest )
_add_test( AthenaPoolExample_RFilter
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RFilterJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_Filter_ctest )
# Read all (including bad files, skipped for now expected failure)
_add_test( AthenaPoolExample_ReadAgain
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadAgainJobOptions.py"
   EXTRA_PATTERNS "Deaccess DbContainer|Database being retired|BYTES_READ|endTransaction|Bytes:"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_RFilter_ctest )

# Concatenate jobs write 'Hits' and 'Tracks' to different streams
_add_test( AthenaPoolExample_Concat
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ConcatJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_RFilter_ctest
           AthenaPoolExampleAlgorithms_AthenaPoolExample_ReadAgain_ctest
   PRE_EXEC test/pre_check.sh )
_add_test( AthenaPoolExample_ReadConcat
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadJobOptions.py"
   EXTRA_PATTERNS "BYTES_READ"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_Concat_ctest )

# Read ByteStream and write to APR/POOL
_add_test( AthenaPoolExample_RWBs
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RWBsJobOptions.py"
   EXTRA_PATTERNS "BSF:|Picked valid file:|RootCollection DEBUG File|found catalog guid|from the catalog"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReadConcat_ctest )
_add_test( AthenaPoolExample_ReadBs
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadBsJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_RWBs_ctest )
# Read ByteStream via TAGs
#_add_test( AthenaPoolExample_ReadBsTag
#   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadBsTagJobOptions.py"
#   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReadBs_ctest
#   PRE_EXEC test/pre_check.sh )

# Read compressed ByteStream and write to APR/POOL
_add_test( AthenaPoolExample_RWcBs
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RWcBsJobOptions.py"
#   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReadBsTag_ctest
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReadBs_ctest
   EXTRA_PATTERNS "Events to copy|Checking file|Created DataWriter|File:|Event index:|^Run:|Event ID:|^LumiBlock:|Wrote 3 events"
   PRE_EXEC test/pre_check.sh )
_add_test( AthenaPoolExample_ReadcBs
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadBsJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_RWcBs_ctest )
# Read compressed ByteStream via TAGs
#_add_test( AthenaPoolExample_ReadcBsTag
#   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadBsTagJobOptions.py"
#   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReadcBs_ctest
#   POST_EXEC test/post_check.sh )

# Testing 'Conditions' I/O
_add_test( AthenaPoolExample_WCond
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_WCondJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReadcBs_ctest )
_add_test( AthenaPoolExample_ACond
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ACondJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_WCond_ctest )
_add_test( AthenaPoolExample_RCond
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RCondJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ACond_ctest )

# Testing 'Metadata' I/O
_add_test( AthenaPoolExample_WMeta
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_WMetaJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_RCond_ctest
   PRE_EXEC test/pre_check.sh )
_add_test( AthenaPoolExample_AMeta
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_AMetaJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_WMeta_ctest )
_add_test( AthenaPoolExample_RMeta
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RMetaJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_AMeta_ctest
   EXTRA_PATTERNS "Bytes:" )

# Testing APR/POOL 'fast' merge
_add_test( AthenaPoolExample_WriteFast
   "athena.py  AthenaPoolExampleAlgorithms/AthenaPoolExample_WriteFastJobOptions.py"
   EXTRA_PATTERNS "RootCollection DEBUG File|found catalog guid|from the catalog"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_RMeta_ctest )
_add_test( AthenaPoolExample_AppendFast
   "athena.py  AthenaPoolExampleAlgorithms/AthenaPoolExample_AppendFastJobOptions.py"
   EXTRA_PATTERNS "RootCollection DEBUG File|found catalog guid|from the catalog|DbContainer|Reading Shape|Reading Assoc|Reading Param|Adding Assoc|Adding Shape|#Elements|->ClassID:|Pushing back ref|Building shape|DataHeader|No objects passing|EventInfo|INFO Token|Class:unsigned int|EventStreamInfo|Class:Token|INFO unsigned int"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_WriteFast_ctest )
#_add_test( AthenaPoolExample_ReWriteFast
#   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RWJobOptions.py"
#   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_AppendFast_ctest )
#_add_test( AthenaPoolExample_ReadFast
#   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadJobOptions.py"
#   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReWriteFast_ctest )
#_add_test( AthenaPoolExample_ReWriteTagFast
#   "athena.py ./AthenaPoolExample_ReWriteTagFast.py"
#   POST_EXEC test/post_check.sh )
#_add_test( AthenaPoolExample_ReadTagFast
#   "athena.py ./AthenaPoolExample_ReadTagFast.py"
#   POST_EXEC test/post_check.sh )

