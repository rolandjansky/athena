# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DerivationFrameworkMuons )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
#the library is used by the MCP software https://gitlab.cern.ch/atlas-mcp/MuonPerformanceAnalysis
atlas_add_library( DerivationFrameworkMuonsLib
   DerivationFrameworkMuons/*.h src/*.cxx
   PUBLIC_HEADERS DerivationFrameworkMuons
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools AthContainers AthenaBaseComps AthenaKernel CaloEvent CaloGeoHelpers DerivationFrameworkInterfaces ExpressionEvaluationLib FourMomUtils GaudiKernel ICaloTrkMuIdTools InDetTrackSelectionToolLib MCTruthClassifierLib RecoToolInterfaces TrigDecisionToolLib TrigMuonMatchingLib TrkExInterfaces TrkParameters TrkSurfaces muonEvent xAODBase xAODCaloEvent xAODEventInfo xAODJet xAODMuon xAODPrimitives xAODTracking xAODTruth )

atlas_add_component( DerivationFrameworkMuons
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel DerivationFrameworkMuonsLib )
# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

