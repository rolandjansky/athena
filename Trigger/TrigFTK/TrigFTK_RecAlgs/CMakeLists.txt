################################################################################
# Package: TrigFTK_RecAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrigFTK_RecAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Trigger/TrigFTK/FTK_DataProviderInterfaces
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Event/EventPrimitives
                          Event/xAOD/xAODTracking
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigEvent/TrigParticle )

# Component(s) in the package:
atlas_add_component( TrigFTK_RecAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel FTK_DataProviderInterfaces TrigInterfacesLib EventPrimitives xAODTracking TrkEventPrimitives TrkParameters TrkParticleBase TrkTrack VxVertex TrigParticle )

# Install files from the package:
atlas_install_headers( TrigFTK_RecAlgs )
atlas_install_python_modules( python/*.py )

