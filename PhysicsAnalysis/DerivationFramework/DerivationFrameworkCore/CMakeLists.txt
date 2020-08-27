# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DerivationFrameworkCore )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkCore
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AsgAnalysisInterfaces AthAnalysisBaseCompsLib AthContainers AthLinks AthenaKernel DerivationFrameworkInterfaces GoodRunsListsLib PathResolver SGTools TrigDecisionToolLib TriggerMatchingToolLib xAODBase xAODEgamma xAODEventInfo xAODMuon )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/frozen_derivation_test.py )
