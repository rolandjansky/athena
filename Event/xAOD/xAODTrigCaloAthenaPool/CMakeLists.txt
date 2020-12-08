# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTrigCaloAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigCaloAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODTrigCalo/TrigCaloClusterContainer.h
         xAODTrigCalo/TrigCaloClusterAuxContainer.h
         xAODTrigCalo/TrigEMClusterContainer.h
         xAODTrigCalo/TrigEMClusterAuxContainer.h
         xAODTrigCalo/CaloClusterTrigAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::TrigCaloClusterContainer
                        xAOD::TrigCaloClusterAuxContainer
                        xAOD::TrigEMClusterContainer
                        xAOD::TrigEMClusterAuxContainer
                        xAOD::CaloClusterTrigAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib
                  AthenaPoolUtilities xAODCaloEvent xAODTrigCalo GaudiKernel )

# Install files from the package.
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODTRIGCALOATHENAPOOL_REFERENCE_TAG
       xAODTrigCaloAthenaPoolReference-01-00-00 )
  run_tpcnv_test( xAODTrigCaloAthenaPool_20.0.0.3   AOD-20.0.0.3-full
                   REQUIRED_LIBRARIES xAODTrigCaloAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRIGCALOATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( xAODTrigCaloAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODTrigCaloAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRIGCALOATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
