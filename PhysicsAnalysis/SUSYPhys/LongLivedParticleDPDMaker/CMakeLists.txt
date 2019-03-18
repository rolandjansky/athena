################################################################################
# Package: LongLivedParticleDPDMaker
################################################################################

# Declare the package name:
atlas_subdir( LongLivedParticleDPDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PhysicsAnalysis/MuonID/MuonSelectorTools
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigAnalysis/TrigObjectMatching
                          PRIVATE
                          Control/AthenaKernel
                          Event/EventBookkeeperMetaData
                          Event/FourMomUtils
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMissingET )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( LongLivedParticleDPDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps xAODCaloEvent xAODEgamma xAODMuon xAODTracking GaudiKernel MuonSelectorToolsLib TrigDecisionToolLib TrigObjectMatchingLib AthenaKernel EventBookkeeperMetaData FourMomUtils xAODEventInfo xAODJet xAODMissingET )

# Install files from the package:
atlas_install_headers( LongLivedParticleDPDMaker )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

