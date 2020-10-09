################################################################################
# Package: MuonResonanceTools
################################################################################

# Declare the package name:
atlas_subdir( MuonResonanceTools )

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

