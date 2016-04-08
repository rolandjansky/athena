################################################################################
# Package: InDetMultipleVertexSeedFinderUtils
################################################################################

# Declare the package name:
atlas_subdir( InDetMultipleVertexSeedFinderUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          PRIVATE
                          Event/EventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# Component(s) in the package:
atlas_add_library( InDetMultipleVertexSeedFinderUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetMultipleVertexSeedFinderUtils
                   LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel
                   PRIVATE_LINK_LIBRARIES EventPrimitives TrkParameters TrkParticleBase TrkTrack VxVertex TrkExInterfaces )

atlas_add_component( InDetMultipleVertexSeedFinderUtils
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel EventPrimitives TrkParameters TrkParticleBase TrkTrack VxVertex TrkExInterfaces InDetMultipleVertexSeedFinderUtilsLib )

