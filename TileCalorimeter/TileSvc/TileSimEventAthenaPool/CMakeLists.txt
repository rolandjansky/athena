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

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  run_tpcnv_test( TileSimEventTPCnv_HITS    HITS.04919495._000416 )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
