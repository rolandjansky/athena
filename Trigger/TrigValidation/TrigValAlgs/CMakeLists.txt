################################################################################
# Package: TrigValAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrigValAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          TestPolicy
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigNavTools
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Control/AthenaKernel
                          Control/SGTools
                          Event/EventInfo
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigBphys
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODTrigEgamma
                          Event/xAOD/xAODTrigMinBias
                          Event/xAOD/xAODTrigMissingET
                          Event/xAOD/xAODTrigMuon
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODTrigger
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          Reconstruction/MuonIdentification/MuonCombinedToolInterfaces
                          Reconstruction/Particle
                          Reconstruction/tauEvent
                          Tracking/TrkEvent/VxSecVertex
                          Trigger/TrigConfiguration/TrigConfigSvc
                          Trigger/TrigEvent/TrigDecisionEvent
                          Trigger/TrigEvent/TrigMissingEtEvent
                          Trigger/TrigTruthEvent/TrigInDetTruthEvent )

# External dependencies:
find_package( CLHEP )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( TrigValAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel TrigDecisionToolLib TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigNavToolsLib TrigNavigationLib TrigParticle TrigSteeringEvent AthenaKernel SGTools EventInfo xAODBTagging xAODEgamma xAODJet xAODMuon xAODTau xAODTracking xAODTrigBphys xAODTrigCalo xAODTrigEgamma xAODTrigMinBias xAODTrigMissingET xAODTrigMuon xAODCore xAODTrigger AnalysisTriggerEvent MuonCombinedToolInterfaces Particle tauEvent VxSecVertex TrigConfigSvcLib TrigDecisionEvent TrigMissingEtEvent TrigInDetTruthEvent )

# Install files from the package:
atlas_install_headers( TrigValAlgs )
atlas_install_python_modules( python/__init__.py python/TrigValAlgsConfig.py )

