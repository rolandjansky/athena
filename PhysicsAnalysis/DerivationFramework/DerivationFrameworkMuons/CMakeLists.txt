# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DerivationFrameworkMuons )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkMuons
   DerivationFrameworkMuons/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AsgTools AthContainers AthenaBaseComps AthenaKernel CaloEvent CaloGeoHelpers DerivationFrameworkInterfaces ExpressionEvaluationLib FourMomUtils GaudiKernel ICaloTrkMuIdTools InDetTrackSelectionToolLib MCTruthClassifierLib RecoToolInterfaces TrigDecisionToolLib TrigMuonMatchingLib TrkExInterfaces TrkParameters TrkSurfaces muonEvent xAODBase xAODCaloEvent xAODEventInfo xAODJet xAODMuon xAODPrimitives xAODTracking xAODTruth )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

