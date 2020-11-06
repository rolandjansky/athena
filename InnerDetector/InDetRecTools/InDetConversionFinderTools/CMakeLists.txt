# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetConversionFinderTools )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( InDetConversionFinderToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetConversionFinderTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel GeoPrimitives InDetRecToolInterfaces Particle TrkEventPrimitives TrkExInterfaces TrkParameters TrkParticleBase TrkToolInterfaces TrkTrack TrkVertexFitterInterfaces TrkVertexSeedFinderUtilsLib xAODTracking
                    PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthLinks InDetPrepRawData TrkMeasurementBase TrkRIO_OnTrack TrkSurfaces VxVertex )

atlas_add_component( InDetConversionFinderTools
                     src/components/*.cxx
                     LINK_LIBRARIES InDetConversionFinderToolsLib )

# Test(s) in the package:
atlas_add_test( VertexPointEstimator_test
                SOURCES test/VertexPointEstimator_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils GaudiKernel InDetConversionFinderToolsLib TestTools )
