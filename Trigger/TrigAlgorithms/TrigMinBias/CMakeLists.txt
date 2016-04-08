################################################################################
# Package: TrigMinBias
################################################################################

# Declare the package name:
atlas_subdir( TrigMinBias )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigMinBias
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Control/StoreGate
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters
                          Trigger/TrigTools/TrigTimeAlgs )

# Component(s) in the package:
atlas_add_library( TrigMinBiasLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigMinBias
                   LINK_LIBRARIES xAODTracking xAODTrigMinBias TrkTrack TrigInDetEvent TrigInterfacesLib StoreGateLib SGtests TrigTimeAlgsLib
                   PRIVATE_LINK_LIBRARIES GaudiKernel TrkParameters )

atlas_add_component( TrigMinBias
                     src/components/*.cxx
                     LINK_LIBRARIES xAODTracking xAODTrigMinBias TrkTrack TrigInDetEvent TrigInterfacesLib StoreGateLib SGtests GaudiKernel TrkParameters TrigTimeAlgsLib TrigMinBiasLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

