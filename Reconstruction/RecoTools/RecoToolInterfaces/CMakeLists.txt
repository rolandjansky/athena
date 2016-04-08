################################################################################
# Package: RecoToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( RecoToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODPrimitives
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Reconstruction/RecoEvent/ParticleCaloExtension
                          Tracking/TrkEvent/TrkCaloExtension
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_library( RecoToolInterfaces
                   PUBLIC_HEADERS RecoToolInterfaces
                   LINK_LIBRARIES AsgTools xAODBase xAODCaloEvent xAODPrimitives xAODTracking GaudiKernel ParticleCaloExtension TrkCaloExtension TrkTrack )

