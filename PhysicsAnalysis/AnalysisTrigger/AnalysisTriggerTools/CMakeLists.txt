################################################################################
# Package: AnalysisTriggerTools
################################################################################

# Declare the package name:
atlas_subdir( AnalysisTriggerTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Reconstruction/MuonIdentification/muonEvent
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigEvent/TrigDecisionEvent
                          Trigger/TrigSteer/TrigSteering
                          Trigger/TrigT1/TrigT1Interfaces
                          PRIVATE
                          Control/StoreGate
                          Event/FourMom
                          PhysicsAnalysis/AnalysisCommon/AnalysisTools
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( AnalysisTriggerToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS AnalysisTriggerTools
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel muonEvent TrigDecisionEvent TrigT1Interfaces TrigDecisionToolLib TrigSteeringLib StoreGateLib SGtests AthAnalysisToolsLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} FourMom ParticleEvent AnalysisTriggerEvent McParticleEvent TrigConfL1Data TrigMuonEvent TrigT1Result )

atlas_add_component( AnalysisTriggerTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel muonEvent TrigDecisionToolLib TrigDecisionEvent TrigSteeringLib TrigT1Interfaces StoreGateLib SGtests FourMom AthAnalysisToolsLib ParticleEvent AnalysisTriggerEvent McParticleEvent TrigConfL1Data TrigMuonEvent TrigT1Result AnalysisTriggerToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

