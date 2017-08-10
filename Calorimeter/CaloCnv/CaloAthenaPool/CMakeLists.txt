# $Id: CMakeLists.txt 780060 2016-10-24 14:27:13Z krasznaa $
################################################################################
# Package: CaloAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( CaloAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Database/AthenaPOOL/AthenaPoolUtilities
   PRIVATE
   AtlasTest/TestTools
   Calorimeter/CaloCnv/CaloTPCnv
   Calorimeter/CaloDetDescr
   Calorimeter/CaloEvent
   Calorimeter/CaloInterface
   Calorimeter/CaloUtils
   Control/SGTools
   Control/StoreGate
   Database/AthenaPOOL/AthenaPoolCnvSvc )

# Component(s) in the package:
atlas_add_poolcnv_library( CaloAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES CaloEvent/CaloCellContainer.h CaloEvent/CaloClusterContainer.h
   CaloEvent/CaloClusterCellLinkContainer.h CaloEvent/CaloTopoTowerContainer.h
   CaloEvent/CaloTowerContainer.h CaloEvent/CaloCellLinkContainer.h
   CaloEvent/CaloShowerContainer.h
   LINK_LIBRARIES AthenaPoolUtilities CaloTPCnv CaloDetDescrLib CaloEvent
   CaloUtilsLib SGTools StoreGateLib AthenaPoolCnvSvcLib )

# Install files from the package:
atlas_install_headers( CaloAthenaPool )
atlas_install_joboptions( share/*.py )

# Find the helper code for the T/P tests:
set( _poolTestDir
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../Database/AthenaPOOL/AthenaPoolUtilities )
if( IS_DIRECTORY ${_poolTestDir} )
   set( AthenaPoolUtilitiesTest_DIR ${_poolTestDir}/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

# Set up the tests if possible:
if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( CALOATHENAPOOL_REFERENCE_TAG
       CaloAthenaPoolReference-01-00-00 )
  run_tpcnv_test( CaloTPCnv_14.5.0   AOD-14.5.0-full
                   REFERENCE_TAG ${CALOATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( CaloTPCnv_15.1.0   AOD-15.1.0-full
                   REFERENCE_TAG ${CALOATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( CaloTPCnv_15.5.0   AOD-15.5.0-full
                   REFERENCE_TAG ${CALOATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( CaloTPCnv_18.0.0   ESD-18.0.0
                   REFERENCE_TAG ${CALOATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( CaloTPCnv_20.1.7.2 ESD-20.1.7.2
                   REFERENCE_TAG ${CALOATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
