# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( egammaAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( egammaAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES egammaEvent/egammaContainer.h egammaEvent/ElectronContainer.h
         egammaEvent/PhotonContainer.h egammaEvent/egDetailContainer.h
         egammaEvent/EMShowerMinimalContainer.h egammaEvent/CaloRingsContainer.h
   LINK_LIBRARIES AthenaPoolUtilities AthenaBaseComps AthenaPoolCnvSvcLib
                  GaudiKernel egammaEvent egammaEventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
   set( EGAMMAATHENAPOOL_REFERENCE_TAG
        egammaAthenaPoolReference-01-00-00 )
  run_tpcnv_test( egammaEventTPCnv_15.1.0    AOD-15.1.0-full
                   REFERENCE_TAG ${EGAMMAATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( egammaEventTPCnv_15.6.9    AOD-15.6.9-full
                   REFERENCE_TAG ${EGAMMAATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( egammaEventTPCnv_16.6.7.1  AOD-16.6.7.1-full
                   REFERENCE_TAG ${EGAMMAATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( egammaEventTPCnv_18.0.0    AOD-18.0.0-full
                   REFERENCE_TAG ${EGAMMAATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
