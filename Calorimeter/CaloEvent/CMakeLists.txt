# $Id: CMakeLists.txt 730170 2016-03-15 16:32:19Z krasznaa $
################################################################################
# Package: CaloEvent
################################################################################

# Declare the package name:
atlas_subdir( CaloEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Calorimeter/CaloConditions
   Calorimeter/CaloDetDescr
   Calorimeter/CaloGeoHelpers
   Calorimeter/CaloIdentifier
   Control/AthAllocators
   Control/AthContainers
   Control/AthLinks
   Control/CLIDSvc
   Control/CxxUtils
   Control/DataModel
   Control/Navigation
   DetectorDescription/GeoPrimitives
   DetectorDescription/Identifier
   Event/EventKernel
   Event/FourMom
   Event/NavFourMom
   PRIVATE
   AtlasTest/TestTools
   Control/AthenaKernel
   Control/SGTools
   Control/StoreGate
   DetectorDescription/IdDictParser
   GaudiKernel )

# External dependencies:
find_package( Boost )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( CaloEvent CaloEvent/*.h CaloEvent/*.icc src/*.cxx
   PUBLIC_HEADERS CaloEvent
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES CaloConditions CaloGeoHelpers CaloIdentifier AthAllocators
   AthContainers AthLinks CxxUtils DataModel Navigation GeoPrimitives
   Identifier EventKernel FourMom NavFourMom CaloDetDescrLib
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel
   SGTools )

atlas_add_dictionary( CaloEventDict
   CaloEvent/CaloEventDict.h CaloEvent/selection.xml
   LINK_LIBRARIES CaloEvent
   NAVIGABLES CaloCellContainer CaloClusterContainer
   DATA_LINKS CaloTowerContainer
   ELEMENT_LINKS CaloShowerContainer CaloCellLinkContainer
   CaloClusterCellLinkContainer )

# Helper variable:
set( _patterns
   "^lar decode|^HistogramPersis.* INFO|initialize_from_dict|mask/zero|^JobOptionsSvc +INFO|channel range|GP:===========|^AtlasDetectorID::" )

# Test(s) in the package:
atlas_add_test( CaloCellContainer_test
   SOURCES test/CaloCellContainer_test.cxx
   LINK_LIBRARIES CaloDetDescrLib CaloIdentifier TestTools IdDictParser
   GaudiKernel CaloEvent
   EXTRA_PATTERNS "${_patterns}"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( CaloConstCellContainer_test
   SOURCES test/CaloConstCellContainer_test.cxx
   LINK_LIBRARIES CaloDetDescrLib CaloIdentifier TestTools IdDictParser
   GaudiKernel CaloEvent
   EXTRA_PATTERNS "${_patterns}"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( CaloTowerContainer_test
   SOURCES test/CaloTowerContainer_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} TestTools CaloEvent
   EXTRA_PATTERNS "${_patterns}" )

atlas_add_test( CaloTopoTowerContainer_test
   SOURCES test/CaloTopoTowerContainer_test.cxx
   LINK_LIBRARIES TestTools StoreGateLib GaudiKernel CaloEvent
   EXTRA_PATTERNS "${_patterns}"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share:${CMAKE_JOBOPT_OUTPUT_DIRECTORY}:$ENV{JOBOPTSEARCHPATH}" )

atlas_add_test( CaloCellPrefetchIterator_test
   SOURCES test/CaloCellPrefetchIterator_test.cxx
   LINK_LIBRARIES CaloDetDescrLib CaloIdentifier TestTools IdDictParser
   GaudiKernel CaloEvent
   EXTRA_PATTERNS "${_patterns}"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( CaloSamplingData_test
   SOURCES test/CaloSamplingData_test.cxx
   LINK_LIBRARIES CaloEvent
   EXTRA_PATTERNS "${_patterns}" )

atlas_add_test( CaloTowerSeg_test
   SOURCES test/CaloTowerSeg_test.cxx
   LINK_LIBRARIES CaloEvent
   EXTRA_PATTERNS "${_patterns}" )
