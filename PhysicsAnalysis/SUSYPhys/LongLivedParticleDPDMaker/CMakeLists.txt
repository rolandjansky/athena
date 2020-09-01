################################################################################
# Package: LongLivedParticleDPDMaker
################################################################################

# Declare the package name:
atlas_subdir( LongLivedParticleDPDMaker )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( LongLivedParticleDPDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps xAODCaloEvent xAODEgamma xAODMuon xAODTracking GaudiKernel MuonSelectorToolsLib TrigDecisionToolLib TrigObjectMatchingLib AthenaKernel EventBookkeeperMetaData FourMomUtils xAODEventInfo xAODJet xAODMissingET DerivationFrameworkInterfaces )

# Install files from the package:
atlas_install_headers( LongLivedParticleDPDMaker )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

