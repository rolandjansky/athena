################################################################################
# Package: MuonResonanceTools
################################################################################
 
# Declare the package name:
atlas_subdir( MuonResonanceTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/EventPrimitives
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces
                          PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonEfficiencyCorrections
                          PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonMomentumCorrections
                          PhysicsAnalysis/MuonID/MuonSelectorTools
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigAnalysis/TrigMuonMatching
                          Tracking/TrkValidation/TrkValHistUtils
                          PRIVATE
                          Event/xAOD/xAODEventInfo
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( MuonResonanceToolsLib
                   Root/*.cxx
                   PUBLIC_HEADERS MuonResonanceTools
                   LINK_LIBRARIES AsgTools EventPrimitives xAODBase xAODMuon xAODTracking PATInterfaces MuonEfficiencyCorrectionsLib MuonMomentumCorrectionsLib 
                   xAODTruth MuonSelectorToolsLib TrigDecisionToolLib TrigMuonMatchingLib TrkValHistUtils
                   PRIVATE_LINK_LIBRARIES xAODEventInfo GaudiKernel )

atlas_add_component( MuonResonanceTools
                     src/components/*.cxx
                     LINK_LIBRARIES AsgTools EventPrimitives xAODBase xAODMuon xAODTracking PATInterfaces MuonEfficiencyCorrectionsLib 
                     MuonMomentumCorrectionsLib MuonSelectorToolsLib TrigDecisionToolLib TrigMuonMatchingLib xAODEventInfo GaudiKernel MuonResonanceToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

