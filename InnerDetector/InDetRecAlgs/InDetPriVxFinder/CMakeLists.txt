################################################################################
# Package: InDetPriVxFinder
################################################################################

# Declare the package name:
atlas_subdir( InDetPriVxFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaMonitoringKernel
                          GaudiKernel
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          PRIVATE
                          Control/CxxUtils
                          Event/EventPrimitives
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODTracking
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Reconstruction/RecoTools/ITrackToVertex
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces)

# Component(s) in the package:

atlas_add_component( InDetPriVxFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel CxxUtils EventPrimitives xAODCore xAODTracking InDetRecToolInterfaces ITrackToVertex TrkSurfaces TrkParticleBase TrkTrack VxVertex TrkVertexFitterInterfaces AthenaMonitoringKernelLib BeamSpotConditionsData )

# Install files from the package:
atlas_install_headers( InDetPriVxFinder )
atlas_install_python_modules( python/*.py )
