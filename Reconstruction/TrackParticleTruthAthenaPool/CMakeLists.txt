################################################################################
# Package: TrackParticleTruthAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TrackParticleTruthAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Reconstruction/ParticleTruth
                          Reconstruction/TrackParticleTruthTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( TrackParticleTruthAthenaPoolPoolCnv
                           src/*.cxx
                           FILES ParticleTruth/TrackParticleTruthCollection.h ParticleTruth/TrackParticleTruthCollectionContainer.h
                           LINK_LIBRARIES AthenaPoolUtilities GaudiKernel StoreGateLib SGtests AthenaPoolCnvSvcLib ParticleTruth TrackParticleTruthTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
   set( TRACKPARTICLETRUTHATHENAPOOL_REFERENCE_TAG
        TrackParticleTruthAthenaPoolReference-01-00-00 )
  run_tpcnv_test( TrackParticleTruthTPCnv_18.0.0   AOD-18.0.0-full
                  REFERENCE_TAG ${TRACKPARTICLETRUTHATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
