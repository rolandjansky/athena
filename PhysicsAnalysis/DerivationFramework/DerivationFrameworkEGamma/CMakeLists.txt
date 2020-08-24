# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DerivationFrameworkEGamma )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkEGamma
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools AthContainers AthenaBaseComps CaloCalibHitRecLib DerivationFrameworkInterfaces EgammaAnalysisHelpersLib EgammaAnalysisInterfacesLib ExpressionEvaluationLib GaudiKernel MCTruthClassifierLib PATCoreLib xAODBase xAODCaloEvent xAODEgamma xAODMissingET xAODMuon xAODTracking xAODTruth )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

