# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaPoolCnvSvc )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( AthenaPoolCnvSvcLib
   AthenaPoolCnvSvc/*.h AthenaPoolCnvSvc/*.icc src/*.cxx
   PUBLIC_HEADERS AthenaPoolCnvSvc
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel AthenaPoolUtilities CxxUtils DataModelRoot GaudiKernel PersistentDataModel PoolSvcLib StorageSvc TPTools
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainersInterfaces AthContainersRoot RootUtils SGTools StoreGateLib )

atlas_add_component( AthenaPoolCnvSvc
   src/components/*.cxx
   LINK_LIBRARIES AthenaPoolCnvSvcLib )

atlas_add_dictionary( AthenaPoolCnvSvcTestDict
   test/AthenaPoolCnvSvcTestDict.h
   AthenaPoolCnvSvc/selection_test.xml
   LINK_LIBRARIES AthenaPoolCnvSvcLib
   NO_ROOTMAP_MERGE )

# Test(s) in the package:
atlas_add_test( exceptions_test
   SOURCES test/exceptions_test.cxx
   LINK_LIBRARIES AthenaPoolCnvSvcLib )

atlas_add_test( TPCnvElt_test
   SOURCES test/TPCnvElt_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib )

atlas_add_test( TPCnvList_test
   SOURCES test/TPCnvList_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib )

atlas_add_test( T_AthenaPoolCreateFuncs_test
   SOURCES test/T_AthenaPoolCreateFuncs_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib )

atlas_add_test( T_AthenaPoolViewVectorCnv_test
   SOURCES test/T_AthenaPoolViewVectorCnv_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TestTools
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( T_AthenaPoolxAODCnv_test
   SOURCES test/T_AthenaPoolxAODCnv_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TestTools
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( T_AthenaPoolAuxContainerCnv_test
   SOURCES test/T_AthenaPoolAuxContainerCnv_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TestTools
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( T_AthenaPoolTPCnvCnv_test
   SOURCES test/T_AthenaPoolTPCnvCnv_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TestTools
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( T_AuxContainerCopyTPCnv_test
   SOURCES test/T_AuxContainerCopyTPCnv_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TestTools )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
