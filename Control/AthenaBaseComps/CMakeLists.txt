# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaBaseComps )

# External dependencies:
find_package( Boost COMPONENTS thread )
find_package( ROOT COMPONENTS Hist Tree Core )

# Component(s) in the package:
atlas_add_library( AthenaBaseComps
   AthenaBaseComps/*.h src/*.cxx
   PUBLIC_HEADERS AthenaBaseComps
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils xAODEventInfo GaudiKernel StoreGateLib )

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

 atlas_add_test( AthAlgStartVisitor_test
   SOURCES test/AthAlgStartVisitor_test.cxx
   LINK_LIBRARIES StoreGateLib GaudiKernel TestTools AthenaBaseComps
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )
