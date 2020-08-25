################################################################################
# Package: MuonTagTools
################################################################################

# Declare the package name:
atlas_subdir( MuonTagTools )

# Component(s) in the package:
atlas_add_library( MuonTagToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonTagTools
                   LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities xAODEventInfo xAODMuon GaudiKernel TagEvent IsolationSelectionLib MuonSelectorToolsLib AnalysisUtilsLib xAODParticleEvent
                   PRIVATE_LINK_LIBRARIES AthContainers xAODTracking )

atlas_add_component( MuonTagTools
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities xAODEventInfo xAODMuon GaudiKernel IsolationSelectionLib TagEvent MuonSelectorToolsLib AthContainers xAODTracking AnalysisUtilsLib MuonTagToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

