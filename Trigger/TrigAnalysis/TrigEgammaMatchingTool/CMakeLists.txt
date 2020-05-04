################################################################################
# Package: TrigEgammaMatchingTool
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaMatchingTool )


# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODTrigEgamma
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTrigger
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/DecisionHandling
                          PRIVATE
                          Control/AthToolSupport/AsgTools
                          Control/AthenaMonitoring
                          Control/StoreGate
                          Control/AthenaBaseComps
                          GaudiKernel )



# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigEgammaMatchingToolLib
                   Root/*.cxx
                   PUBLIC_HEADERS TrigEgammaMatchingTool
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools xAODCaloEvent xAODEgamma xAODTracking xAODTrigCalo xAODTrigEgamma TrigConfHLTData 
                   TrigSteeringEvent TrigDecisionToolLib DecisionHandlingLib AthenaMonitoringLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} xAODMuon xAODTau xAODTrigger  )


atlas_add_component( TrigEgammaMatchingTool
                     src/*.h src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma xAODTracking xAODTrigCalo xAODTrigEgamma 
                     TrigDecisionToolLib TrigConfHLTData TrigSteeringEvent AthenaBaseComps xAODMuon xAODTau xAODTrigger GaudiKernel 
                     TrigEgammaMatchingToolLib AthenaMonitoringLib StoreGateLib)


