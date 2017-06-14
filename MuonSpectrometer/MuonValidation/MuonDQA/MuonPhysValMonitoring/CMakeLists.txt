################################################################################
# Package: MuonPhysValMonitoring
################################################################################

# Declare the package name:
atlas_subdir( MuonPhysValMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaMonitoring
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigMuon
                          Event/xAOD/xAODTrigger
                          Event/xAOD/xAODTruth
                          MuonSpectrometer/MuonValidation/MuonHistogramming/MuonHistUtils
                          PhysicsAnalysis/AnalysisCommon/IsolationSelection
                          PhysicsAnalysis/MuonID/MuonSelectorTools
                          Reconstruction/MuonIdentification/MuonCombinedToolInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkValidation/TrkValHistUtils
                          Trigger/TrigAnalysis/TrigDecisionTool )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonPhysValMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaMonitoringLib xAODBase xAODCore xAODEventInfo xAODMuon xAODTracking xAODTrigMuon xAODTrigger xAODTruth MuonHistUtils IsolationSelectionLib MuonSelectorToolsLib MuonCombinedToolInterfaces TrkToolInterfaces TrkValHistUtils TrigDecisionToolLib )

# Install files from the package:
atlas_install_scripts( macros/MuonValidation_*.py macros/MuonValidation_*.sh )

