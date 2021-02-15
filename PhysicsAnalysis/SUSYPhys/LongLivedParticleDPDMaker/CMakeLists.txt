# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LongLivedParticleDPDMaker )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( LongLivedParticleDPDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODCaloEvent xAODEgamma xAODMuon xAODTracking GaudiKernel MuonAnalysisInterfacesLib TrigDecisionToolLib TrigObjectMatchingLib EventBookkeeperMetaData FourMomUtils xAODJet xAODMissingET DerivationFrameworkInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

