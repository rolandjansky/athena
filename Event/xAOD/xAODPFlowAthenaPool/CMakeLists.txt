# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODPFlowAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODPFlowAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODPFlow/PFOContainer.h xAODPFlow/PFOAuxContainer.h
         xAODPFlow/TrackCaloClusterContainer.h
         xAODPFlow/TrackCaloClusterAuxContainer.h
         xAODPFlow/FlowElementContainer.h
         xAODPFlow/FlowElementAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::PFOContainer xAOD::PFOAuxContainer
                        xAOD::TrackCaloClusterContainer
                        xAOD::TrackCaloClusterAuxContainer
			xAOD::FlowElementContainer
			xAOD::FlowElementAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib
                  AthenaPoolUtilities xAODPFlow )

# Install files from the package.
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODPFLOWATHENAPOOL_REFERENCE_TAG
       xAODPFlowAthenaPoolReference-01-00-00 )
  run_tpcnv_legacy_test( xAODPFlowAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODPFlowAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODPFLOWATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
