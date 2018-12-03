# $Id: CMakeLists.txt 749568 2016-05-25 04:49:18Z krasznaa $
################################################################################
# Package: TrackParticleAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TrackParticleAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Database/AthenaPOOL/AthenaPoolUtilities
   PRIVATE
   AtlasTest/TestTools
   Control/StoreGate
   Database/AthenaPOOL/AthenaPoolCnvSvc
   GaudiKernel
   Reconstruction/Particle
   Reconstruction/TrackParticleTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( TrackParticleAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES Particle/TrackParticleContainer.h
   TYPES_WITH_NAMESPACE Rec::TrackParticleContainer
   LINK_LIBRARIES AthenaPoolUtilities StoreGateLib AthenaPoolCnvSvcLib
   GaudiKernel Particle TrackParticleTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )
if( ATHENAPOOLUTILITIESTEST_FOUND )
   set( TRACKPARTICLEATHENAPOOL_REFERENCE_TAG
        TrackParticleAthenaPoolReference-01-00-00 )
   run_tpcnv_test( TrackParticleTPCnv_16.6.3   AOD-16.6.3-full
                   REFERENCE_TAG ${TRACKPARTICLEATHENAPOOL_REFERENCE_TAG} )
   run_tpcnv_test( TrackParticleTPCnv_17.2.8   AOD-17.2.8-full
                   REFERENCE_TAG ${TRACKPARTICLEATHENAPOOL_REFERENCE_TAG} )
   run_tpcnv_test( TrackParticleTPCnv_18.0.0   AOD-18.0.0-full
                   REFERENCE_TAG ${TRACKPARTICLEATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
