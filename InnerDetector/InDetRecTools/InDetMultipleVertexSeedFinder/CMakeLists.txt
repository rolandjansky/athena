################################################################################
# Package: InDetMultipleVertexSeedFinder
################################################################################

# Declare the package name:
atlas_subdir( InDetMultipleVertexSeedFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          PRIVATE
                          Event/EventPrimitives
                          Event/xAOD/xAODTracking
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetRecTools/InDetMultipleVertexSeedFinderUtils
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces )

# Component(s) in the package:
atlas_add_component( InDetMultipleVertexSeedFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRecToolInterfaces EventPrimitives xAODTracking InDetMultipleVertexSeedFinderUtilsLib TrkParticleBase TrkTrack VxVertex TrkExInterfaces TrkToolInterfaces TrkVertexFitterInterfaces InDetBeamSpotServiceLib )

# Install files from the package:
atlas_install_headers( InDetMultipleVertexSeedFinder )

