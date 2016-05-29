################################################################################
# Package: LArAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( LArAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          PRIVATE
                          AtlasTest/TestTools
                          Calorimeter/CaloDetDescr
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          LArCalorimeter/LArCnv/LArTPCnv
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArRecEvent )

# Component(s) in the package:
atlas_add_poolcnv_library( LArAthenaPoolPoolCnv
                           src/*.cxx
                           FILES LArRawEvent/LArRawChannelContainer.h LArRawEvent/LArDigitContainer.h LArRawEvent/LArTTL1Container.h LArRawEvent/LArFebErrorSummary.h LArRecEvent/LArNoisyROSummary.h
                           LINK_LIBRARIES AthenaPoolUtilities CaloDetDescrLib SGTools StoreGateLib SGtests AthenaPoolCnvSvcLib GaudiKernel LArTPCnv LArRawEvent LArRecEvent )

# Install files from the package:
atlas_install_headers( LArAthenaPool )
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  run_tpcnv_test( LArTPCnv_15.6.7   ESD-15.6.7-data )
  run_tpcnv_test( LArTPCnv_20.1.7.2 ESD-20.1.7.2 )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
