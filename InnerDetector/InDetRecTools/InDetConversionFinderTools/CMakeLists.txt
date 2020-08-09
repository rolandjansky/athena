################################################################################
# Package: InDetConversionFinderTools
################################################################################

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
                   LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel InDetRecToolInterfaces Particle TrkEventPrimitives TrkParameters TrkParticleBase TrkTrack TrkVertexSeedFinderUtilsLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks InDetPrepRawData TrkSurfaces TrkMeasurementBase TrkRIO_OnTrack VxVertex
                   TrkExInterfaces TrkToolInterfaces TrkVertexFitterInterfaces )

atlas_add_component( InDetConversionFinderTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps xAODTracking GaudiKernel InDetRecToolInterfaces Particle TrkEventPrimitives
                     TrkParameters TrkParticleBase TrkTrack AthLinks InDetPrepRawData TrkSurfaces TrkMeasurementBase TrkRIO_OnTrack VxVertex TrkExInterfaces
                     TrkToolInterfaces TrkVertexFitterInterfaces InDetConversionFinderToolsLib )


atlas_add_test( VertexPointEstimator_test
                SOURCES test/VertexPointEstimator_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} InDetConversionFinderToolsLib )
