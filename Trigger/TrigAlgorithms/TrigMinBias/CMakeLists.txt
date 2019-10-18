################################################################################
# Package: TrigMinBias
################################################################################

# Declare the package name:
atlas_subdir( TrigMinBias )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigMinBias
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigSteer/DecisionHandling
                          Control/StoreGate
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters
                          Trigger/TrigTools/TrigTimeAlgs
                          Event/xAOD/xAODTrigger
                          Control/AthenaMonitoring)

# Component(s) in the package:
atlas_add_component( TrigMinBias
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES DecisionHandlingLib AthenaMonitoringLib xAODTracking xAODTrigMinBias TrkTrack TrigInDetEvent TrigInterfacesLib StoreGateLib GaudiKernel TrkParameters TrigTimeAlgsLib)

# Install files from the package:
atlas_install_python_modules( python/*.py )
