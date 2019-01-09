################################################################################
# Package: MuonSimEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( MuonSimEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          PRIVATE
                          AtlasTest/TestTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          MuonSpectrometer/MuonCnv/MuonSimEventTPCnv
                          MuonSpectrometer/MuonSimEvent
                          Simulation/HitManagement )

# Component(s) in the package:
atlas_add_poolcnv_library( MuonSimEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES MuonSimEvent/CSCSimHitCollection.h MuonSimEvent/MDTSimHitCollection.h MuonSimEvent/RPCSimHitCollection.h MuonSimEvent/TGCSimHitCollection.h MuonSimEvent/GenericMuonSimHitCollection.h MuonSimEvent/sTGCSimHitCollection.h MuonSimEvent/MMSimHitCollection.h
                           LINK_LIBRARIES AthenaPoolUtilities AtlasSealCLHEP StoreGateLib SGtests AthenaPoolCnvSvcLib GaudiKernel MuonSimEventTPCnv MuonSimEvent HitManagement )

# Install files from the package:
atlas_install_headers( MuonSimEventAthenaPool )
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( MUONSIMEVENTATHENAPOOL_REFERENCE_TAG
       MuonSimEventAthenaPoolReference-01-01-00 )
  run_tpcnv_test( MuonSimEventTPCnv_HITS   HITS.04919495._000416
                   REFERENCE_TAG ${MUONSIMEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
