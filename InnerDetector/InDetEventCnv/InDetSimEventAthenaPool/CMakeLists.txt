# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetSimEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( InDetSimEventAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES InDetSimEvent/SiHitCollection.h
         InDetSimEvent/TRTUncompressedHitCollection.h
   LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities AtlasSealCLHEP
                  GaudiKernel InDetSimEventTPCnv InDetSimEvent )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( INDETSIMEVENTATHENAPOOL_REFERENCE_TAG
       InDetSimEventAthenaPoolReference-02-01-00 )
  run_tpcnv_legacy_test( InDetSimEventTPCnv_HITS   HITS.04919495._000416
                   REFERENCE_TAG ${INDETSIMEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
