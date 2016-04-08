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

