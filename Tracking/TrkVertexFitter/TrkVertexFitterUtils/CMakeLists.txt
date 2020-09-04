################################################################################
# Package: TrkVertexFitterUtils
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexFitterUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( CLHEP )

atlas_add_library( TrkVertexFitterUtilsLib
                   TrkVertexFitterUtils/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrkVertexFitterUtils
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES TrkVertexFitterInterfaces AthenaBaseComps xAODTracking GaudiKernel TrkParametersBase TrkParameters MagFieldConditions ${CLHEP_LIBRARIES} )

# Component(s) in the package:
atlas_add_component( TrkVertexFitterUtils
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} MagFieldElements TrkSurfaces TrkEventPrimitives TrkNeutralParameters TrkParticleBase TrkTrack VxVertex TrkExInterfaces TrkExUtils TrkVertexFitterUtilsLib )

# Install files from the package:
atlas_install_joboptions( share/*.txt )


atlas_add_test( DummyAnnealingMaker_test
                SOURCES test/DummyAnnealingMaker_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODTracking GaudiKernel TrkParameters TrkParametersBase TrkVertexFitterInterfaces MagFieldConditions MagFieldElements PathResolver TrkSurfaces TrkEventPrimitives TrkNeutralParameters TrkParticleBase TrkTrack VxVertex TrkExInterfaces TrkExUtils TestTools
                )

atlas_add_test( DetAnnealingMaker_test
                SOURCES test/DetAnnealingMaker_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODTracking GaudiKernel TrkParameters TrkParametersBase TrkVertexFitterInterfaces MagFieldConditions MagFieldElements PathResolver TrkSurfaces TrkEventPrimitives TrkNeutralParameters TrkParticleBase TrkTrack VxVertex TrkExInterfaces TrkExUtils TestTools
                )

atlas_add_test( ImpactPoint3dEstimator_test
                SOURCES test/ImpactPoint3dEstimator_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODTracking GaudiKernel TrkParameters TrkParametersBase TrkVertexFitterInterfaces MagFieldConditions MagFieldElements PathResolver TrkSurfaces TrkEventPrimitives TrkNeutralParameters TrkParticleBase TrkTrack VxVertex TrkExInterfaces TrkExUtils TestTools
                )
