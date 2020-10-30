# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTruthAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTruthAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODTruth/TruthParticleContainer.h
   xAODTruth/TruthParticleAuxContainer.h
   xAODTruth/TruthVertexContainer.h
   xAODTruth/TruthVertexAuxContainer.h
   xAODTruth/TruthEventContainer.h
   xAODTruth/TruthEventAuxContainer.h
   xAODTruth/TruthPileupEventContainer.h
   xAODTruth/TruthPileupEventAuxContainer.h
   xAODTruth/TruthMetaDataContainer.h
   xAODTruth/TruthMetaDataAuxContainer.h
   TYPES_WITH_NAMESPACE
   xAOD::TruthParticleContainer xAOD::TruthParticleAuxContainer
   xAOD::TruthVertexContainer xAOD::TruthVertexAuxContainer
   xAOD::TruthEventContainer xAOD::TruthEventAuxContainer
   xAOD::TruthPileupEventContainer xAOD::TruthPileupEventAuxContainer
   xAOD::TruthMetaDataContainer xAOD::TruthMetaDataAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib
   AthenaPoolUtilities xAODTruth )

# Install files from the package.
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND AND NOT SIMULATIONBASE
   AND NOT GENERATIONBASE )
   set( XAODTRUTHATHENAPOOL_REFERENCE_TAG
      xAODTruthAthenaPoolReference-01-01-00 )
   run_tpcnv_legacy_test( xAODTruthAthenaPool_21.0.79   AOD-21.0.79-full
      REQUIRED_LIBRARIES xAODTruthAthenaPoolPoolCnv
      REFERENCE_TAG ${XAODTRUTHATHENAPOOL_REFERENCE_TAG} )
elseif( NOT ATHENAPOOLUTILITIESTEST_FOUND )
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
