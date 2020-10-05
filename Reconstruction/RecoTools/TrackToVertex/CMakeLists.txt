################################################################################
# Package: TrackToVertex
################################################################################

# Declare the package name:
atlas_subdir( TrackToVertex )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TrackToVertex
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives EventPrimitives GaudiKernel ITrackToVertex 
		     TrkNeutralParameters StoreGateLib SGtests Particle TrkDetDescrUtils TrkSurfaces TrkEventPrimitives TrkParameters 
		     TrkParticleBase TrkTrack VxVertex TrkExInterfaces BeamSpotConditionsData CxxUtils)

# Install files from the package:
atlas_install_headers( TrackToVertex )

