################################################################################
# Package: TileEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TileEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          AtlasTest/TestTools
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileSvc/TileTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( TileEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TileEvent/TileHitContainer.h TileEvent/TileDigitsContainer.h TileEvent/TileRawChannelContainer.h TileEvent/TileBeamElemContainer.h TileEvent/TileCellContainer.h TileEvent/TileMuContainer.h TileEvent/TileL2Container.h TileEvent/TileTTL1Container.h TileEvent/TileTriggerContainer.h TileEvent/TileCosmicMuonContainer.h TileEvent/TileCosmicMuon.h TileEvent/TileMuonReceiverContainer.h
                           LINK_LIBRARIES AthenaPoolUtilities CaloDetDescrLib CaloIdentifier StoreGateLib SGtests AthenaPoolCnvSvcLib GaudiKernel TileEvent TileTPCnv )

# Install files from the package:
atlas_install_headers( TileEventAthenaPool )
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( TILEEVENTATHENAPOOL_REFERENCE_TAG
       TileEventAthenaPoolReference-01-00-00 )
  run_tpcnv_test( TileTPCnv_15.6.4    ESD-15.6.4
                  REFERENCE_TAG ${TILEEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( TileTPCnv_20.1.7.2  ESD-20.1.7.2
                  REFERENCE_TAG ${TILEEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
