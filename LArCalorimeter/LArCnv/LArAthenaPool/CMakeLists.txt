# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( LArAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES LArRawEvent/LArRawChannelContainer.h LArRawEvent/LArDigitContainer.h
         LArRawEvent/LArTTL1Container.h LArRawEvent/LArFebErrorSummary.h LArRawEvent/LArLATOMEHeaderContainer.h
         LArRecEvent/LArNoisyROSummary.h
   LINK_LIBRARIES AthenaPoolUtilities CaloDetDescrLib AthenaKernel StoreGateLib
                  AthenaPoolCnvSvcLib GaudiKernel LArTPCnv LArRawEvent
                  LArRecEvent )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  run_tpcnv_legacy_test( LArTPCnv_15.6.7   ESD-15.6.7-data )
  run_tpcnv_legacy_test( LArTPCnv_20.1.7.2 ESD-20.1.7.2 )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
