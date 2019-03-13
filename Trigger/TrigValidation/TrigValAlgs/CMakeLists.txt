################################################################################
# Package: TrigValAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrigValAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthAnalysisBaseComps
                          GaudiKernel
                          TestPolicy
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigNavTools
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigEvent/TrigRoiConversion
                          PRIVATE
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/AthViews
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
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
                          Trigger/TrigEvent/TrigNavStructure
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfigSvc
                          Trigger/TrigEvent/TrigDecisionEvent
                          Trigger/TrigEvent/TrigMissingEtEvent
                          Trigger/TrigSteer/DecisionHandling
                          Trigger/TrigTruthEvent/TrigInDetTruthEvent
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( CLHEP )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( TrigValAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthAnalysisBaseCompsLib GaudiKernel TrigDecisionToolLib TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigNavToolsLib TrigNavigationLib TrigParticle TrigSteeringEvent AthenaKernel CxxUtils EventInfo xAODEventInfo xAODBTagging xAODEgamma xAODJet xAODMuon xAODTau xAODTracking xAODTrigBphys xAODTrigCalo xAODTrigEgamma xAODTrigMinBias xAODTrigMissingET xAODTrigMuon xAODCore xAODTrigger AnalysisTriggerEvent MuonCombinedToolInterfaces Particle tauEvent VxSecVertex TrigConfigSvcLib TrigConfHLTData TrigRoiConversionLib TrigDecisionEvent TrigMissingEtEvent TrigInDetTruthEvent TrigT1Interfaces DecisionHandlingLib AthViews )

# Install files from the package:
atlas_install_headers( TrigValAlgs )
atlas_install_python_modules( python/__init__.py python/TrigValAlgsConfig.py )

