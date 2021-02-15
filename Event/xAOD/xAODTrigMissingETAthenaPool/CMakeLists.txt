# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTrigMissingETAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigMissingETAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODTrigMissingET/TrigMissingETContainer.h
         xAODTrigMissingET/TrigMissingETAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::TrigMissingETContainer
                        xAOD::TrigMissingETAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTrigMissingET
                  GaudiKernel )

# Install files from the package.
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODTRIGMISSINGETATHENAPOOL_REFERENCE_TAG
       xAODTrigMissingETAthenaPoolReference-01-00-00 )
  run_tpcnv_test( xAODTrigMissingETAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODTrigMissingETAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRIGMISSINGETATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
