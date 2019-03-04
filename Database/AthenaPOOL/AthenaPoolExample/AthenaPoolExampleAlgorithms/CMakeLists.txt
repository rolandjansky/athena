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

   list( APPEND _options ENVIRONMENT "ATLAS_REFERENCE_TAG=AthenaPoolExampleAlgorithms/AthenaPoolExampleAlgorithms-01-00-03" )

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
# Write 'Hits', with multistreamand
_add_test( AthenaPoolExample_Write 
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_WriteJobOptions.py"
   EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|BYTES_READ|READ_CALLS|DEBUG lookupPFN|DEBUG registered PFN|XMLCatalog +INFO|Found address:"
   PRE_EXEC test/pre_check.sh )
# Read 'Hits' and write 'Tracks'
_add_test( AthenaPoolExample_ReWrite
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RWJobOptions.py"
#   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_Append_ctest
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_Write_ctest
   EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|BYTES_READ|READ_CALLS|DEBUG lookupPFN|DEBUG registered PFN|XMLCatalog +INFO|Found address:" )
# Read all output
_add_test( AthenaPoolExample_Read
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReWrite_ctest
   EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|BYTES_READ|READ_CALLS|DEBUG lookupPFN|DEBUG registered PFN|XMLCatalog +INFO|Found address:" )
# Copy 'Hits' file without extending provenance
_add_test( AthenaPoolExample_Copy
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_CopyJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_Read_ctest
   EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|BYTES_READ|READ_CALLS|DEBUG lookupPFN|DEBUG registered PFN|XMLCatalog +INFO|Found address:" )
# Read copied 'Hits' and write 'Tracks'
_add_test( AthenaPoolExample_ReWriteAgain
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReWriteAgainJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_Copy_ctest
   EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|BYTES_READ|READ_CALLS|DEBUG lookupPFN|DEBUG registered PFN|XMLCatalog +INFO|Found address:" )
_add_test( AthenaPoolExample_ReWriteNext
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReWriteNextJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReWriteAgain_ctest
   EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|BYTES_READ|READ_CALLS|DEBUG lookupPFN|DEBUG registered PFN|XMLCatalog +INFO|Found address:" )
# Read all
_add_test( AthenaPoolExample_ReadAgain
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadAgainJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReWriteNext_ctest
   EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|BYTES_READ|READ_CALLS|DEBUG lookupPFN|DEBUG registered PFN|XMLCatalog +INFO|Found address:" )

# Concatenate jobs write 'Hits' and 'Tracks' to different streams
_add_test( AthenaPoolExample_Concat
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ConcatJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReadAgain_ctest
   EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|BYTES_READ|READ_CALLS|DEBUG lookupPFN|DEBUG registered PFN|XMLCatalog +INFO|Found address:"
   PRE_EXEC test/pre_check.sh )
_add_test( AthenaPoolExample_ReadConcat
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_ReadJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_Concat_ctest
   EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|BYTES_READ|READ_CALLS|DEBUG lookupPFN|DEBUG registered PFN|XMLCatalog +INFO|Found address:" )

# Testing 'Conditions' I/O
_add_test( AthenaPoolExample_WCond
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_WCondJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_ReadConcat_ctest
   EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|BYTES_READ|READ_CALLS|DEBUG lookupPFN|DEBUG registered PFN|XMLCatalog +INFO|Found address:" )
_add_test( AthenaPoolExample_RCond
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RCondJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_WCond_ctest
   EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|BYTES_READ|READ_CALLS|DEBUG lookupPFN|DEBUG registered PFN|XMLCatalog +INFO|Found address:" )

# Testing 'Metadata' I/O
_add_test( AthenaPoolExample_WMeta
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_WMetaJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_RCond_ctest
   EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|BYTES_READ|READ_CALLS|DEBUG lookupPFN|DEBUG registered PFN|XMLCatalog +INFO|Found address:"
   PRE_EXEC test/pre_check.sh )
_add_test( AthenaPoolExample_RMeta
   "athena.py AthenaPoolExampleAlgorithms/AthenaPoolExample_RMetaJobOptions.py"
   DEPENDS AthenaPoolExampleAlgorithms_AthenaPoolExample_WMeta_ctest
   EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|BYTES_READ|READ_CALLS|DEBUG lookupPFN|DEBUG registered PFN|XMLCatalog +INFO|Found address:" )
