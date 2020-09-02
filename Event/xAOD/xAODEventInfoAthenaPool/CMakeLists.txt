# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODEventInfoAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODEventInfoAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODEventInfo/EventInfo.h xAODEventInfo/EventAuxInfo.h
      xAODEventInfo/EventInfoContainer.h
      xAODEventInfo/EventInfoAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::EventInfo xAOD::EventAuxInfo
      xAOD::EventInfoContainer xAOD::EventInfoAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities AthContainers
      xAODEventInfo )

# Install files from the package.
atlas_install_joboptions( share/*.py )

# Find the T/P test CMake code.
find_package( AthenaPoolUtilitiesTest )
if( NOT ATHENAPOOLUTILITIESTEST_FOUND )
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
   return()
endif()

# Set up the T/P converter test(s).
if( NOT GENERATIONBASE )
   run_tpcnv_legacy_test( xAODEventInfoAthenaPool_20.0.0.3 AOD-20.0.0.3-full
      REQUIRED_LIBRARIES xAODEventInfoAthenaPoolPoolCnv )
endif()
# TPCnv unit tests disabled for AthGeneration project due to a dependency on DetDescrCnvSvc