################################################################################
# Package: MissingEtDQA
################################################################################

# Declare the package name:
atlas_subdir( MissingEtDQA )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthToolSupport/AsgTools
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTracking
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          PhysicsAnalysis/MuonID/MuonSelectorTools
                          PhysicsAnalysis/TauID/TauAnalysisTools
                          Reconstruction/Jet/JetInterface
                          Reconstruction/MET/METInterface )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MissingEtDQA
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AsgTools AthenaBaseComps AthenaMonitoringLib xAODEgamma xAODJet xAODMissingET xAODMuon xAODTau xAODTracking ElectronPhotonSelectorToolsLib MuonSelectorToolsLib JetInterface METInterface )

