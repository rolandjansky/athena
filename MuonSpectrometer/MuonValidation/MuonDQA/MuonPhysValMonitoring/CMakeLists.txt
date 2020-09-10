################################################################################
# Package: MuonPhysValMonitoring
################################################################################

# Declare the package name:
atlas_subdir( MuonPhysValMonitoring )

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

