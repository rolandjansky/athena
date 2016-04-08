################################################################################
# Package: InDetVKalPriVxFinderTool
################################################################################

# Declare the package name:
atlas_subdir( InDetVKalPriVxFinderTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkVertexFitter/TrkVKalVrtFitter
                          PRIVATE
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkTrackLink
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetVKalPriVxFinderTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODTracking GaudiKernel InDetRecToolInterfaces TrkParameters TrkParticleBase TrkTrack TrkVKalVrtFitterLib TrkEventPrimitives TrkTrackSummary VxVertex TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( InDetVKalPriVxFinderTool )
atlas_install_python_modules( python/*.py )

