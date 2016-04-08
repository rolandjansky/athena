################################################################################
# Package: xAODTriggerCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODTriggerCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolKernel
                          Event/EventInfo
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigEvent/TrigDecisionEvent
                          Trigger/TrigEvent/TrigDecisionInterface
                          Trigger/TrigEvent/TrigSteeringEvent )

# Component(s) in the package:
atlas_add_component( xAODTriggerCnv
                     src/*.cxx
                     Root/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AsgTools xAODTrigger GaudiKernel AthenaBaseComps AthenaKernel StoreGateLib SGtests EventInfo AnalysisTriggerEvent TrigConfHLTData TrigDecisionEvent TrigSteeringEvent )

# Install files from the package:
atlas_install_headers( xAODTriggerCnv )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

