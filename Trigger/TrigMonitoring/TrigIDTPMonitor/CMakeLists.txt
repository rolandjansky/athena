################################################################################
# Package: TrigIDTPMonitor
################################################################################
# Declare the package name:
atlas_subdir( TrigIDTPMonitor )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthLinks
                          Event/EventInfo
                          Event/xAOD/xAODEgamma
			  Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigBphys
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigBphysicsEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces )

# Component(s) in the package:
atlas_add_component( TrigIDTPMonitor
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthLinks EventInfo xAODEgamma xAODCaloEvent xAODEventInfo xAODMuon xAODTracking xAODTrigBphys GaudiKernel TrigConfHLTData TrigBphysicsEvent TrigParticle TrigSteeringEvent TrigInterfacesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

