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
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParametersBase
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          PRIVATE
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackLink
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrkVertexFitterUtils
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODTracking GaudiKernel TrkParameters TrkParametersBase TrkVertexFitterInterfaces MagFieldInterfaces TrkSurfaces TrkEventPrimitives TrkNeutralParameters TrkParticleBase TrkTrack VxVertex TrkExInterfaces TrkExUtils )

# Install files from the package:
atlas_install_headers( TrkVertexFitterUtils )

