# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTrigEgammaAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigEgammaAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODTrigEgamma/TrigPhotonContainer.h
         xAODTrigEgamma/TrigPhotonAuxContainer.h
         xAODTrigEgamma/TrigElectronContainer.h
         xAODTrigEgamma/TrigElectronAuxContainer.h
         xAODTrigEgamma/ElectronTrigAuxContainer.h
         xAODTrigEgamma/PhotonTrigAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::TrigPhotonContainer xAOD::TrigElectronContainer
                        xAOD::ElectronTrigAuxContainer
                        xAOD::PhotonTrigAuxContainer
                        xAOD::TrigElectronAuxContainer
                        xAOD::TrigPhotonAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib
                  AthenaPoolUtilities xAODEgamma xAODTrigEgamma GaudiKernel )

# Install files from the package.
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODTRIGEGAMMAATHENAPOOL_REFERENCE_TAG
       xAODTrigEgammaAthenaPoolReference-01-01-00 )
  run_tpcnv_legacy_test( xAODTrigEgammaAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODTrigEgammaAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRIGEGAMMAATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
