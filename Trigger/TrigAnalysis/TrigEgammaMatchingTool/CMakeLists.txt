################################################################################
# Package: TrigEgammaMatchingTool
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaMatchingTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODTrigEgamma
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTrigger
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigEgammaMatchingToolLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS TrigEgammaMatchingTool
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools xAODCaloEvent xAODEgamma xAODTracking xAODTrigCalo xAODTrigEgamma TrigConfHLTData TrigSteeringEvent TrigDecisionToolLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODMuon xAODTau xAODTrigger GaudiKernel )

atlas_add_component( TrigEgammaMatchingTool
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma xAODTracking xAODTrigCalo xAODTrigEgamma TrigDecisionToolLib TrigConfHLTData TrigSteeringEvent AthenaBaseComps xAODMuon xAODTau xAODTrigger GaudiKernel TrigEgammaMatchingToolLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

