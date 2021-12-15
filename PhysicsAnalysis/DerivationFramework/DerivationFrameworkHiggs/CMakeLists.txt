# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DerivationFrameworkHiggs )


# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Physics )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkHiggs
   src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel
   xAODEventInfo xAODEgamma xAODJet xAODMuon xAODTracking xAODTruth xAODTrigger xAODPFlow
   FourMomUtils TrkVKalVrtFitterLib TrkVertexFitterInterfaces TrkVertexAnalysisUtilsLib 
   TrigDecisionToolLib VxVertex GammaORToolsLib egammaInterfacesLib PFlowUtilsLib PhotonVertexSelectionLib VrtSecInclusiveLib PFlowUtilsLib
   EgammaAnalysisInterfacesLib DerivationFrameworkInterfaces CaloDetDescrLib
   ExpressionEvaluationLib AthContainers PathResolver
   TruthConvertersLib TruthRivetToolsLib TruthUtils 
   ElectronPhotonSelectorToolsLib PFlowUtilsLib JpsiUpsilonToolsLib)


# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.cfg )
