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
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigEvent/TrigParticle
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Matrix TreePlayer )

# Component(s) in the package:
atlas_add_component( TrigFTK_RecAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel FTK_DataProviderInterfaces TrigInterfacesLib EventPrimitives xAODTracking TrkEventPrimitives TrkParameters TrkParticleBase TrkRIO_OnTrack TrkTrack VxVertex TrkToolInterfaces TrigParticle InDetRIO_OnTrack )

# Install files from the package:
atlas_install_headers( TrigFTK_RecAlgs )
atlas_install_python_modules( python/*.py )

