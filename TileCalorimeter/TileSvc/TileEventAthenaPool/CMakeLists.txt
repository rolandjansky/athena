# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( TileEventAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES TileEvent/TileHitContainer.h TileEvent/TileDigitsContainer.h
         TileEvent/TileRawChannelContainer.h TileEvent/TileBeamElemContainer.h
         TileEvent/TileCellContainer.h TileEvent/TileMuContainer.h
         TileEvent/TileL2Container.h TileEvent/TileTTL1Container.h
         TileEvent/TileTriggerContainer.h TileEvent/TileCosmicMuonContainer.h
         TileEvent/TileCosmicMuon.h TileEvent/TileMuonReceiverContainer.h
   LINK_LIBRARIES AthenaPoolUtilities CaloDetDescrLib CaloIdentifier
                  StoreGateLib AthenaPoolCnvSvcLib GaudiKernel TileEvent
                  TileTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( TILEEVENTATHENAPOOL_REFERENCE_TAG
       TileEventAthenaPoolReference-01-00-00 )
  run_tpcnv_test( TileTPCnv_15.6.4    ESD-15.6.4
                  REFERENCE_TAG ${TILEEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( TileTPCnv_20.1.7.2  ESD-20.1.7.2
                  REFERENCE_TAG ${TILEEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
