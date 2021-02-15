# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTrigRingerAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigRingerAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODTrigRinger/TrigRingerRingsContainer.h
         xAODTrigRinger/TrigRingerRingsAuxContainer.h
         xAODTrigRinger/TrigRNNOutputContainer.h
         xAODTrigRinger/TrigRNNOutputAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::TrigRingerRingsContainer
                        xAOD::TrigRingerRingsAuxContainer
                        xAOD::TrigRNNOutputContainer
                        xAOD::TrigRNNOutputAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib
                  AthenaPoolUtilities xAODTrigRinger GaudiKernel )

# Install files from the package.
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODTRIGRINGERATHENAPOOL_REFERENCE_TAG
       xAODTrigRingerAthenaPoolReference-01-00-00 )
  run_tpcnv_test( xAODTrigRingerAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODTrigRingerAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRIGRINGERATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
