# $Id: CMakeLists.txt 739578 2016-04-12 07:34:46Z krasznaa $
################################################################################
# Package: AthenaBaseComps
################################################################################

# Declare the package name:
atlas_subdir( AthenaBaseComps )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaKernel
   Control/CxxUtils
   Control/SGTools
   Control/StoreGate
   GaudiKernel
   PRIVATE
   AtlasTest/TestTools )

# External dependencies:
find_package( ROOT COMPONENTS Hist Tree Core )

# Component(s) in the package:
atlas_add_library( AthenaBaseComps
   AthenaBaseComps/*.h src/*.cxx
   PUBLIC_HEADERS AthenaBaseComps
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel CxxUtils SGTools GaudiKernel
   StoreGateLib )

# Test(s) in the package:
atlas_add_test( propertyHandling_test
   SOURCES test/propertyHandling_test.cxx
   LINK_LIBRARIES StoreGateLib TestTools AthenaBaseComps 
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( AthReentrantAlgorithm_test
   SOURCES test/AthReentrantAlgorithm_test.cxx
   LINK_LIBRARIES StoreGateLib GaudiKernel TestTools AthenaBaseComps 
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( AthAlgorithm_test
   SOURCES test/AthAlgorithm_test.cxx
   LINK_LIBRARIES StoreGateLib GaudiKernel TestTools AthenaBaseComps 
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( AthAlgTool_test
   SOURCES test/AthAlgTool_test.cxx
   LINK_LIBRARIES StoreGateLib GaudiKernel TestTools AthenaBaseComps 
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( AthAlgorithmDHUpdate_test
   SOURCES test/AthAlgorithmDHUpdate_test.cxx
   LINK_LIBRARIES StoreGateLib GaudiKernel TestTools AthenaBaseComps
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )
