# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODBTaggingAthenaPool )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODBTaggingAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODBTagging/BTaggingContainer.h xAODBTagging/BTaggingAuxContainer.h
         xAODBTagging/BTaggingTrigAuxContainer.h
         xAODBTagging/BTagVertexContainer.h
         xAODBTagging/BTagVertexAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::BTaggingContainer xAOD::BTaggingAuxContainer
                        xAOD::BTaggingTrigAuxContainer xAOD::BTagVertexContainer
                        xAOD::BTagVertexAuxContainer
   CNV_PFX xAOD
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaKernel
                  AthenaPoolCnvSvcLib AthenaPoolUtilities xAODBTagging
                  xAODTracking GaudiKernel )

# Install files from the package.
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODBTAGGINGATHENAPOOL_REFERENCE_TAG
       xAODBTaggingAthenaPoolReference-01-01-00 )
  run_tpcnv_legacy_test( xAODBTaggingAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODBTaggingAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODBTAGGINGATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
