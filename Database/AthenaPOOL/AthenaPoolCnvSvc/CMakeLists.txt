# $Id: CMakeLists.txt 790790 2016-12-21 22:31:42Z gemmeren $
################################################################################
# Package: AthenaPoolCnvSvc
################################################################################

# Declare the package name:
atlas_subdir( AthenaPoolCnvSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/CxxUtils
   Control/DataModelRoot
   Control/SGTools
   Database/APR/StorageSvc
   Database/AthenaPOOL/AthenaPoolUtilities
   Database/AthenaPOOL/PoolSvc
   Database/PersistentDataModel
   Database/TPTools
   GaudiKernel
   PRIVATE
   AtlasTest/TestTools
   Control/AthContainersInterfaces
   Control/AthContainersRoot
   Control/RootUtils
   Control/StoreGate )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( AthenaPoolCnvSvcLib
   AthenaPoolCnvSvc/*.h AthenaPoolCnvSvc/*.icc src/*.cxx
   PUBLIC_HEADERS AthenaPoolCnvSvc
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AthContainers AthContainersRoot AthLinks AthenaBaseComps
   AthenaKernel CxxUtils DataModelRoot RootUtils SGTools CollectionBase
   CollectionUtilities POOLCore PersistencySvc StorageSvc AthenaPoolUtilities
   PersistentDataModel TPTools GaudiKernel StoreGateLib
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

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
   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaPoolCnvSvcLib )

atlas_add_test( TPCnvList_test
   SOURCES test/TPCnvList_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaPoolCnvSvcLib )

atlas_add_test( T_AthenaPoolViewVectorCnv_test
   SOURCES test/T_AthenaPoolViewVectorCnv_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks SGTools GaudiKernel
   TestTools AthenaPoolCnvSvcLib
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( T_AthenaPoolxAODCnv_test
   SOURCES test/T_AthenaPoolxAODCnv_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PersistentDataModel SGTools TestTools
   CxxUtils AthenaPoolCnvSvcLib
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( T_AthenaPoolAuxContainerCnv_test
   SOURCES test/T_AthenaPoolAuxContainerCnv_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PersistentDataModel AthContainers SGTools
   TestTools CxxUtils AthenaPoolCnvSvcLib
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( T_AthenaPoolTPCnvCnv_test
   SOURCES test/T_AthenaPoolTPCnvCnv_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PersistentDataModel SGTools TestTools
   CxxUtils AthenaPoolCnvSvcLib
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
