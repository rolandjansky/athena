# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODEgammaAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODEgammaAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODEgamma/ElectronContainer.h xAODEgamma/ElectronAuxContainer.h
         xAODEgamma/PhotonContainer.h xAODEgamma/PhotonAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::ElectronContainer xAOD::ElectronAuxContainer
                        xAOD::PhotonContainer xAOD::PhotonAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib
                  AthenaPoolUtilities xAODEgamma GaudiKernel )

# Install files from the package.
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODEGAMMAATHENAPOOL_REFERENCE_TAG
       xAODEgammaAthenaPoolReference-01-00-00 )
  run_tpcnv_test( xAODEgammaAthenaPool_20.1.7.2   AOD-20.1.7.2-full
                   REQUIRED_LIBRARIES xAODEgammaAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODEGAMMAATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( xAODEgammaAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODEgammaAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODEGAMMAATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
