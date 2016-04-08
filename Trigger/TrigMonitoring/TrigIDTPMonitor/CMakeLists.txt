################################################################################
# Package: TrigIDTPMonitor
################################################################################

# Declare the package name:
atlas_subdir( TrigIDTPMonitor )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/DataModel
                          Control/StoreGate
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigBphys
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigBphysicsEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces )

# Component(s) in the package:
atlas_add_component( TrigIDTPMonitor
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES DataModel StoreGateLib SGtests EventInfo xAODEventInfo xAODMuon xAODTracking xAODTrigBphys GaudiKernel TrigConfHLTData TrigBphysicsEvent TrigSteeringEvent TrigInterfacesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

