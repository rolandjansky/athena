################################################################################
# Package: TrkVertexFitterUtils
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexFitterUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          MagneticField/MagFieldConditions   # exposed by FullLinearizedTrackFactory.h
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParametersBase
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          PRIVATE
                          AtlasTest/TestTools
                          MagneticField/MagFieldElements
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackLink
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils 
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrkVertexFitterUtils
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODTracking GaudiKernel TrkParameters TrkParametersBase TrkVertexFitterInterfaces MagFieldConditions MagFieldElements TrkSurfaces TrkEventPrimitives TrkNeutralParameters TrkParticleBase TrkTrack VxVertex TrkExInterfaces TrkExUtils )

# Install files from the package:
atlas_install_headers( TrkVertexFitterUtils )
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
