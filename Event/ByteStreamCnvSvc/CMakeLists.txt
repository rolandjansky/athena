# $Id: CMakeLists.txt 768274 2016-08-16 19:05:07Z ssnyder $
################################################################################
# Package: ByteStreamCnvSvc
################################################################################

# Declare the package name:
atlas_subdir( ByteStreamCnvSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   Event/ByteStreamCnvSvcBase
   Event/ByteStreamData
   GaudiKernel
   PRIVATE
   Control/AthenaKernel
   Control/SGTools
   Control/StoreGate
   Database/APR/CollectionBase
   Database/APR/POOLCore
   Database/APR/FileCatalog
   Database/AthenaPOOL/AthenaPoolKernel
   Database/AthenaPOOL/AthenaPoolUtilities
   Database/PersistentDataModel
   Event/EventInfo
   Event/ByteStreamCnvSvcLegacy )

# External dependencies:
find_package( Boost COMPONENTS system )
find_package( CORAL COMPONENTS CoralBase )
find_package( tdaq-common COMPONENTS eformat_old eformat_write RawFileName
   DataReader DataWriter )

# Libraries in the package:
atlas_add_library( ByteStreamCnvSvcLib
   ByteStreamCnvSvc/*.h src/*.cxx
   PUBLIC_HEADERS ByteStreamCnvSvc
   PRIVATE_INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES AthenaBaseComps ByteStreamData GaudiKernel
   ByteStreamCnvSvcBaseLib StoreGateLib rt
   PRIVATE_LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${Boost_LIBRARIES}
   AthenaKernel SGTools CollectionBase POOLCore FileCatalog
   AthenaPoolUtilities PersistentDataModel EventInfo
   ByteStreamCnvSvcLegacy )

atlas_add_component( ByteStreamCnvSvc
   src/components/*.cxx
   LINK_LIBRARIES ByteStreamCnvSvcLib )

# Executables in the package:
atlas_add_executable( AtlFindBSEvent test/AtlFindBSEvent.cxx
   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${Boost_LIBRARIES} )

atlas_add_executable( AtlCopyBSEvent test/AtlCopyBSEvent.cxx
   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
   ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${CORAL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${Boost_LIBRARIES}
   CollectionBase POOLCore FileCatalog PersistentDataModel )

atlas_add_executable( AtlListBSEvents test/AtlListBSEvents.cxx
   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${Boost_LIBRARIES} )

# Function helping to set up the integration tests
function( _add_test testName toExecute )

   # Look for possible extra arguments:
   cmake_parse_arguments( ARG "" "POST_EXEC;EXTRA_PATTERNS"
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
_add_test( BSEventSelector
   "athena.py ByteStreamCnvSvc/BSEventSelector_test_jobOptions.py"
   POST_EXEC test/post_check.sh )

_add_test( AtlCopyBSEvent1_test
   "AtlCopyBSEvent -e 14350,14356,14382 -o test.data /afs/cern.ch/atlas/offline/test/daq.m4_combined.0020720.extract.L1TT-b00000010._0001.data"
   DEPENDS ByteStreamCnvSvc_BSEventSelector_ctest )
_add_test( AtlFindBSEvent2_test
   "AtlFindBSEvent -e 14356 /afs/cern.ch/atlas/offline/test/daq.m4_combined.0020720.extract.L1TT-b00000010._0001.data"
   DEPENDS ByteStreamCnvSvc_AtlCopyBSEvent1_test_ctest )
_add_test( AtlCopyBSEvent3_test
   "AtlCopyBSEvent -d -e 14350,14356,14382 -o test_defl.data test.data"
   DEPENDS ByteStreamCnvSvc_AtlCopyBSEvent1_test_ctest )
_add_test( AtlFindBSEvent4_test
   "AtlFindBSEvent -e 14382 test_defl.data"
   EXTRA_PATTERNS "+Timestamp"
   DEPENDS ByteStreamCnvSvc_AtlCopyBSEvent3_test_ctest )
_add_test( AtlCopyBSEvent5_test
   "AtlCopyBSEvent -e 14350,14356,14382 -o test_infl.data test_defl.data"
   DEPENDS ByteStreamCnvSvc_AtlFindBSEvent4_test_ctest )
# Input file has vanished.
#_add_test( AtlCopyBSEvent6_test
#   "AtlCopyBSEvent -e all -o empty.data /afs/cern.ch/atlas/maxidisk/d108/cranshaw/nightlies/RAW.01524408._005549.data.1"
#   DEPENDS ByteStreamCnvSvc_AtlFindBSEvent4_test_ctest )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/catalogBytestreamFiles.sh )
