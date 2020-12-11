# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ZdcEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( ZdcEventAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES ZdcEvent/ZdcDigitsCollection.h ZdcEvent/ZdcRawChannelCollection.h
         ZDC_SimEvent/ZDC_SimStripHit_Collection.h
         ZDC_SimEvent/ZDC_SimPixelHit_Collection.h
   LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities ZDC_SimEvent
                  ZdcEventTPCnv ZdcEvent GaudiKernel )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  run_tpcnv_test( ZdcEventTPCnv_15.6.7   ESD-15.6.7-data )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
