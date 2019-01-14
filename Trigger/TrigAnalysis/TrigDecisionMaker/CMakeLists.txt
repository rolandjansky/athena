################################################################################
# Package: TrigDecisionMaker
################################################################################

# Declare the package name:
atlas_subdir( TrigDecisionMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigDecisionEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigSteering
                          Trigger/TrigSteer/DecisionHandling
                          Trigger/TrigT1/TrigT1Result )

# Component(s) in the package:
atlas_add_component( TrigDecisionMaker
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps DecisionHandling StoreGateLib SGtests EventInfo xAODEventInfo GaudiKernel TrigConfL1Data TrigConfHLTData TrigDecisionEvent TrigSteeringEvent TrigSteeringLib TrigT1Result xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/__init__.py python/TrigDecisionMakerConfig.py )
atlas_install_joboptions( share/jobOfragment*.py )

