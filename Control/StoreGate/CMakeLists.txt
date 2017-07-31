# $Id: CMakeLists.txt 795192 2017-02-02 17:18:15Z ssnyder $
################################################################################
# Package: StoreGate
################################################################################

# Declare the package name:
atlas_subdir( StoreGate )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthAllocators
   Control/AthContainersInterfaces
   Control/AthContainers
   Control/AthenaKernel
   Control/SGTools
   Database/PersistentDataModel
   GaudiKernel
   PRIVATE
   AtlasTest/TestTools
   Control/CxxUtils )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase )
find_package( TBB )

# Component(s) in the package:
atlas_add_library( StoreGateLib src/*.cxx
   PUBLIC_HEADERS StoreGate
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${TBB_LIBRARIES} ${CORAL_LIBRARIES} AthAllocators AthenaKernel
   SGTools GaudiKernel
   PRIVATE_LINK_LIBRARIES CxxUtils TestTools )

atlas_add_component( StoreGate src/components/*.cxx
   LINK_LIBRARIES GaudiKernel StoreGateLib )

# Test library used by the package's unit tests:
atlas_add_library( SGtests test/SGtests.cxx
   NO_PUBLIC_HEADERS
   LINK_LIBRARIES StoreGateLib TestTools )

# Declare the package's tests:
atlas_add_test( ActiveStore_test
   SOURCES test/ActiveStore_test.cxx
   LINK_LIBRARIES SGtests
   EXTRA_PATTERNS "JobOptionsSvc +INFO"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

   set( _hivePatterns "JobOptionsSvc +INFO|StoreGateSvc.*DEBUG|HiveMgrSvc +DEBUG" )

atlas_add_test( SGTiming_test
   SOURCES test/SGTiming_test.cxx
   LINK_LIBRARIES SGtests
   EXTRA_PATTERNS "${_hivePatterns}|rec:|averages"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( SGHive_test
   SOURCES test/SGHive_test.cxx
   LINK_LIBRARIES SGtests
   EXTRA_PATTERNS "${_hivePatterns}"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( DataHandle_test
   SOURCES test/DataHandle_test.cxx
   LINK_LIBRARIES SGtests
   EXTRA_PATTERNS "ClassIDSvc Initialized successfully|^HistogramPersis.* INFO" )

atlas_add_test( SGIterator_test
   SOURCES test/SGIterator_test.cxx
   LINK_LIBRARIES SGtests
   EXTRA_PATTERNS "^HistogramPersis.* INFO" )

atlas_add_test( KeyConcept_test
   SOURCES test/KeyConcept_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( StoreClearedIncident_test
   SOURCES test/StoreClearedIncident_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( SegMemSvc_test
   SOURCES test/SegMemSvc_test.cxx
   LINK_LIBRARIES SGtests
   EXTRA_PATTERNS "^HistogramPersis.* INFO" )

atlas_add_test( exceptions_test
   SOURCES test/exceptions_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( VarHandleKey_parseKey_test
   SOURCES test/VarHandleKey_parseKey_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( VarHandleKey_test
   SOURCES test/VarHandleKey_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( VarHandleKeyProperty_test
   SOURCES test/VarHandleKeyProperty_test.cxx
   LINK_LIBRARIES SGtests
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( VarHandleProperty_test
   SOURCES test/VarHandleProperty_test.cxx
   LINK_LIBRARIES SGtests
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( ReadHandleKey_test
   SOURCES test/ReadHandleKey_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( UpdateHandleKey_test
   SOURCES test/UpdateHandleKey_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( VarHandleBase_test
   SOURCES test/VarHandleBase_test.cxx
   LINK_LIBRARIES SGtests
   EXTRA_PATTERNS "could not get proxy for|try using a ReadHandle"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( VarHandles_test
   SOURCES test/VarHandles_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( WriteHandle_test
   SOURCES test/WriteHandle_test.cxx
   LINK_LIBRARIES SGtests AthContainers
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( ReadHandle_test
   SOURCES test/ReadHandle_test.cxx
   LINK_LIBRARIES SGtests
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( UpdateHandle_test
   SOURCES test/UpdateHandle_test.cxx
   LINK_LIBRARIES SGtests
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( DecorKeyHelpers_test
   SOURCES test/DecorKeyHelpers_test.cxx
   LINK_LIBRARIES StoreGateLib )

atlas_add_test( ReadDecorHandleKey_test
   SOURCES test/ReadDecorHandleKey_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
   LINK_LIBRARIES StoreGateLib TestTools
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( WriteDecorHandleKey_test
   SOURCES test/WriteDecorHandleKey_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
   LINK_LIBRARIES StoreGateLib TestTools
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( ReadDecorHandle_test
   SOURCES test/ReadDecorHandle_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
   LINK_LIBRARIES StoreGateLib AthContainers TestTools
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( WriteDecorHandle_test
   SOURCES test/WriteDecorHandle_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
   LINK_LIBRARIES StoreGateLib AthContainers TestTools
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/StoreGate_jobOptions.txt
   share/StoreGate_jobOptions.py
   share/SGHive_test.txt
   share/SGTiming_test.txt
   share/ActiveStore_test.txt )

