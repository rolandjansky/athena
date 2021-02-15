# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODCaloRingsAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODCaloRingsAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODCaloRings/CaloRingsContainer.h
         xAODCaloRings/CaloRingsAuxContainer.h
         xAODCaloRings/RingSetContainer.h
         xAODCaloRings/RingSetAuxContainer.h
         xAODCaloRings/RingSetConfContainer.h
         xAODCaloRings/RingSetConfAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::CaloRingsContainer xAOD::CaloRingsAuxContainer
                        xAOD::RingSetContainer xAOD::RingSetAuxContainer
                        xAOD::RingSetConfContainer xAOD::RingSetConfAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODCaloRings )

# Install files from the package.
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODCALORINGSATHENAPOOL_REFERENCE_TAG
       xAODCaloRingsAthenaPoolReference-01-00-00 )
  run_tpcnv_test( xAODCaloRingsAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODCaloRingsAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODCALORINGSATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
