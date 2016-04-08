################################################################################
# Package: InDetPriVxFinder
################################################################################

# Declare the package name:
atlas_subdir( InDetPriVxFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Control/CxxUtils
                          Event/EventPrimitives
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODTracking
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Reconstruction/RecoTools/ITrackToVertex
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Tracking/TrkVertexFitter/TrkVxEdmCnv )

# Component(s) in the package:
atlas_add_component( InDetPriVxFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel CxxUtils EventPrimitives xAODCore xAODTracking InDetRecToolInterfaces ITrackToVertex TrkSurfaces TrkParticleBase TrkTrack VxVertex TrkVertexFitterInterfaces TrkVxEdmCnvLib )

# Install files from the package:
atlas_install_headers( InDetPriVxFinder )

