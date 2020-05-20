################################################################################
# Package: DerivationFrameworkFlavourTag
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkFlavourTag )


# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigger
                          Event/xAOD/xAODTruth
                          Event/xAOD/xAODCaloEvent
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/xAODBTaggingEfficiency
                          PhysicsAnalysis/JetTagging/FlavorTagDiscriminants
                          Reconstruction/Jet/JetInterface
                          Reconstruction/Jet/BoostedJetTaggers
                          Reconstruction/Jet/JetCalibTools
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          )

# Component(s) in the package:
atlas_add_library(   DerivationFrameworkFlavourTagLib
                     src/*.cxx
		     PUBLIC_HEADERS DerivationFrameworkFlavourTag
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps xAODCore xAODJet xAODPFlow
                     xAODTracking xAODTrigger xAODTruth JetInterface TrkExInterfaces
                     JetCalibToolsLib FlavorTagDiscriminants )

atlas_add_component( DerivationFrameworkFlavourTag
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps xAODCore xAODJet xAODPFlow
                     xAODTracking xAODTrigger xAODTruth JetInterface TrkExInterfaces
                     JetCalibToolsLib DerivationFrameworkFlavourTagLib )


# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

# add the test utility
atlas_add_executable( test_btagging util/test-btagging.cxx )
atlas_add_executable( test_jet_links util/test-jet-links.cxx )
target_link_libraries(test_btagging xAODCore xAODJet xAODRootAccess)
target_link_libraries(test_jet_links xAODCore xAODJet xAODRootAccess)

