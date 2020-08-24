################################################################################
# Package: TrackToVertex
################################################################################

# Declare the package name:
atlas_subdir( TrackToVertex )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          Reconstruction/RecoTools/ITrackToVertex
                          Tracking/TrkEvent/TrkNeutralParameters
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          PRIVATE
                          Control/StoreGate
                          Reconstruction/Particle
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TrackToVertex
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives EventPrimitives GaudiKernel ITrackToVertex TrkNeutralParameters StoreGateLib SGtests Particle TrkDetDescrUtils TrkSurfaces TrkEventPrimitives TrkParameters TrkParticleBase TrkTrack VxVertex TrkExInterfaces BeamSpotConditionsData )

# Install files from the package:
atlas_install_headers( TrackToVertex )

