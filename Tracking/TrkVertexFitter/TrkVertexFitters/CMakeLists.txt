################################################################################
# Package: TrkVertexFitters
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexFitters )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParametersBase
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          PRIVATE
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkLinks
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxMultiVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkVertexFitter/TrkVxEdmCnv )

# Component(s) in the package:
atlas_add_library( TrkVertexFittersLib
                   src/*.cxx
                   PUBLIC_HEADERS TrkVertexFitters
                   LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel TrkParameters TrkParametersBase TrkParticleBase VxVertex TrkVertexFitterInterfaces TrkVxEdmCnvLib
                   PRIVATE_LINK_LIBRARIES TrkSurfaces TrkLinks TrkTrack VxMultiVertex TrkExInterfaces )

atlas_add_component( TrkVertexFitters
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel TrkParameters TrkParametersBase TrkParticleBase VxVertex TrkVertexFitterInterfaces TrkSurfaces TrkLinks TrkTrack VxMultiVertex TrkExInterfaces TrkVxEdmCnvLib TrkVertexFittersLib )

