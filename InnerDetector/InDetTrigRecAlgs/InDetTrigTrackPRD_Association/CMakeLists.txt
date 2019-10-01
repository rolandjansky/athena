################################################################################
# Package: InDetTrigTrackPRD_Association
################################################################################

# Declare the package name:
atlas_subdir( InDetTrigTrackPRD_Association )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          Tracking/TrkEvent/TrkEventUtils
                          PRIVATE
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigEvent/TrigParticle )

# Component(s) in the package:
atlas_add_component( InDetTrigTrackPRD_Association
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrkToolInterfaces TrigInterfacesLib TrigTimeAlgsLib TrkTrack TrigParticle TrkEventUtils)

# Install files from the package:
atlas_install_headers( InDetTrigTrackPRD_Association )

