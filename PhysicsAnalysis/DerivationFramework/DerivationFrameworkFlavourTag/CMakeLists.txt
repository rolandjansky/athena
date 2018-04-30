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
                          Reconstruction/Jet/JetInterface
                          Reconstruction/Jet/BoostedJetTaggers
                          Reconstruction/Jet/JetCalibTools
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          )

# External dependencies:
find_package( COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkFlavourTag
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES GaudiKernel AthenaBaseComps xAODCore xAODJet xAODPFlow
   xAODTracking xAODTrigger xAODTruth JetInterface TrkExInterfaces
   BoostedJetTaggersLib JetCalibToolsLib )


# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

