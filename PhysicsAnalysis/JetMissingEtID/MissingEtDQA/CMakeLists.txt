################################################################################
# Package: MissingEtDQA
################################################################################

# Declare the package name:
atlas_subdir( MissingEtDQA )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MissingEtDQA
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AsgTools AthenaBaseComps AthenaMonitoringLib xAODEgamma xAODJet xAODMissingET xAODMuon xAODTau xAODTracking EgammaAnalysisInterfacesLib MuonSelectorToolsLib JetInterface METInterface METUtilitiesLib TauAnalysisToolsLib)

