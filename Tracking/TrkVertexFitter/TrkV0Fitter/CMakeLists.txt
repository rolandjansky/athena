################################################################################
# Package: TrkV0Fitter
################################################################################

# Declare the package name:
atlas_subdir( TrkV0Fitter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          MagneticField/MagFieldConditions
                          MagneticField/MagFieldElements
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          PRIVATE
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkLinks
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# Component(s) in the package:
atlas_add_library( TrkV0FitterLib
                   src/*.cxx
                   PUBLIC_HEADERS TrkV0Fitter
                   LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel MagFieldElements MagFieldConditions TrkParameters TrkVertexFitterInterfaces
                   PRIVATE_LINK_LIBRARIES TrkDetDescrUtils TrkSurfaces TrkLinks TrkParticleBase TrkTrack VxVertex TrkExInterfaces )

atlas_add_component( TrkV0Fitter
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel TrkParameters TrkVertexFitterInterfaces TrkDetDescrUtils TrkSurfaces TrkLinks TrkParticleBase TrkTrack VxVertex TrkExInterfaces TrkV0FitterLib )

