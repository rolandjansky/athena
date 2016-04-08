################################################################################
# Package: TileSimEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TileSimEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          AtlasTest/TestTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          TileCalorimeter/TileSimEvent
                          TileCalorimeter/TileSvc/TileSimEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( TileSimEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TileSimEvent/TileHitVector.h
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities GaudiKernel TileSimEvent TileSimEventTPCnv )

# Install files from the package:
atlas_install_headers( TileSimEventAthenaPool )
atlas_install_joboptions( share/*.py )

