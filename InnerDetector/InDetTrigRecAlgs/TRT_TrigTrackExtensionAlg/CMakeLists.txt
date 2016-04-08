################################################################################
# Package: TRT_TrigTrackExtensionAlg
################################################################################

# Declare the package name:
atlas_subdir( TRT_TrigTrackExtensionAlg )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigEvent/TrigParticle )

# Component(s) in the package:
atlas_add_component( TRT_TrigTrackExtensionAlg
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrigInterfacesLib InDetRecToolInterfaces TrkTrack TrigParticle )

# Install files from the package:
atlas_install_headers( TRT_TrigTrackExtensionAlg )
atlas_install_python_modules( python/*.py )

