# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODJetAthenaPool )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Types to set up converters for.
set( poolcnv_files xAODJet/JetContainer.h xAODJet/JetAuxContainer.h )
set( poolcnv_types xAOD::JetContainer xAOD::JetAuxContainer )
if( NOT SIMULATIONBASE AND NOT GENERATIONBASE )
   list( APPEND poolcnv_files xAODJet/JetTrigAuxContainer.h )
   list( APPEND poolcnv_types xAOD::JetTrigAuxContainer )
endif()

# Component(s) in the package:
atlas_add_poolcnv_library( xAODJetAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES ${poolcnv_files}
   TYPES_WITH_NAMESPACE ${poolcnv_types}
   CNV_PFX xAOD
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaKernel
                  AthenaPoolCnvSvcLib AthenaPoolUtilities xAODJet )

# Install files from the package.
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND AND NOT SIMULATIONBASE
   AND NOT GENERATIONBASE )
   set( XAODJETATHENAPOOL_REFERENCE_TAG
      xAODJetAthenaPoolReference-01-01-00 )
   run_tpcnv_legacy_test( xAODJetAthenaPool_20.7.2.2   AOD-20.7.2.2-full
      REQUIRED_LIBRARIES xAODJetAthenaPoolPoolCnv
      REFERENCE_TAG ${XAODJETATHENAPOOL_REFERENCE_TAG} )
   run_tpcnv_legacy_test( xAODJetAthenaPool_21.0.79   AOD-21.0.79-full
      REQUIRED_LIBRARIES xAODJetAthenaPoolPoolCnv
      REFERENCE_TAG ${XAODJETATHENAPOOL_REFERENCE_TAG} )
elseif( NOT ATHENAPOOLUTILITIESTEST_FOUND )
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
